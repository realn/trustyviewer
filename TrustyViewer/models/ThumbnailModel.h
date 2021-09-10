#pragma once

#include <unordered_map>

#include <QPixmap>
#include <QAbstractItemModel>

#include "../MediaItem.h"
#include "../extensions/ExtPlugin.h"

namespace realn {
  class ThumbnailModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ThumbnailModel(std::shared_ptr<ExtPluginList> _plugins, QSize _thumbnailSize);

    void setRootItem(MediaItem::ptr_t item);

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  private:
    using pixmap_ptr_t = std::unique_ptr<QPixmap>;
    using thumbnail_map_t = std::unordered_map<QString, pixmap_ptr_t>;

    void createThumbnails();
    QPixmap getThumbnail(const QString& filepath) const;

    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<MediaItem> rootItem;
    thumbnail_map_t thumbnails;
    QSize thumbnailSize;
    QPixmap defaultThumbnail;
  };
}