#pragma once

#include <QObject>

#include "extensions/ExtPlugin.h"
#include "MediaItem.h"

namespace realn {
  class MediaDatabase : public QObject 
  {
    Q_OBJECT;
  public:
    MediaDatabase(std::shared_ptr<ExtPluginList> plugins);

    MediaItem::ptr_t getRootItem() const;

    void rebuid(const QString& newRootPath);

  signals:
    void rebuildingDatabase();
    void databaseRebuild();

  private:
    MediaItem::ptr_t buildFromPath(const QString& path);
    MediaItem::ptr_t buildMediaItem(const QString& path, MediaItemType type);
    MediaItem::ptr_t buildDir(const QString& path);

    QStringList getNameFilters() const;

    std::shared_ptr<ExtPluginList> plugins;
    QString rootPath;
    MediaItem::ptr_t rootItem;
    MediaItem::itemvector_t mainList;
  };
}