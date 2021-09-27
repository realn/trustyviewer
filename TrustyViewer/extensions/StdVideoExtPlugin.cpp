
#include <thread>

#include <QUrl>
#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QMediaPlayer>
#include <QPixmap>
#include <QEventLoop>

#include "DummyVideoSurface.h"
#include "StdVideoExtPlugin.h"

namespace realn {
  namespace {
    template<class _Func>
    void waitForSignal(const typename QtPrivate::FunctionPointer<_Func>::Object* sender, _Func signal) {
      QEventLoop waitLoop;
      waitLoop.connect(sender, signal, &waitLoop, &QEventLoop::quit);
      waitLoop.exec();
    }
  }

  StdVideoExtPlugin::StdVideoExtPlugin()
  {
    player = std::make_unique<QMediaPlayer>();
  }

  StdVideoExtPlugin::~StdVideoExtPlugin() = default;

  QStringList StdVideoExtPlugin::getSupportedExts() const
  {
    return QStringList() << "mp4" << "avi" << "webm";
  }

  bool StdVideoExtPlugin::isVideo(const QString& fileExt) const
  {
    return getSupportedExts().contains(fileExt);
  }
  
  std::unique_ptr<QMediaContent> StdVideoExtPlugin::loadVideo(const QString& filepath) const
  {
    return std::make_unique<QMediaContent>(QUrl(filepath));
  }

  std::unique_ptr<QPixmap> StdVideoExtPlugin::createThumbnail(const QString& filepath, QSize size) const
  {
    auto surface = std::make_unique<DummyVideoSurface>();
    QPixmap pix;
    {
      auto lock = std::unique_lock<std::mutex>(mutex);
      auto video = QMediaContent(QUrl(filepath));

      player->setVideoOutput(surface.get());
      player->setMedia(video);
      player->setVolume(0);

      auto len = player->duration();
      auto pos = static_cast<qint64>(len * 0.2);

      player->setPosition(pos);
      player->play();
      waitForSignal(surface.get(), &DummyVideoSurface::imagePresented);

      pix = QPixmap::fromImage(surface->outputFrame);
      player->stop();
      player->setMedia(QMediaContent());
    }

    return std::make_unique<QPixmap>(std::move(pix.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation)));
  }
}