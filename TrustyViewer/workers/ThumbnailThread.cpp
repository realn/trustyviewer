
#include <QFileInfo>
#include <QPainter>

#include "ThumbnailThread.h"

namespace realn {
  namespace {
    QPoint toPoint(const QSize& size) {
      return QPoint(size.width(), size.height());
    }
  }

  ThumbnailThread::ThumbnailThread(
    std::shared_ptr<ExtPluginList> _plugins,
    std::shared_ptr<ThumbnailRequestList> _requests,
    std::shared_ptr<ThumbnailDoneList> _completed,
    QSize targetThumbnailSize)
    : plugins(_plugins)
    , requests(_requests)
    , completed(_completed)
    , targetThumbnailSize(targetThumbnailSize)
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

  void ThumbnailThread::setTargetThumbnailSize(QSize size) {
    targetThumbnailSize = size;
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

      completed->addDone(request, createCorrectThumbnail(*thumbnailResult.thumbnail));
    }
  }

  ThumbnailThread::pixmap_ptr_t ThumbnailThread::createCorrectThumbnail(const QPixmap& pixmap) const {
    auto thumbnail = QPixmap(targetThumbnailSize);
    thumbnail.fill(QColor::fromRgb(230, 230, 230));

    auto pos = (toPoint(thumbnail.size()) - toPoint(pixmap.size())) / 2;
    {
      QPainter pnt(&thumbnail);
      pnt.drawPixmap(pos, pixmap);
    }

    return std::make_unique<QPixmap>(std::move(thumbnail));
  }

}