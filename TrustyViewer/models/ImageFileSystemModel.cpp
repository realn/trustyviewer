
#include "ImageFileSystemModel.h"

namespace realn {
  ImageFileSystemModel::ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase)
    : database(mediaDatabase)
  {
    iconProvider = std::make_unique<QFileIconProvider>();
  }

  MediaItem::ptr_t ImageFileSystemModel::getItemForIndex(const QModelIndex& index) const {
    if (!index.isValid())
      return nullptr;

    auto item = fromIndex(index);
    return item;

  }

  QModelIndex ImageFileSystemModel::getIndexForItem(MediaItem::ptr_t item) const
  {
    if(!isItemOfRoot(item))
      return QModelIndex();

    if (item == database->getRootItem())
      return index(0, 0);

    int row = static_cast<int>(item->getIndexFromParent());
    return index(row, 0, getIndexForItem(item->getParent()));
  }

  QModelIndex ImageFileSystemModel::index(int row, int column, const QModelIndex& parent) const
  {
    if (column >= columnCount())
      return QModelIndex();

    if (!parent.isValid()) {
      if (row != 0)
        return QModelIndex();

      return createIndex(row, column, database->getRootItem().get());
    }
      
    auto parentItem = fromIndex(parent);
    auto rowIndex = static_cast<size_t>(row);
    
    if (rowIndex >= parentItem->getChildren().size()) {
      return QModelIndex();
    }

    return createIndex(row, column, parentItem->getChildren()[rowIndex].get());
  }

  QModelIndex ImageFileSystemModel::parent(const QModelIndex& child) const
  {
    if (!child.isValid())
      return QModelIndex();

    auto item = fromIndex(child);
    auto parentLock = item->getParent();
    if (!parentLock)
      return QModelIndex();

    return createIndex(0, 0, parentLock.get());
  }

  int ImageFileSystemModel::rowCount(const QModelIndex& parent) const
  {
    if (!parent.isValid()) {
      if (database->getRootItem())
        return 1;
      return 0;
    }
    auto parentItem = fromIndex(parent);
    if (!parentItem)
      return 0;

    return static_cast<int>(parentItem->getChildren().size());
  }

  int ImageFileSystemModel::columnCount(const QModelIndex& parent) const
  {
    return 1;
  }

  QVariant ImageFileSystemModel::data(const QModelIndex& index, int role) const
  {
    auto item = fromIndex(index);

    QFileInfo info(item->getFilePath());

    if (index.column() == 0 && role == Qt::DecorationRole)
      return iconProvider->icon(info);
    if(index.column() == 0 && role == Qt::DisplayRole)
      return info.fileName();

    return QVariant();
  }

  QVariant ImageFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (orientation != Qt::Horizontal)
      return QVariant();

    return "Name";
  }

  void ImageFileSystemModel::reloadDatabase() {
    beginResetModel();

    endResetModel();
  }

  MediaItem::ptr_t ImageFileSystemModel::fromIndex(const QModelIndex& index)
  {
    auto ptr = reinterpret_cast<MediaItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }
  
  bool ImageFileSystemModel::isItemOfRoot(MediaItem::ptr_t item) const
  {
    auto check = item;
    do {
      if (check == database->getRootItem())
        return true;

      check = check->getParent();
    } while (check);

    return false;
  }
}