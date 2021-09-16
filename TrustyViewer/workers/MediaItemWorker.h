#pragma once

#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <mutex>

#include <QString>
#include <QStringList>

#include "../Utils.h"
#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"
#include "MediaItemWorkerTaskList.h"

namespace realn{
  class MediaItemWorker {
  public:
    using task_t = MediaItemWorkerTask;
    using task_id = task_t::task_id;

    MediaItemWorker(std::shared_ptr<ExtPluginList> _plugins);
    ~MediaItemWorker();

    task_id addItemScanTask(const QString& itemPath);

    bool isTaskCompleted(task_id taskId) const;

    MediaItem::ptr_t popCompletedItem(task_id taskId);

  private:
    using task_ptr_t = task_t::ptr_t;
    using task_id_vec_t = task_t::task_id_vec_t;
    using task_list_t = MediaItemWorkerTaskList;
    using thread_vec_t = std::vector<std::thread>;

    void run();

    void runTask(task_ptr_t task);
    task_id addTask(const QString& filePath);
    std::vector<task_t::ptr_t> popDoneTasks(task_id_vec_t& taskIds);

    MediaItem::ptr_t buildFromPath(const QString& path, task_id_vec_t& waitingTasks);
    MediaItem::ptr_t buildMediaItem(const QString& path, MediaItemType type);
    MediaItem::ptr_t buildDir(const QString& path, task_id_vec_t& waitingTasks);

    QStringList getNameFilters() const;

    std::shared_ptr<ExtPluginList> plugins;
    task_list_t tasks;
    task_list_t completed;
    task_id lastId = 0;
    thread_vec_t threads;
    std::atomic_bool runThreads = true;
  };
}