#pragma once

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QDir>
#include <QFileIconProvider>

namespace realn {
  class ImageFileSystemModel : public QAbstractItemModel {
    Q_OBJECT;
  public:
    ImageFileSystemModel();

    void setRootPath(QString path);
    void setSupportedExtensions(const QStringList& exts);

    QFileInfo getFileInfoForIndex(const QModelIndex& index);

    // Inherited via QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  private:
    class Item;
    using ItemPtr = std::shared_ptr<Item>;
    using ItemWeakPtr = std::weak_ptr<Item>;
    using ItemVector = std::vector<ItemPtr>;

    class Item : public std::enable_shared_from_this<Item> {
    public:
      enum class Type {
        Directory = 0,
        SymLink = 1,
        Image = 2,
        Video = 3,
      };

      ItemVector children;
      ItemWeakPtr parent;
      QFileInfo info;
      Type type = Type::Image;

      ItemPtr ptr() { return shared_from_this(); }
    };

    static ItemPtr fromIndex(const QModelIndex& index);

    ItemPtr scanDir(QString path, ItemPtr parent);
    QStringList getNameFilters() const;

    ItemPtr rootItem;
    QDir rootDir;
    std::unique_ptr<QFileIconProvider> iconProvider;
    QStringList supportedExts;
  };
}