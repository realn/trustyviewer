
#include "ImageFileSystemModel.h"

namespace realn {
  ImageFileSystemModel::ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase, MediaItemFilter itemFilter)
    : database(mediaDatabase), filter(itemFilter) {
    iconProvider = std::make_unique<QFileIconProvider>();
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

    return createIndex(row, column, getChild(row, parentItem).get());
  }

  QModelIndex ImageFileSystemModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
      return QModelIndex();

    auto item = fromIndex(child);
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

  void ImageFileSystemModel::beginRemoveItem(MediaItem::ptr_t item) {
    auto modelIdx = getIndexForItem(item);
    auto modelParent = parent(modelIdx);

    beginRemoveRows(modelParent, modelIdx.row(), modelIdx.row());
  }

  void ImageFileSystemModel::endRemoveItem() {
    endRemoveRows();
  }

  void ImageFileSystemModel::beginMoveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    auto modelIdx = getIndexForItem(item);
    auto modelParent = parent(modelIdx);
    auto modelNewParent = getIndexForItem(newParent);

    auto newRowIdx = 0;
    {
      auto tempList = newParent->getChildren();
      tempList.push_back(item);
      std::sort(tempList.begin(), tempList.end(), MediaItem::AscTypeNameSorter);
      auto it = std::find(tempList.begin(), tempList.end(), item);

      newRowIdx = static_cast<int>(it - tempList.begin());
    }

    if (modelParent == modelNewParent)
      return;

    beginMoveRows(modelParent, modelIdx.row(), modelIdx.row(), modelNewParent, newRowIdx + 1);
    
  }

  void ImageFileSystemModel::endMoveItem() {
    endMoveRows();
  }

  void ImageFileSystemModel::beginAddItem(MediaItem::ptr_t newItem, MediaItem::ptr_t parent) {
    auto list = parent->getChildren();
    list.push_back(newItem);
    sort_cont(list, MediaItem::AscTypeNameSorter);
    auto idx = find_index(list, newItem);

    auto modelParent = getIndexForItem(parent);

    beginInsertRows(modelParent, idx, idx);
  }

  void ImageFileSystemModel::endAddItem() {
    endInsertRows();
  }

  void ImageFileSystemModel::reloadDatabase() {
    beginResetModel();

    root.reset();

    auto dbRoot = database->getRootItem();
    root = std::make_shared<IFSModelItem>(dbRoot, *iconProvider);

    syncChildren(root, dbRoot);

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

    auto list = parentItem->getChildren();
    for (auto& item : list) {
      if (!itemFulfillsCondition(item))
        continue;

      if (rowIndex > 0) {
        rowIndex--;
        continue;
      }

      return item;
    }

    throw std::runtime_error("Failed to find child for row index.");
  }

  int ImageFileSystemModel::getChildrenCount(IFSModelItem::ptr_t item) const {
    auto list = item->getChildren();

    auto result = std::count_if(list.begin(), list.end(), [this](auto& listItem) {
      return itemFulfillsCondition(listItem);
                                });

    return static_cast<int>(result);
  }

  int ImageFileSystemModel::getIndexFromParent(IFSModelItem::ptr_t item) const {
    auto parent = item->getParent();
    assert(parent);

    int index = 0;
    for (auto& listItem : parent->getChildren()) {
      if (!itemFulfillsCondition(listItem))
        continue;
      if (item != listItem) {
        index++;
        continue;
      }

      return index;
    }

    throw std::runtime_error("Failed to find item index of parent.");
  }

  bool ImageFileSystemModel::itemFulfillsCondition(IFSModelItem::ptr_t item) const {
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
      auto item = std::make_shared<IFSModelItem>(dbItem, *iconProvider);

      parentItem->addChild(item);
      syncChildren(item, dbItem);
    }

    parentItem->sortChildren(IFSModelItem::AscTypeNameSorter);
  }
}