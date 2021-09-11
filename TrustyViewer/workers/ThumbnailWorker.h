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
#include "ThumbnailDoneList.h"

namespace realn {

  class ThumbnailWorker {
  public:
    
    ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins);
    ~ThumbnailWorker();

    void addThumbnailRequest(const QString& filePath);
    void clearRequests();

    bool hasDoneThumbnails() const;
    ThumbnailDoneList::done_vec_t popDoneThumbnails();

  private:
    void Run();

    std::shared_ptr<ThumbnailRequestList> requests;
    std::shared_ptr<ThumbnailDoneList> completed;
    std::shared_ptr<ExtPluginList> plugins;
    std::thread jobThread;
    std::atomic_bool canRun = true;
  };
}