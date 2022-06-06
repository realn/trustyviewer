#pragma once

#include <unordered_map>

#include <QPixmap>
#include <QAbstractItemModel>

#include "ThumbnailModelItem.h"
#include "../MediaItemStorage.h"
#include "../Utils.h"
#include "../MediaItem.h"
#include "../extensions/ExtPlugin.h"
#include "../workers/ThumbnailWorker.h"

namespace realn {
  class MediaDatabase;

  class ThumbnailDragDropView {
  public:
    virtual QModelIndex findDropIndex() const = 0;
  };

  class ThumbnailModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ThumbnailModel(std::shared_ptr<MediaDatabase> _database, std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<ThumbnailWorker> _worker, std::shared_ptr<MediaItemStorage> storage, ThumbnailDragDropView* dragDropView, QSize _thumbnailSize);

    void setRootItem(MediaItem::ptr_t item);
    MediaItem::ptr_t getRootItem() const;

    MediaItem::ptr_t getMediaItemForIndex(QModelIndex index) const;
    QModelIndex getIndexForItem(MediaItem::ptr_t item) const;

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indices) const override;
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

  signals:
    void moveItemRequested(MediaItem::ptr_t item, MediaItem::ptr_t newParent);

  public slots:
    void refreshModel();
    void removeItem(MediaItem::ptr_t item);
    void moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent);

  private slots:
    void retrieveThumbnails();

  private:
    using pixmap_ptr_t = std::unique_ptr<QPixmap>;
    using thumbnail_map_t = std::unordered_map<QString, pixmap_ptr_t>;
    using items_t = std::vector<ThumbnailModelItem::ptr_t>;

    ThumbnailModelItem::ptr_t fromIndex(const QModelIndex& index) const;
    void createThumbnails();
    QPixmap getThumbnail(const QString& filepath) const;
    void emitThumbnailsDataChanged(QStringList items);
    QPixmap createFolderThumbnail();

    std::shared_ptr<MediaDatabase> database;
    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<ThumbnailWorker> worker;
    std::shared_ptr<MediaItemStorage> itemStorage;

    MediaItem::ptr_t rootItem;
    items_t items;

    thumbnail_map_t thumbnails;
    QSize thumbnailSize;
    QPixmap defaultThumbnail;
    QPixmap folderThumbnail;

    ThumbnailDragDropView* view;
  };
}