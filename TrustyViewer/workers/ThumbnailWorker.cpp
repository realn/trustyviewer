
#include <QFileInfo>

#include "ThumbnailWorker.h"

namespace realn {
  ThumbnailWorker::ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins, QSize targetThumbnailSize)
  {
    requests = std::make_shared<ThumbnailRequestList>();
    completed = std::make_shared<ThumbnailDoneList>();

    for (auto i = 0; i < std::thread::hardware_concurrency() / 2; i++) {
      threads.push_back(std::make_unique<ThumbnailThread>(_plugins, requests, completed, targetThumbnailSize));
    }
  }

  ThumbnailWorker::~ThumbnailWorker()
  {
    for(auto& thread : threads) {
      thread->stop();
    }
    requests->wakeAll();
  }

  void ThumbnailWorker::addThumbnailRequest(const QString& filePath)
  {
    requests->addRequest(filePath);
  }

  void ThumbnailWorker::clearRequests()
  {
    requests->clearRequests();
  }

  bool ThumbnailWorker::hasDoneThumbnails() const
  {
    return completed->hasDone();
  }

  ThumbnailDoneList::done_vec_t ThumbnailWorker::popDoneThumbnails()
  {
    return completed->popAllDone();
  }

  void ThumbnailWorker::setTargetThumbnailSize(QSize size) {
    for (auto& thread : threads) {
      thread->setTargetThumbnailSize(size);
    }
  }


}