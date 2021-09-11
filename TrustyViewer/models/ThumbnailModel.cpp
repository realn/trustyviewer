
#include <QTimer>
#include <QColor>
#include <QFileInfo>

#include "ThumbnailModel.h"

namespace realn {
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
    thumbnails.clear();
    for (auto& item : rootItem->getChildren()) {
      if (item->isDirectory())
        continue;

      worker->addThumbnailRequest(item->getFilePath());
    }
    QTimer::singleShot(std::chrono::milliseconds(500), this, &ThumbnailModel::retrieveThumbnails);
  }

  void ThumbnailModel::retrieveThumbnails() {
    if (worker->hasDoneThumbnails()) {
      auto result = worker->popDoneThumbnails();

      for (auto& item : result) {
        thumbnails[item.filePath] = std::move(item.thumbnail);
      }

      emitThumbnailsDataChanged();
    }
    QTimer::singleShot(std::chrono::milliseconds(500), this, &ThumbnailModel::retrieveThumbnails);
  }

  QPixmap ThumbnailModel::getThumbnail(const QString& filepath) const
  {
    if (thumbnails.count(filepath) == 0)
      return defaultThumbnail;
    return *thumbnails.at(filepath);
  }

  void ThumbnailModel::emitThumbnailsDataChanged()
  {
    auto top = index(0, 0);
    auto bottom = index(rowCount() - 1, 0);
    QVector<int> roles = { Qt::DecorationRole };
    emit dataChanged(top, bottom, roles);
  }

  //QVariant ThumbnailModel::headerData(int section, Qt::Orientation orientation, int role) const
  //{
  //  if(section != 0 || orientation != Qt::Horizontal || role != Qt::DisplayRole)
  //    return QVariant();
  //  return "Name";
  //}
}