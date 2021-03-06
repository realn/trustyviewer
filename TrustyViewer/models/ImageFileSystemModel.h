#pragma once

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QDir>
#include <QFileIconProvider>

#include "IFSModelItem.h"
#include "../MediaDatabase.h"

namespace realn {
  class MediaItemStorage;
  class DragDropView;

  enum class MediaItemFilter {
    NoFilter = 0,
    OnlyDirectories
  };

  class ImageFileSystemModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<MediaItemStorage> storage, DragDropView* view = nullptr, MediaItemFilter filter = MediaItemFilter::NoFilter);

    MediaItem::ptr_t getItemForIndex(const QModelIndex& index) const;
    MediaItem::itemvector_t getItemsForIndices(const QModelIndexList& indices) const;
    QModelIndex getIndexForItem(MediaItem::ptr_t item) const;
    bool isDirectory(const QModelIndex& index) const;

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indices) const override;
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

  public slots:
    void reloadDatabase();
    void removeItem(MediaItem::ptr_t item);
    void moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    void addItem(MediaItem::ptr_t newItem, MediaItem::ptr_t parent);

  private:
    static IFSModelItem::ptr_t fromIndex(const QModelIndex& index);
    bool isItemOfRoot(IFSModelItem::ptr_t item) const;
    int getChildrenCount(IFSModelItem::ptr_t item) const;
    IFSModelItem::ptr_t getChild(int rowIndex, IFSModelItem::ptr_t parentItem) const;
    int getIndexFromParent(IFSModelItem::ptr_t item) const;
    bool itemFulfillsCondition(MediaItem::ptr_t item) const;

    IFSModelItem::ptr_t findItemForMediaItem(MediaItem::ptr_t dbItem, IFSModelItem::ptr_t parentItem = nullptr) const;
    void syncChildren(IFSModelItem::ptr_t parentItem, MediaItem::ptr_t dbParentItem);

    IFSModelItem::ptr_t root;

    std::shared_ptr<MediaDatabase> database;
    std::shared_ptr<MediaItemStorage> itemStorage;
    std::unique_ptr<QFileIconProvider> iconProvider;
    DragDropView* view;
    MediaItemFilter filter;
  };
}