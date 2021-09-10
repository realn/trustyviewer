
#include <QColor>
#include <QFileInfo>

#include "ThumbnailModel.h"

namespace realn {
  ThumbnailModel::ThumbnailModel(std::shared_ptr<ExtPluginList> _plugins, QSize _thumbnailSize)
    : plugins(_plugins)
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

  QModelIndex ThumbnailModel::index(int row, int column, const QModelIndex& parent) const
  {
    if (parent.isValid() || !rootItem)
      return QModelIndex();

    auto item = rootItem->getChild(static_cast<size_t>(row));
    if (!item)
      return QModelIndex();

    return createIndex(row, column);
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
    thumbnails.clear();
    for (auto& item : rootItem->getChildren()) {
      if (item->isDirectory())
        continue;

      auto plugin = plugins->getPluginForExt(QFileInfo(item->getFilePath()).completeSuffix());
      if (!plugin)
        continue;

      auto thumbnail = plugin->createThumbnail(item->getFilePath(), thumbnailSize);
      if (!thumbnail)
        continue;

      thumbnails[item->getFilePath()] = std::move(thumbnail);
    }
  }

  QPixmap ThumbnailModel::getThumbnail(const QString& filepath) const
  {
    if (thumbnails.count(filepath) == 0)
      return defaultThumbnail;
    return *thumbnails.at(filepath);
  }

  //QVariant ThumbnailModel::headerData(int section, Qt::Orientation orientation, int role) const
  //{
  //  if(section != 0 || orientation != Qt::Horizontal || role != Qt::DisplayRole)
  //    return QVariant();
  //  return "Name";
  //}
}