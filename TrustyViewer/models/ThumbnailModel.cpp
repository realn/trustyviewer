
#include <QTimer>
#include <QColor>
#include <QFileInfo>
#include <QPainter>

#include "../Utils.h"

#include "ThumbnailModel.h"

namespace realn {
  namespace {
    QPoint toPoint(const QSize& size) {
      return QPoint(size.width(), size.height());
    }
  }

  ThumbnailModel::ThumbnailModel(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<ThumbnailWorker> _worker, QSize _thumbnailSize)
    : plugins(_plugins)
    , worker(_worker)
    , thumbnailSize(_thumbnailSize)
  {
    defaultThumbnail = QPixmap(thumbnailSize);
    defaultThumbnail.fill(QColor(Qt::lightGray));
  }

  void ThumbnailModel::setRootItem(MediaItem::ptr_t item)
  {
    if (rootItem == item)
      return;
    beginResetModel();
    rootItem = item;
    createThumbnails();
    endResetModel();
  }

  MediaItem::ptr_t ThumbnailModel::fromIndex(const QModelIndex& index) const
  {
    auto ptr = reinterpret_cast<MediaItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }

  QModelIndex ThumbnailModel::getIndexForItem(MediaItem::ptr_t item) const
  {
    if (!rootItem)
      return QModelIndex();

    if (rootItem == item) {
      if (rootItem->getChildren().size() > 0)
        return index(0, 0);
      return QModelIndex();
    }

    auto& list = rootItem->getChildren();
    auto it = std::find(list.begin(), list.end(), item);
    if (it == list.end())
      return QModelIndex();

    auto rowIndex = it - list.begin();
    return index(static_cast<int>(rowIndex), 0);
  }

  QModelIndex ThumbnailModel::index(int row, int column, const QModelIndex& parent) const
  {
    if (parent.isValid() || !rootItem)
      return QModelIndex();

    auto item = rootItem->getChild(static_cast<size_t>(row));
    if (!item)
      return QModelIndex();

    return createIndex(row, column, item.get());
  }

  QModelIndex ThumbnailModel::parent(const QModelIndex& child) const
  {
    return QModelIndex();
  }

  int ThumbnailModel::rowCount(const QModelIndex& parent) const
  {
    if (!rootItem || parent.isValid())
      return 0;
    return static_cast<int>(rootItem->getChildren().size());
  }

  int ThumbnailModel::columnCount(const QModelIndex& parent) const
  {
    return parent.isValid() ? 0 : 1;
  }

  QVariant ThumbnailModel::data(const QModelIndex& index, int role) const
  {
    if (!rootItem)
      return QVariant();

    auto item = rootItem->getChild(static_cast<size_t>(index.row()));
    if (!item)
      return QVariant();

    QFileInfo info(item->getFilePath());

    if (role == Qt::DisplayRole)
      return info.fileName();
    if (role == Qt::DecorationRole)
      return getThumbnail(item->getFilePath());

    return QVariant();
  }

  void ThumbnailModel::createThumbnails()
  {
    worker->clearRequests();

    {
      thumbnail_map_t temp;
      std::swap(temp, thumbnails);
      std::thread([](thumbnail_map_t&& memory) { memory.clear(); }, std::move(temp)).detach();
    }

    for (auto& item : rootItem->getChildren()) {
      if (item->isDirectory())
        continue;

      worker->addThumbnailRequest(item->getFilePath());
    }
    QTimer::singleShot(std::chrono::milliseconds(1000), this, &ThumbnailModel::retrieveThumbnails);
  }

  void ThumbnailModel::retrieveThumbnails() {
    if (worker->hasDoneThumbnails()) {
      auto result = worker->popDoneThumbnails();

      QStringList changed;
      for (auto& item : result) {
        thumbnails[item.filePath] = createCorrectThumbnail(*item.thumbnail);
        changed << item.filePath;
      }

      emitThumbnailsDataChanged(changed);
    }
    QTimer::singleShot(std::chrono::milliseconds(1000), this, &ThumbnailModel::retrieveThumbnails);
  }

  QPixmap ThumbnailModel::getThumbnail(const QString& filepath) const
  {
    if (thumbnails.count(filepath) == 0)
      return defaultThumbnail;
    return *thumbnails.at(filepath);
  }

  void ThumbnailModel::emitThumbnailsDataChanged(QStringList items)
  {
    if (items.empty())
      return;
    size_t first = find_index_if(rootItem->getChildren(), [&](const MediaItem::ptr_t& mitem) { return mitem->getFilePath() == items.front(); }, 0);
    items.pop_front();
    size_t last = first;
    for (auto& item : items) {
      auto idx = find_index_if(rootItem->getChildren(), [&](const MediaItem::ptr_t& mitem) { return mitem->getFilePath() == item; }, 0);
      first = std::min(first, idx);
      last = std::max(last, idx);
    }
    
    auto top = index(static_cast<int>(first), 0);
    auto bottom = index(static_cast<int>(last), 0);
    QVector<int> roles = { Qt::DecorationRole };
    emit dataChanged(top, bottom, roles);
  }

  ThumbnailModel::pixmap_ptr_t ThumbnailModel::createCorrectThumbnail(const QPixmap& pixmap) const {
    auto thumbnail = QPixmap(defaultThumbnail.size());
    thumbnail.fill(QColor::fromRgb(230, 230, 230));

    auto pos = (toPoint(thumbnail.size()) - toPoint(pixmap.size())) / 2;
    {
      QPainter pnt(&thumbnail);
      pnt.drawPixmap(pos, pixmap);
    }
    
    return std::make_unique<QPixmap>(std::move(thumbnail));
  }
}