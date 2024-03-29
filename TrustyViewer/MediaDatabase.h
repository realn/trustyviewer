#pragma once

#include <QObject>

#include "extensions/ExtPlugin.h"
#include "workers/MediaItemWorker.h"
#include "MediaItem.h"

namespace realn {
  class MediaDatabase : public QObject 
  {
    Q_OBJECT;
  public:
    MediaDatabase(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<MediaItemWorker> _worker);

    MediaItem::ptr_t getRootItem() const;

    void rebuid(const QString& newRootPath);

  signals:
    void rebuildingDatabase();
    void databaseRebuild();
    void itemWillBeRemoved(MediaItem::ptr_t item);
    void itemRemoved(MediaItem::ptr_t item);
    void itemWillBeMoved(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    void itemMoved(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    void itemsWillBeMoved(MediaItem::itemvector_t items, MediaItem::ptr_t newParent);
    void itemsMoved(MediaItem::itemvector_t items, MediaItem::ptr_t newParent);
    void itemWillBeAdded(MediaItem::ptr_t newItem, MediaItem::ptr_t parent);
    void itemAdded(MediaItem::ptr_t newItem, MediaItem::ptr_t parent);
    void itemWillBeChanged(MediaItem::ptr_t item);
    void rebuildProgressUpdated(int doneTasks, int totalTasks);

  public slots:
    void moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    void moveItems(MediaItem::itemvector_t items, MediaItem::ptr_t newParent);
    void deleteItem(MediaItem::ptr_t item);
    void makeFolderItem(QString newName, MediaItem::ptr_t parent);

  private slots:
    void checkForData();

  private:
    void asyncWaitForCheck(std::chrono::milliseconds value);
    bool moveItemPriv(MediaItem::ptr_t item, MediaItem::ptr_t newParent);

    bool moveDirectory(MediaItem::ptr_t item, MediaItem::ptr_t newParent);
    bool moveFile(MediaItem::ptr_t item, MediaItem::ptr_t newParent);

    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<MediaItemWorker> worker;
    QString rootPath;
    MediaItem::ptr_t rootItem;
    MediaItemWorker::task_id itemTaskId = 0;
  };
}