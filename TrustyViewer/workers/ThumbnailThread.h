#pragma once

#include <thread>
#include <atomic>

#include "../extensions/ExtPlugin.h"
#include "ThumbnailRequestList.h"
#include "ThumbnailDoneList.h"

namespace realn {
  class ThumbnailThread {
  public:
    using pixmap_ptr_t = std::unique_ptr<QPixmap>;

    ThumbnailThread(
      std::shared_ptr<ExtPluginList> _plugins,
      std::shared_ptr<ThumbnailRequestList> _requests,
      std::shared_ptr<ThumbnailDoneList> _completed,
      QSize targetThumbnailSize
    );
    ~ThumbnailThread();

    void stop();

    void setTargetThumbnailSize(QSize size);

  private:
    void run();

    ThumbnailThread::pixmap_ptr_t createCorrectThumbnail(const QPixmap& pixmap) const;

    std::shared_ptr<ThumbnailRequestList> requests;
    std::shared_ptr<ThumbnailDoneList> completed;
    std::shared_ptr<ExtPluginList> plugins;
    std::thread thread;
    std::atomic_bool canRun = true;
    QSize targetThumbnailSize = QSize(30, 30);
  };
}