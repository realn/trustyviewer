
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

#include "MediaItem.h"
#include "MediaDatabase.h"

using namespace std::chrono_literals;

namespace realn {
  MediaDatabase::MediaDatabase(std::shared_ptr<ExtPluginList> _plugins, std::shared_ptr<MediaItemWorker> _worker)
    : plugins(_plugins)
    , worker(_worker) {
  }

  MediaItem::ptr_t MediaDatabase::getRootItem() const {
    return rootItem;
  }

  void MediaDatabase::rebuid(const QString& newRootPath) {
    emit rebuildingDatabase();

    itemTaskId = worker->addItemScanTask(newRootPath);

    asyncWaitForCheck(100ms);
  }

  void MediaDatabase::checkForData() {
    auto [done, total] = worker->getStats();
    emit rebuildProgressUpdated(done, total);

    if (!worker->isTaskCompleted(itemTaskId)) {
      asyncWaitForCheck(100ms);
      return;
    }

    rootItem = worker->popCompletedItem(itemTaskId);
    rootItem->sortChildren(MediaItem::AscTypeNameSorter);

    rootPath = rootItem->getFilePath();
    emit databaseRebuild();
  }

  void MediaDatabase::moveItem(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    if (!moveItemPriv(item, newParent))
      return;

    emit itemWillBeMoved(item, newParent);

    newParent->addChild(item);
    newParent->sortChildren(MediaItem::AscTypeNameSorter);

    emit itemMoved(item, newParent);
  }

  void MediaDatabase::moveItems(MediaItem::itemvector_t items, MediaItem::ptr_t newParent) {
    auto result = MediaItem::itemvector_t();
    for (auto& item : items) {
      if (moveItemPriv(item, newParent))
        result.push_back(item);
    }

    emit itemsWillBeMoved(result, newParent);

    for (auto& item : result) {

      emit itemWillBeMoved(item, newParent);

      newParent->addChild(item);

      emit itemMoved(item, newParent);
    }
    newParent->sortChildren(MediaItem::AscTypeNameSorter);

    emit itemsMoved(result, newParent);
  }

  void MediaDatabase::deleteItem(MediaItem::ptr_t item) {
    emit itemWillBeChanged(item);
    if (item->isDirectory()) {
      for (auto& child : item->getChildren()) {
        deleteItem(child);
      }
      if (item->hasChildren()) {
        return;
      }
      auto dirPath = item->getFilePath();
      QDir dir(dirPath);
      if (dir.removeRecursively()) {
        emit itemWillBeRemoved(item);

        item->makeOrphan();

        emit itemRemoved(item);
      }
    }
    else {
      auto filePath = item->getFilePath();
      if (QFile::remove(filePath)) {

        emit itemWillBeRemoved(item);

        item->makeOrphan();

        emit itemRemoved(item);
      }
    }
  }

  void MediaDatabase::makeFolderItem(QString newName, MediaItem::ptr_t parent) {
    assert(!newName.isEmpty());
    assert(parent && parent->isDirectory());

    auto dirPath = parent->getFilePath();
    QDir dir(dirPath);
    if (dir.mkdir(newName) && dir.cd(newName)) {
      auto item = std::make_shared<MediaItem>(dir.path(), MediaItemType::Directory);

      emit itemWillBeAdded(item, parent);
      parent->addChild(item);
      parent->sortChildren(MediaItem::AscTypeNameSorter);
      emit itemAdded(item, parent);
    }
  }

  void MediaDatabase::asyncWaitForCheck(std::chrono::milliseconds value) {
    QTimer::singleShot(value, this, &MediaDatabase::checkForData);

  }
  bool MediaDatabase::moveItemPriv(MediaItem::ptr_t item, MediaItem::ptr_t newParent) {
    if (item->getParent() == newParent)
      return false;
    if (newParent->hasInPathToRoot(item))
      return false;

    emit itemWillBeChanged(item);
    if (item->isDirectory()) {
      auto oldDir = QDir(item->getFilePath());
      auto newDir = QDir(newParent->getFilePath());

      auto newPath = newDir.filePath(oldDir.dirName());

      if (oldDir.rename(item->getFilePath(), newPath)) {
        item->replaceFilePath(newPath);
      }
      else
        return false;
    }
    else {
      auto oldFile = QFileInfo(item->getFilePath());
      auto newDir = QDir(newParent->getFilePath());

      auto newFilePath = newDir.filePath(oldFile.fileName());

      if (QFile::rename(item->getFilePath(), newFilePath)) {
        item->replaceFilePath(newFilePath);
      }
      else
        return false;
    }

    return true;
  }
}