
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
    return item;

  }

  MediaItem::itemvector_t ImageFileSystemModel::getItemsForIndices(const QModelIndexList& indices) const {
    auto result = MediaItem::itemvector_t();
    for (auto& index : indices) {
      auto item = fromIndex(index);
      if (!item)
        continue;
      result.push_back(item);
    }
    return result;
  }

  QModelIndex ImageFileSystemModel::getIndexForItem(MediaItem::ptr_t item) const {
    if (!isItemOfRoot(item))
      return QModelIndex();

    if (item == database->getRootItem())
      return index(0, 0);

    int row = getIndexFromParent(item);
    return index(row, 0, getIndexForItem(item->getParent()));
  }

  QModelIndex ImageFileSystemModel::index(int row, int column, const QModelIndex& parent) const {
    if (column >= columnCount())
      return QModelIndex();

    if (!parent.isValid()) {
      return createIndex(row, column, database->getRootItem().get());
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

    QFileInfo info(item->getFilePath());

    if (index.column() == 0 && role == Qt::DecorationRole)
      return iconProvider->icon(info);
    if (index.column() == 0 && role == Qt::DisplayRole)
      return info.fileName();

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

  void ImageFileSystemModel::reloadDatabase() {
    beginResetModel();

    endResetModel();
  }

  MediaItem::ptr_t ImageFileSystemModel::fromIndex(const QModelIndex& index) {
    auto ptr = reinterpret_cast<MediaItem*>(index.internalPointer());
    if (!ptr)
      return nullptr;
    return ptr->getPtr();
  }

  bool ImageFileSystemModel::isItemOfRoot(MediaItem::ptr_t item) const {
    auto check = item;
    do {
      if (check == database->getRootItem())
        return true;

      check = check->getParent();
    } while (check);

    return false;
  }

  MediaItem::ptr_t ImageFileSystemModel::getChild(int rowIndex, MediaItem::ptr_t parentItem) const {
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

  int ImageFileSystemModel::getChildrenCount(MediaItem::ptr_t item) const {
    auto list = item->getChildren();

    auto result = std::count_if(list.begin(), list.end(), [this](MediaItem::ptr_t listItem) {
      return itemFulfillsCondition(listItem);
                                });

    return static_cast<int>(result);
  }

  int ImageFileSystemModel::getIndexFromParent(MediaItem::ptr_t item) const {
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

  bool ImageFileSystemModel::itemFulfillsCondition(MediaItem::ptr_t item) const {
    if (filter == MediaItemFilter::NoFilter)
      return true;

    if (filter == MediaItemFilter::OnlyDirectories) {
      return item->getType() == MediaItemType::Directory;
    }

    return false;
  }
}