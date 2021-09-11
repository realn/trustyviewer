
#include <QFileInfo>

#include "ThumbnailWorker.h"

namespace realn {
  ThumbnailWorker::ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins)
    : plugins(_plugins)
  {
    requests = std::make_shared<ThumbnailRequestList>();

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
    return hasDone;
  }

  ThumbnailWorker::done_vec_t ThumbnailWorker::popDoneThumbnails()
  {
    auto lock = std::unique_lock<std::mutex>(doneMutex);
    auto result = done_vec_t();
    std::swap(completedJobs, result);
    hasDone = false;
    return result;
  }


  void ThumbnailWorker::addCompleted(QString filePath, std::unique_ptr<QPixmap> thumbnail)
  {
    auto lock = std::unique_lock<std::mutex>(doneMutex);
    completedJobs.push_back({filePath, std::move(thumbnail)});
    hasDone = true;
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

      addCompleted(request, std::move(thumbnail));
    }
  }


}