#pragma once

#include <unordered_map>

#include <QPixmap>
#include <QAbstractItemModel>

#include "ThumbnailModelItem.h"
#include "../Utils.h"
#include "../MediaItem.h"
#include "../extensions/ExtPlugin.h"
#include "../workers/ThumbnailWorker.h"

namespace realn {
  class ThumbnailModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ThumbnailModel(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<ThumbnailWorker> _worker, QSize _thumbnailSize);

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

    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<ThumbnailWorker> worker;

    MediaItem::ptr_t rootItem;
    items_t items;

    thumbnail_map_t thumbnails;
    QSize thumbnailSize;
    QPixmap defaultThumbnail;

    pair_call_guard guardRemove;
  };
}