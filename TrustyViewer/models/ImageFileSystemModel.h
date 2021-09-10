#pragma once

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QDir>
#include <QFileIconProvider>

#include "../MediaDatabase.h"

namespace realn {
  class ImageFileSystemModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ImageFileSystemModel(std::shared_ptr<MediaDatabase> mediaDatabase);

    MediaItem::ptr_t getItemForIndex(const QModelIndex& index) const;

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  public slots:
    void reloadDatabase();

  private:
    static MediaItem::ptr_t fromIndex(const QModelIndex& index);

    std::shared_ptr<MediaDatabase> database;
    std::unique_ptr<QFileIconProvider> iconProvider;
  };
}