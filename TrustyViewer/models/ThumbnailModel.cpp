
#include <QMimeData>
#include <QDataStream>
#include <QTimer>
#include <QColor>
#include <QFileInfo>
#include <QPainter>

#include "../widgets/DragDropView.h"
#include "../MediaDatabase.h"
#include "../Utils.h"
#include "../ImageUtils.h"

#include "ThumbnailModel.h"

namespace realn {
  const QString ITEM_MIME_TYPE = "application/trust.view.item.list";

  ThumbnailModel::ThumbnailModel(std::shared_ptr<MediaDatabase> _database, std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<ThumbnailWorker> _worker, std::shared_ptr<MediaItemStorage> storage, DragDropView* dragDropView, QSize _thumbnailSize)
    : database(_database)
    , plugins(_plugins)
    , worker(_worker)
    , itemStorage(storage)
    , thumbnailSize(_thumbnailSize)
    , view(dragDropView) {
    defaultThumbnail = QPixmap(thumbnailSize);
    defaultThumbnail.fill(QColor(Qt::lightGray));

    folderThumbnail = createFolderThumbnail();

    cC(connect(database.get(), &MediaDatabase::itemWillBeRemoved, this, &ThumbnailModel::removeItem));
    cC(connect(database.get(), &MediaDatabase::itemWillBeMoved, this, &ThumbnailModel::moveItem));
    cC(connect(database.get(), &MediaDatabase::itemsWillBeMoved, this, &ThumbnailModel::moveItems));

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

  void ThumbnailModel::updateItemIndices() {
    //std::sort(items.begin(), items.end(), ThumbnailModelItem::AscTypeNameSorter);
    for (size_t i = 0; i < items.size(); i++) {
      items[i]->setIndex(i);
    }
  }

  ThumbnailModelItem::ptr_t ThumbnailModel::fromIndex(const QModelIndex& index) const {
    if (!index.isValid())
      return nullptr;
    auto ptr = reinterpret_cast<ThumbnailModelItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }

  QModelIndex ThumbnailModel::toIndex(ThumbnailModelItem::ptr_t item) {
    auto it = std::find(items.begin(), items.end(), item);
    if (it == items.end())
      return QModelIndex();

    auto rowIndex = it - items.begin();
    return createIndex(static_cast<int>(rowIndex), 0, item.get());
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
    if (parent.isValid() || !rootItem || column != 0)
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
      if (item->isDirectory())
        return folderThumbnail;
      if (!item->hasThumbnail())
        return defaultThumbnail;
      return item->getThumbnail();
    }

    return QVariant();
  }

  Qt::DropActions ThumbnailModel::supportedDropActions() const {
    return Qt::DropActions() | Qt::MoveAction;
  }

  Qt::ItemFlags ThumbnailModel::flags(const QModelIndex& index) const {
    auto flags = QAbstractItemModel::flags(index);
    auto item = fromIndex(index);
    if (!item)
      return flags;

    if (item->isDirectory())
      return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

    return flags | Qt::ItemIsDragEnabled;
  }

  QStringList ThumbnailModel::mimeTypes() const {
    return QStringList()
      << ITEM_MIME_TYPE;
  }

  QMimeData* ThumbnailModel::mimeData(const QModelIndexList& indices) const {

    QByteArray data;
    auto stream = QDataStream(&data, QIODevice::WriteOnly);

    for (auto& index : indices) {
      auto item = fromIndex(index);
      if (!item)
        continue;

      auto id = itemStorage->store(item->getMediaItem());
      stream << id;
    }

    auto result = new QMimeData();
    result->setData(ITEM_MIME_TYPE, data);
    return result;
  }

  bool ThumbnailModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {
    if (action != Qt::MoveAction)
      return false;
    if (!data->hasFormat(ITEM_MIME_TYPE))
      return false;

    auto idx = index(row, column, parent);
    if (!idx.isValid()) {
      idx = view->getPointedIndex();
      if (!idx.isValid())
        return false;
    }

    auto item = fromIndex(idx);
    if (!item)
      return false;

    if (!item->isDirectory())
      return false;

    return true;
  }

  bool ThumbnailModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {
    if (action == Qt::IgnoreAction)
      return true;
    if (!canDropMimeData(data, action, row, column, parent))
      return false;

    auto idx = index(row, column, parent);
    if (!idx.isValid()) {
      idx = view->getPointedIndex();
      if (!idx.isValid())
        return false;
    }

    auto targetItem = fromIndex(idx);
    if (!targetItem)
      return false;

    auto newParent = targetItem->getMediaItem();

    auto encData = data->data(ITEM_MIME_TYPE);
    auto stream = QDataStream(&encData, QIODevice::ReadOnly);

    auto items = MediaItem::itemvector_t();
    while (!stream.atEnd()) {
      MediaItemStorage::item_id itemId;
      stream >> itemId;

      auto item = itemStorage->retrieve(itemId);
      if (item)
        items.push_back(item);
    }

    database->moveItems(items, newParent);
    //for (auto& item : items) {
    //  database->moveItem(item, newParent);
    //}

    return true;
  }

  void ThumbnailModel::removeItem(MediaItem::ptr_t item) {
    if (item == rootItem) {
      setRootItem(nullptr);
      return;
    }

    if (item->getParent() != rootItem)
      return;

    auto modelIdx = getIndexForItem(item);
    if (!modelIdx.isValid())
      return;

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

  void ThumbnailModel::removeItems(MediaItem::itemvector_t mediaItems) {

    auto modelItems = std::vector<ThumbnailModelItem::ptr_t>();
    for (auto& item : mediaItems) {
      if (item == rootItem || item->getParent() != rootItem)
        continue;

      auto modelIdx = getIndexForItem(item);
      if (modelIdx.isValid()) {
        auto modelItem = fromIndex(modelIdx);
        modelItems.push_back(modelItem);
      }
    }
    if (modelItems.empty())
      return;

    updateItemIndices();
    std::sort(modelItems.begin(), modelItems.end(), [this](const ThumbnailModelItem::ptr_t& item1, const ThumbnailModelItem::ptr_t& item2) {
      return item1->getIndex() < item2->getIndex();
              });


    do {
      updateItemIndices();

      auto it = modelItems.begin();

      auto beg = it;
      auto last = it;
      it++;

      while (it != modelItems.end() && (*last)->getIndex() + 1 == (*it)->getIndex()) {
        last = it;
        it++;
      }

      beginRemoveRows(QModelIndex(), (*beg)->getIndex(), (*last)->getIndex());

      auto itemFirst = std::find(items.begin(), items.end(), *beg);
      auto itemLast = std::find(items.begin(), items.end(), *last) + 1;

      for (auto dit = beg; dit != last + 1; dit++) {
        auto item = *dit;

        auto idx = item->getIndex();
        {
          ThumbnailModelItem::ptr_t temp;
          std::swap(temp, items[idx]);
          std::thread([](ThumbnailModelItem::ptr_t memory) { memory.reset(); }, std::move(temp)).detach();
        }
      }

      items.erase(itemFirst, itemLast);

      endRemoveRows();

      modelItems.erase(beg, last + 1);

    } while (!modelItems.empty());
  }

  void ThumbnailModel::moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    if (newParent == rootItem)
      return;

    removeItem(item);
  }

  void ThumbnailModel::moveItems(MediaItem::itemvector_t items, MediaItem::ptr_t newParent) {
    if (newParent == rootItem)
      return;

    removeItems(items);
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

  QPixmap ThumbnailModel::createFolderThumbnail() {
    auto image = loadScaled("assets/folder_thumbnail.png", thumbnailSize, defaultThumbnail);
    return createFilledThumbnail(image, thumbnailSize);
  }
}