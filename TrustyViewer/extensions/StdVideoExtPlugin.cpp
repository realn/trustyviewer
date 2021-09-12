
#include <QUrl>
#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QMediaPlayer>
#include <QPixmap>
#include <QEventLoop>

#include "DummyVideoSurface.h"
#include "StdVideoExtPlugin.h"

namespace realn {

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
    QEventLoop waitLoop;
    auto surface = std::make_unique<DummyVideoSurface>();
    QPixmap pix;
    {
      auto video = QMediaContent(QUrl(filepath));
      QMediaPlayer player;

      player.setVideoOutput(surface.get());
      player.setMedia(video);

      auto len = player.duration();
      auto pos = static_cast<qint64>(len * 0.2);

      player.setPosition(pos);

      waitLoop.connect(surface.get(), &DummyVideoSurface::imagePresented, &waitLoop, &QEventLoop::quit);

      player.play();

      waitLoop.exec();
      pix = QPixmap::fromImage(surface->outputFrame);
      player.stop();
    }

    return std::make_unique<QPixmap>(std::move(pix.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation)));
  }
}