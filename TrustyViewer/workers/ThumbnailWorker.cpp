
#include <QFileInfo>

#include "ThumbnailWorker.h"

namespace realn {
  ThumbnailWorker::ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins)
  {
    requests = std::make_shared<ThumbnailRequestList>();
    completed = std::make_shared<ThumbnailDoneList>();

    thread = std::make_unique<ThumbnailThread>(_plugins, requests, completed);
  }

  ThumbnailWorker::~ThumbnailWorker()
  {
    thread->stop();
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


}