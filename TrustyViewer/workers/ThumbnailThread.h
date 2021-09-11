#pragma once

#include <thread>
#include <atomic>

#include "../extensions/ExtPlugin.h"
#include "ThumbnailRequestList.h"
#include "ThumbnailDoneList.h"

namespace realn {
  class ThumbnailThread {
  public:
    ThumbnailThread(
      std::shared_ptr<ExtPluginList> _plugins,
      std::shared_ptr<ThumbnailRequestList> _requests,
      std::shared_ptr<ThumbnailDoneList> _completed
    );
    ~ThumbnailThread();

    void stop();

  private:
    void run();

    std::shared_ptr<ThumbnailRequestList> requests;
    std::shared_ptr<ThumbnailDoneList> completed;
    std::shared_ptr<ExtPluginList> plugins;
    std::thread thread;
    std::atomic_bool canRun = true;
  };
}