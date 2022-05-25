
#include <QFileInfo>
#include <QDir>
#include <QTimer>

#include "MediaDatabase.h"

using namespace std::chrono_literals;

namespace realn {
  MediaDatabase::MediaDatabase(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<MediaItemWorker> _worker)
    : plugins(_plugins)
    , worker(_worker)
  {
  }

  MediaItem::ptr_t MediaDatabase::getRootItem() const
  {
    return rootItem;
  }

  void MediaDatabase::rebuid(const QString& newRootPath)
  {
    emit rebuildingDatabase();

    itemTaskId = worker->addItemScanTask(newRootPath);

    asyncWaitForCheck(500ms);
  }

  void MediaDatabase::checkForData() {
    if (worker->isTaskCompleted(itemTaskId)) {
      rootItem = worker->popCompletedItem(itemTaskId);
      rootItem->sortChildren([](MediaItem::ptr_t& itemA, MediaItem::ptr_t& itemB) {
        if (itemA->getType() == itemB->getType())
          return QString::localeAwareCompare(itemA->getFilePath(), itemB->getFilePath()) < 0;
        return itemA->getType() < itemB->getType();
        });

      rootPath = rootItem->getFilePath();
      emit databaseRebuild();
    }
    else {
      asyncWaitForCheck(500ms);
    }
  }

  void MediaDatabase::asyncWaitForCheck(std::chrono::milliseconds value) {
    QTimer::singleShot(value, this, &MediaDatabase::checkForData);
  }
}