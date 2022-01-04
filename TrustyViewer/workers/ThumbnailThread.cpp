
#include <QFileInfo>

#include "ThumbnailThread.h"

namespace realn {
  ThumbnailThread::ThumbnailThread(
    std::shared_ptr<ExtPluginList> _plugins,
    std::shared_ptr<ThumbnailRequestList> _requests,
    std::shared_ptr<ThumbnailDoneList> _completed)
    : plugins(_plugins)
    , requests(_requests)
    , completed(_completed)
  {
    thread = std::thread(&ThumbnailThread::run, this);
  }

  ThumbnailThread::~ThumbnailThread()
  {
    thread.join();
  }

  void ThumbnailThread::stop()
  {
    canRun = false;
  }

  void ThumbnailThread::run()
  {
    while (canRun) {
      QString request = requests->popRequest();
      if (request.isEmpty())
        continue;

      QFileInfo info(request);
      auto plugin = plugins->getPluginForExt(info.completeSuffix());
      if (!plugin)
        continue;

      auto thumbnailResult = plugin->createThumbnail(request, QSize(100, 150));
      if (thumbnailResult.error == ExtPlugin::Errors::PluginBusy) {
        requests->addRequest(request);
        continue;
      }
      else if (thumbnailResult.error != ExtPlugin::Errors::NoError)
        continue;

      completed->addDone(request, std::move(thumbnailResult.thumbnail));
    }
  }
}