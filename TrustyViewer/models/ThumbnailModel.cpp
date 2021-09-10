

#include <QFileInfo>

#include "ThumbnailModel.h"

namespace realn {
  ThumbnailModel::ThumbnailModel()
  {
  }

  void ThumbnailModel::setRootItem(MediaItem::ptr_t item)
  {
    if (rootItem == item)
      return;
    beginResetModel();
    rootItem = item;
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

    return QVariant();
  }

  //QVariant ThumbnailModel::headerData(int section, Qt::Orientation orientation, int role) const
  //{
  //  if(section != 0 || orientation != Qt::Horizontal || role != Qt::DisplayRole)
  //    return QVariant();
  //  return "Name";
  //}
}