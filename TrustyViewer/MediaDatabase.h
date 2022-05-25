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

  private slots:
    void checkForData();

  private:
    void asyncWaitForCheck(std::chrono::milliseconds value);

    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<MediaItemWorker> worker;
    QString rootPath;
    MediaItem::ptr_t rootItem;
    //MediaItem::itemvector_t mainList;
    MediaItemWorker::task_id itemTaskId = 0;
  };
}