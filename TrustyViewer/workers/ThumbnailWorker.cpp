
#include <QFileInfo>

#include "ThumbnailWorker.h"

namespace realn {
  ThumbnailWorker::ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins)
    : plugins(_plugins)
  {
    requests = std::make_shared<ThumbnailRequestList>();
    completed = std::make_shared<ThumbnailDoneList>();

    jobThread = std::thread(&ThumbnailWorker::Run, this);
  }

  ThumbnailWorker::~ThumbnailWorker()
  {
    canRun = false;
    requests->wakeAll();
    jobThread.join();
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

  void ThumbnailWorker::Run()
  {
    while (canRun) {
      if (requests->isEmpty()) {
        requests->waitForRequests();
        continue;
      }
      QString request = requests->popRequest();
      if (request.isEmpty())
        continue;

      QFileInfo info(request);
      auto plugin = plugins->getPluginForExt(info.completeSuffix());
      if (!plugin)
        continue;

      auto thumbnail = plugin->createThumbnail(request, QSize(100, 150));
      if (!thumbnail)
        continue;

      completed->addDone(request, std::move(thumbnail));
    }
  }


}