#pragma once

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QSize>

#include "../extensions/ExtPlugin.h"
#include "ThumbnailRequestList.h"
#include "ThumbnailDoneList.h"
#include "ThumbnailThread.h"

namespace realn {

  class ThumbnailWorker {
  public:
    
    ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins, QSize targetThumbnailSize);
    ~ThumbnailWorker();

    void addThumbnailRequest(const QString& filePath);
    void clearRequests();

    bool hasDoneThumbnails() const;
    ThumbnailDoneList::done_vec_t popDoneThumbnails();

    void setTargetThumbnailSize(QSize size);

  private:
    using thread_ptr_t = std::unique_ptr<ThumbnailThread>;
    using thread_vec_t = std::vector<thread_ptr_t>;

    std::shared_ptr<ThumbnailRequestList> requests;
    std::shared_ptr<ThumbnailDoneList> completed;

    thread_vec_t threads;
  };
}