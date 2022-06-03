
#include "ImageFileSystemModel.h"

namespace realn {
  ImageFileSystemModel::ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase, MediaItemFilter itemFilter)
    : database(mediaDatabase), filter(itemFilter) {
    iconProvider = std::make_unique<QFileIconProvider>();

    reloadDatabase();
  }

  MediaItem::ptr_t ImageFileSystemModel::getItemForIndex(const QModelIndex& index) const {
    if (!index.isValid())
      return nullptr;

    auto item = fromIndex(index);
    if(item)
      return item->getMediaItem();
    return nullptr;
  }

  MediaItem::itemvector_t ImageFileSystemModel::getItemsForIndices(const QModelIndexList& indices) const {
    auto result = MediaItem::itemvector_t();
    for (auto& index : indices) {
      auto item = fromIndex(index);
      if (!item)
        continue;
      result.push_back(item->getMediaItem());
    }
    return result;
  }

  QModelIndex ImageFileSystemModel::getIndexForItem(MediaItem::ptr_t item) const {
    if (!root)
      return QModelIndex();

    if (item == root->getMediaItem())
      return index(0, 0);

    auto modelItem = findItemForMediaItem(item);
    if (!modelItem)
      return QModelIndex();

    int row = getIndexFromParent(modelItem);
    return index(row, 0, getIndexForItem(item->getParent()));
  }

  QModelIndex ImageFileSystemModel::index(int row, int column, const QModelIndex& parent) const {
    if (column >= columnCount())
      return QModelIndex();

    if (!parent.isValid()) {
      return createIndex(row, column, root.get());
    }

    auto parentItem = fromIndex(parent);

    if (row >= getChildrenCount(parentItem)) {
      return QModelIndex();
    }

    auto child = getChild(row, parentItem);
    assert(child);

    return createIndex(row, column, child.get());
  }

  QModelIndex ImageFileSystemModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
      return QModelIndex();

    auto item = fromIndex(child);
    assert(item);

    auto parentLock = item->getParent();
    if (!parentLock)
      return QModelIndex();

    return createIndex(0, 0, parentLock.get());
  }

  int ImageFileSystemModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) {
      if (database->getRootItem())
        return 1;
      return 0;
    }
    auto parentItem = fromIndex(parent);
    if (!parentItem)
      return 0;

    return getChildrenCount(parentItem);
  }

  int ImageFileSystemModel::columnCount(const QModelIndex& parent) const {
    return 1;
  }

  QVariant ImageFileSystemModel::data(const QModelIndex& index, int role) const {
    auto item = fromIndex(index);

    if (index.column() == 0 && role == Qt::DecorationRole)
      return item->getIcon();
    if (index.column() == 0 && role == Qt::DisplayRole)
      return item->getName();

    return QVariant();
  }

  QVariant ImageFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal)
      return QVariant();

    return "Name";
  }

  void ImageFileSystemModel::removeItem(MediaItem::ptr_t item) {
    auto modelIdx = getIndexForItem(item);
    auto modelParent = parent(modelIdx);

    beginRemoveRows(modelParent, modelIdx.row(), modelIdx.row());

    auto parent = fromIndex(modelParent);
    auto ifsitem = fromIndex(modelIdx);
    parent->removeChild(ifsitem);

    endRemoveRows();
  }

  void ImageFileSystemModel::moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    auto modelIdx = getIndexForItem(item);
    auto modelParent = parent(modelIdx);
    auto modelNewParent = getIndexForItem(newParent);

    if (modelParent == modelNewParent)
      return;

    auto ifParent = fromIndex(modelParent);
    auto ifItem = fromIndex(modelIdx);
    auto ifNewParent = fromIndex(modelNewParent);

    auto newRowIdx = 0;
    {
      auto tempList = ifNewParent->getChildren();
      tempList.push_back(ifItem);
      std::sort(tempList.begin(), tempList.end(), IFSModelItem::AscTypeNameSorter);
      auto it = std::find(tempList.begin(), tempList.end(), ifItem);

      newRowIdx = static_cast<int>(it - tempList.begin());
    }

    beginMoveRows(modelParent, modelIdx.row(), modelIdx.row(), modelNewParent, newRowIdx);

    ifNewParent->addChild(ifItem);

    endMoveRows();
  }

  void ImageFileSystemModel::addItem(MediaItem::ptr_t newItem, MediaItem::ptr_t parent) {
    auto modelParent = getIndexForItem(parent);
    auto ifParent = fromIndex(modelParent);

    auto ifItem = std::make_shared<IFSModelItem>(newItem, *iconProvider);

    int idx = 0;
    {
      auto list = ifParent->getChildren();
      list.push_back(ifItem);
      sort_cont(list, IFSModelItem::AscTypeNameSorter);
      idx = static_cast<int>(find_index(list, ifItem));
    }

    beginInsertRows(modelParent, idx, idx);

    ifParent->addChild(ifItem);

    endInsertRows();
  }

  void ImageFileSystemModel::reloadDatabase() {
    beginResetModel();

    root.reset();

    auto dbRoot = database->getRootItem();
    if (dbRoot) {
      root = std::make_shared<IFSModelItem>(dbRoot, *iconProvider);

      syncChildren(root, dbRoot);
    }

    endResetModel();
  }

  IFSModelItem::ptr_t ImageFileSystemModel::fromIndex(const QModelIndex& index) {
    auto ptr = reinterpret_cast<IFSModelItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }

  bool ImageFileSystemModel::isItemOfRoot(IFSModelItem::ptr_t item) const {
    auto check = item;
    do {
      if (check == root)
        return true;

      check = check->getParent();
    } while (check);

    return false;
  }

  IFSModelItem::ptr_t ImageFileSystemModel::getChild(int rowIndex, IFSModelItem::ptr_t parentItem) const {
    assert(rowIndex >= 0);

    return parentItem->getChild(static_cast<size_t>(rowIndex));
  }

  int ImageFileSystemModel::getChildrenCount(IFSModelItem::ptr_t item) const {
    return static_cast<int>(item->getNumberOfChildren());
  }

  int ImageFileSystemModel::getIndexFromParent(IFSModelItem::ptr_t item) const {
    auto parent = item->getParent();
    assert(parent);

    auto index = find_index(parent->getChildren(), item);
    return static_cast<int>(index);
  }

  bool ImageFileSystemModel::itemFulfillsCondition(MediaItem::ptr_t item) const {
    if (filter == MediaItemFilter::NoFilter)
      return true;

    if (filter == MediaItemFilter::OnlyDirectories) {
      return item->getType() == MediaItemType::Directory;
    }

    return false;
  }

  IFSModelItem::ptr_t ImageFileSystemModel::findItemForMediaItem(MediaItem::ptr_t dbItem, IFSModelItem::ptr_t parentItem) const {
    if (!parentItem) {
      if (root->getMediaItem() == dbItem)
        return root;
      return findItemForMediaItem(dbItem, root);
    }

    {
      auto item = find_value_if(parentItem->getChildren(), IFSModelItem::itemNull, [&](IFSModelItem::ptr_t item) {return item->getMediaItem() == dbItem; });
      if (item)
        return item;
    }

    for (auto& item : parentItem->getChildren()) {
      auto found = findItemForMediaItem(dbItem, item);
      if (found)
        return found;
    }

    return IFSModelItem::itemNull;
  }

  void ImageFileSystemModel::syncChildren(IFSModelItem::ptr_t parentItem, MediaItem::ptr_t dbParentItem) {
    if (!dbParentItem->hasChildren())
      return;

    for (auto& dbItem : dbParentItem->getChildren()) {
      if (!itemFulfillsCondition(dbItem))
        continue;

      auto item = std::make_shared<IFSModelItem>(dbItem, *iconProvider);

      parentItem->addChild(item);
      syncChildren(item, dbItem);
    }

    parentItem->sortChildren(IFSModelItem::AscTypeNameSorter);
  }
}