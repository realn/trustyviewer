
#include "ImageFileSystemModel.h"

namespace realn {
  ImageFileSystemModel::ImageFileSystemModel()
  {
    iconProvider = std::make_unique<QFileIconProvider>();
  }

  void ImageFileSystemModel::setRootPath(QString path)
  {
    beginResetModel();
    rootItem = scanDir(path, nullptr);
    endResetModel();
  }

  QFileInfo ImageFileSystemModel::getFileInfoForIndex(const QModelIndex& index)
  {
    if (!index.isValid())
      return QFileInfo();

    auto item = fromIndex(index);
    return item->info;
  }

  QModelIndex ImageFileSystemModel::index(int row, int column, const QModelIndex& parent) const
  {
    if (column >= columnCount())
      return QModelIndex();

    if (!parent.isValid()) {
      if (row != 0)
        return QModelIndex();

      return createIndex(row, column, rootItem.get());
    }
      
    auto parentItem = fromIndex(parent);
    auto rowIndex = static_cast<size_t>(row);
    
    if (rowIndex >= parentItem->children.size()) {
      return QModelIndex();
    }

    return createIndex(row, column, parentItem->children[rowIndex].get());
  }

  QModelIndex ImageFileSystemModel::parent(const QModelIndex& child) const
  {
    if (!child.isValid())
      return QModelIndex();

    auto item = fromIndex(child);
    auto parentLock = item->parent.lock();
    if (!parentLock)
      return QModelIndex();

    return createIndex(0, 0, parentLock.get());
  }

  int ImageFileSystemModel::rowCount(const QModelIndex& parent) const
  {
    if (!parent.isValid())
      return 1;
    auto parentItem = fromIndex(parent);
    if (!parentItem)
      return 0;

    return static_cast<int>(parentItem->children.size());
  }

  int ImageFileSystemModel::columnCount(const QModelIndex& parent) const
  {
    return 1;
  }

  QVariant ImageFileSystemModel::data(const QModelIndex& index, int role) const
  {
    auto item = fromIndex(index);

    if (index.column() == 0 && role == Qt::DecorationRole)
      return iconProvider->icon(item->info);
    if(index.column() == 0 && role == Qt::DisplayRole)
      return item->info.fileName();

    return QVariant();
  }

  QVariant ImageFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (orientation != Qt::Horizontal)
      return QVariant();

    return "Name";
  }

  ImageFileSystemModel::ItemPtr ImageFileSystemModel::scanDir(QString path, ItemPtr parent)
  {
    auto item = std::make_shared<Item>();

    item->parent = parent;
    item->info = QFileInfo(path);

    if (item->info.isDir()) {
      item->type = Item::Type::Directory;

      auto dir = QDir(path, "*", QDir::Type | QDir::IgnoreCase, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

      int idx = 0;
      for (auto& dirName : dir.entryList()) {
        auto subPath = dir.filePath(dirName);

        auto child = scanDir(subPath, item);
        idx++;
        item->children.push_back(child);
      }
    }

    return item;
  }

  ImageFileSystemModel::ItemPtr ImageFileSystemModel::fromIndex(const QModelIndex& index)
  {
    return reinterpret_cast<Item*>(index.internalPointer())->ptr();
  }
}