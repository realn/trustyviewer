
#include <QFileInfo>

#include "ThumbnailWorker.h"

namespace realn {
  ThumbnailWorker::ThumbnailWorker(std::shared_ptr<ExtPluginList> _plugins)
    : plugins(_plugins)
  {
    jobThread = std::thread(&ThumbnailWorker::Run, this);
  }

  ThumbnailWorker::~ThumbnailWorker()
  {
    canRun = false;
    jobCondition.notify_one();
    jobThread.join();
  }

  void ThumbnailWorker::addThumbnailRequest(const QString& filePath)
  {
    auto lock = std::unique_lock<std::mutex>(jobMutex);
    requests.push_back(filePath);
    jobCondition.notify_one();
  }

  void ThumbnailWorker::clearRequests()
  {
    auto lock = std::unique_lock<std::mutex>(jobMutex);
    requests.clear();
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

  QString ThumbnailWorker::popRequest()
  {
    auto filepath = requests.first();
    requests.pop_front();
    return filepath;
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
      QString request;
      {
        auto lock = std::unique_lock<std::mutex>(jobMutex);
        if (requests.empty()) {
          jobCondition.wait(lock);
          continue;
        }
        request = popRequest();
      }

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