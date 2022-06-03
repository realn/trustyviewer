
#include <QTimer>
#include <QColor>
#include <QFileInfo>
#include <QPainter>

#include "../Utils.h"

#include "ThumbnailModel.h"

namespace realn {
  ThumbnailModel::ThumbnailModel(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<ThumbnailWorker> _worker, QSize _thumbnailSize)
    : plugins(_plugins)
    , worker(_worker)
    , thumbnailSize(_thumbnailSize) {
    defaultThumbnail = QPixmap(thumbnailSize);
    defaultThumbnail.fill(QColor(Qt::lightGray));

    worker->setTargetThumbnailSize(_thumbnailSize);
  }

  void ThumbnailModel::setRootItem(MediaItem::ptr_t item) {
    if (rootItem == item)
      return;
    beginResetModel();
    rootItem = item;

    {
      items_t temp;
      std::swap(temp, items);
      std::thread([](items_t&& memory) { memory.clear(); }, std::move(temp)).detach();
    }

    if (rootItem) {
      for (auto& dbItem : rootItem->getChildren()) {
        auto modelItem = std::make_shared<ThumbnailModelItem>(dbItem);
        items.push_back(modelItem);
      }
    }

    std::sort(items.begin(), items.end(), ThumbnailModelItem::AscTypeNameSorter);

    createThumbnails();
    endResetModel();
  }

  MediaItem::ptr_t ThumbnailModel::getRootItem() const {
    return rootItem;
  }

  ThumbnailModelItem::ptr_t ThumbnailModel::fromIndex(const QModelIndex& index) const {
    auto ptr = reinterpret_cast<ThumbnailModelItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }

  void ThumbnailModel::refreshModel() {
    beginResetModel();
    endResetModel();
  }

  MediaItem::ptr_t ThumbnailModel::getMediaItemForIndex(QModelIndex index) const {
    auto item = fromIndex(index);
    if (item)
      return item->getMediaItem();
    return nullptr;
  }

  QModelIndex ThumbnailModel::getIndexForItem(MediaItem::ptr_t item) const {
    if (!rootItem)
      return QModelIndex();

    if (rootItem == item) {
      return QModelIndex();
    }

    auto it = find_if(items, [&](ThumbnailModelItem::ptr_t modelItem) {return modelItem->getMediaItem() == item; });
    if (it == items.end())
      return QModelIndex();

    auto rowIndex = it - items.begin();
    return index(static_cast<int>(rowIndex), 0);
  }

  QModelIndex ThumbnailModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() || !rootItem)
      return QModelIndex();

    auto modelIdx = static_cast<size_t>(row);
    if (modelIdx >= items.size())
      return QModelIndex();

    return createIndex(row, column, items[modelIdx].get());
  }

  QModelIndex ThumbnailModel::parent(const QModelIndex& child) const {
    return QModelIndex();
  }

  int ThumbnailModel::rowCount(const QModelIndex& parent) const {
    if (!rootItem || parent.isValid())
      return 0;
    return static_cast<int>(items.size());
  }

  int ThumbnailModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : 1;
  }

  QVariant ThumbnailModel::data(const QModelIndex& index, int role) const {
    if (!rootItem)
      return QVariant();

    auto modelIdx = static_cast<size_t>(index.row());
    if (modelIdx >= items.size())
      return QVariant();

    auto item = items[modelIdx];

    if (role == Qt::DisplayRole)
      return item->getName();
    if (role == Qt::DecorationRole) {
      if (!item->hasThumbnail())
        return defaultThumbnail;
      return item->getThumbnail();
    }

    return QVariant();
  }

  void ThumbnailModel::removeItem(MediaItem::ptr_t item) {
    if (item == rootItem) {
      setRootItem(nullptr);
      return;
    }

    if (item->getParent() != rootItem)
      return;

    auto modelIdx = getIndexForItem(item);
    auto modelParent = parent(modelIdx);

    beginRemoveRows(modelParent, modelIdx.row(), modelIdx.row());

    auto idx = static_cast<size_t>(modelIdx.row());
    {
      ThumbnailModelItem::ptr_t temp;
      std::swap(temp, items[idx]);
      std::thread([](ThumbnailModelItem::ptr_t memory) { memory.reset(); }, std::move(temp)).detach();
    }
    items.erase(items.begin() + idx);

    endRemoveRows();
  }

  void ThumbnailModel::moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    if (newParent == rootItem)
      return;

    removeItem(item);
  }

  void ThumbnailModel::createThumbnails() {
    worker->clearRequests();

    if (rootItem) {
      for (auto& item : items) {
        if (item->isDirectory())
          continue;
        if (item->hasThumbnail())
          continue;
        worker->addThumbnailRequest(item->getFilepath());
      }
    }

    QTimer::singleShot(std::chrono::milliseconds(1000), this, &ThumbnailModel::retrieveThumbnails);
  }

  void ThumbnailModel::retrieveThumbnails() {
    if (worker->hasDoneThumbnails()) {
      auto result = worker->popDoneThumbnails();

      QStringList changed;
      for (auto& item : items) {
        if (item->isDirectory() || item->hasThumbnail())
          continue;

        auto it = std::find_if(result.begin(), result.end(), [&](auto& resItem) { return resItem.filePath == item->getFilepath(); });
        if (it == result.end())
          continue;

        item->setThumbnail(std::move(it->thumbnail));
        changed << it->filePath;
      }

      emitThumbnailsDataChanged(changed);
    }
    QTimer::singleShot(std::chrono::milliseconds(1000), this, &ThumbnailModel::retrieveThumbnails);
  }

  QPixmap ThumbnailModel::getThumbnail(const QString& filepath) const {
    if (thumbnails.count(filepath) == 0)
      return defaultThumbnail;
    return *thumbnails.at(filepath);
  }

  void ThumbnailModel::emitThumbnailsDataChanged(QStringList changedItems) {
    if (changedItems.empty())
      return;
    size_t first = find_index_if(items, [&](const ThumbnailModelItem::ptr_t& mitem) { return mitem->getFilepath() == changedItems.front(); }, 0);
    changedItems.pop_front();
    size_t last = first;
    for (auto& item : changedItems) {
      auto idx = find_index_if(items, [&](const ThumbnailModelItem::ptr_t& mitem) { return mitem->getFilepath() == item; }, 0);
      first = std::min(first, idx);
      last = std::max(last, idx);
    }

    auto top = index(static_cast<int>(first), 0);
    auto bottom = index(static_cast<int>(last), 0);
    QVector<int> roles = { Qt::DecorationRole };
    emit dataChanged(top, bottom, roles);
  }
}