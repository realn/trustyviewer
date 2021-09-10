#pragma once

#include <QAbstractItemModel>

#include "../MediaItem.h"

namespace realn {
  class ThumbnailModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ThumbnailModel();

    void setRootItem(MediaItem::ptr_t item);

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  private:
    std::shared_ptr<MediaItem> rootItem;

  };
}