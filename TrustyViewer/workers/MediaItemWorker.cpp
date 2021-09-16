
#include <QDir>
#include <QFileInfo>

#include "../Utils.h"

#include "MediaItemWorker.h"

namespace realn {
  MediaItemWorker::MediaItemWorker(std::shared_ptr<ExtPluginList> _plugins) 
    : plugins(_plugins)
  {
    for (auto i = 0; i < std::thread::hardware_concurrency() / 2; i++) {
      threads.emplace_back(std::thread(&MediaItemWorker::run, this));
    }
  }

  MediaItemWorker::~MediaItemWorker()
  {
    runThreads = false;
    tasks.wakeAll();
    completed.wakeAll();
    for (auto& thread : threads)
      thread.join();
  }

  MediaItemWorker::task_id MediaItemWorker::addItemScanTask(const QString& itemPath)
  {
    return addTask(itemPath);
  }

  bool MediaItemWorker::isTaskCompleted(task_id taskId) const
  {
    return completed.contains_if([&](const task_ptr_t& task) { return task->id == taskId; });
  }

  MediaItem::ptr_t MediaItemWorker::popCompletedItem(task_id taskId)
  {
    auto item = completed.pop_if([&](task_ptr_t& task) {return task->id == taskId; });
    assert(item);
    return item->item;
  }

  void MediaItemWorker::run()
  {
    while (runThreads) {
      auto task = tasks.pop_front();
      if (!task)
        continue;

      runTask(task);

      if (task->state == task_t::TaskState::Completed)
        completed.push_back(task);
      else
        tasks.push_back(task);
    }
  }

  void MediaItemWorker::runTask(task_ptr_t task)
  {
    if (task->state == task_t::TaskState::Waiting) {
      auto doneTasks = popDoneTasks(task->waitingTaskIds);
      for (auto& doneTask : doneTasks) {
        if (!doneTask->item)
          continue;
        task->item->addChild(doneTask->item);
      }
      task->update();
      return;
    }
    else if (task->state == task_t::TaskState::Completed)
      return;

    task->item = buildFromPath(task->itemPath, task->waitingTaskIds);
    task->update();
  }

  MediaItemWorker::task_id MediaItemWorker::addTask(const QString& filePath)
  {
    auto resultId = lastId++;

    auto task = std::make_shared<task_t>();
    task->id = resultId;
    task->itemPath = filePath;
    tasks.push_back(task);

    return resultId;
  }

  std::vector<MediaItemWorker::task_ptr_t> MediaItemWorker::popDoneTasks(task_id_vec_t& taskIds)
  {
    auto result = std::vector<task_ptr_t>();
    auto it = taskIds.begin();
    while (it != taskIds.end())
    {
      auto taskId = *it;
      auto task = completed.pop_if([&](task_ptr_t& item) { return item->id == taskId; });
      if (!task) {
        it++;
        continue;
      }

      result.push_back(task);
      it = taskIds.erase(it);
    }
    return result;
  }

  MediaItem::ptr_t MediaItemWorker::buildFromPath(const QString& path, task_id_vec_t& waitingTasks)
  {
    auto info = QFileInfo(path);
    if (!info.exists())
      return nullptr;

    if (info.isDir())
      return buildDir(path, waitingTasks);

    auto plugin = plugins->getPluginForExt(info.completeSuffix());
    if (!plugin)
      return nullptr;

    if (plugin->isVideo(info.completeSuffix()))
      return buildMediaItem(path, MediaItemType::Video);

    if (plugin->isAnimated(info.completeSuffix()))
      return buildMediaItem(path, MediaItemType::Animation);

    return buildMediaItem(path, MediaItemType::Image);
  }

  MediaItem::ptr_t MediaItemWorker::buildMediaItem(const QString& path, MediaItemType type)
  {
    auto result = std::make_shared<MediaItem>(path, type);
    return result;
  }

  MediaItem::ptr_t MediaItemWorker::buildDir(const QString& path, task_id_vec_t& waitingTasks)
  {
    auto result = std::make_shared<MediaItem>(path, MediaItemType::Directory);

    auto dir = QDir(path, "*", QDir::Type | QDir::IgnoreCase);

    for (auto& item : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      auto newPath = dir.filePath(item);
      auto taskId = addTask(newPath);
      waitingTasks.push_back(taskId);
    }

    for (auto& item : dir.entryList(getNameFilters(), QDir::Files | QDir::NoDotAndDotDot)) {
      auto newPath = dir.filePath(item);
      auto taskId = addTask(newPath);
      waitingTasks.push_back(taskId);
    }

    return result;
  }

  QStringList MediaItemWorker::getNameFilters() const
  {
    auto result = QStringList();
    for (auto& ext : plugins->getSupportedExts()) {
      result << ("*." + ext);
    }
    return result;
  }
}