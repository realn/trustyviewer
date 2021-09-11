#pragma once

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>

#include <QString>
#include <QStringList>
#include <QPixmap>

#include "../extensions/ExtPlugin.h"
#include "ThumbnailRequestList.h"

namespace realn {

  class ThumbnailWorker {
  public:
    class JobDone {
    public:
      QString filePath;
      std::unique_ptr<QPixmap> thumbnail;
    };
    using done_vec_t = std::vector<JobDone>;
    
    ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins);
    ~ThumbnailWorker();

    void addThumbnailRequest(const QString& filePath);
    void clearRequests();

    bool hasDoneThumbnails() const;
    done_vec_t popDoneThumbnails();

  private:
    void addCompleted(QString filePath, std::unique_ptr<QPixmap> thumbnail);
    void Run();

    std::shared_ptr<ThumbnailRequestList> requests;
    std::shared_ptr<ExtPluginList> plugins;
    std::vector<JobDone> completedJobs;
    std::thread jobThread;
    std::mutex doneMutex;
    std::atomic_bool hasDone = false;
    std::atomic_bool canRun = true;
  };
}