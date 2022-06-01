#pragma once

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QDir>
#include <QFileIconProvider>

#include "../MediaDatabase.h"

namespace realn {
  enum class MediaItemFilter {
    NoFilter = 0,
    OnlyDirectories
  };

  class ImageFileSystemModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase, MediaItemFilter filter = MediaItemFilter::NoFilter);

    MediaItem::ptr_t getItemForIndex(const QModelIndex& index) const;
    MediaItem::itemvector_t getItemsForIndices(const QModelIndexList& indices) const;
    QModelIndex getIndexForItem(MediaItem::ptr_t item) const;

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  public slots:
    void reloadDatabase();
    void beginRemoveItem(MediaItem::ptr_t item);
    void endRemoveItem();
    void beginMoveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    void endMoveItem();
    void beginAddItem(MediaItem::ptr_t newItem, MediaItem::ptr_t parent);
    void endAddItem();

  private:
    static MediaItem::ptr_t fromIndex(const QModelIndex& index);
    bool isItemOfRoot(MediaItem::ptr_t item) const;
    int getChildrenCount(MediaItem::ptr_t item) const;
    MediaItem::ptr_t getChild(int rowIndex, MediaItem::ptr_t parentItem) const;
    int getIndexFromParent(MediaItem::ptr_t item) const;
    bool itemFulfillsCondition(MediaItem::ptr_t item) const;

    std::shared_ptr<MediaDatabase> database;
    std::unique_ptr<QFileIconProvider> iconProvider;
    MediaItemFilter filter;
  };
}