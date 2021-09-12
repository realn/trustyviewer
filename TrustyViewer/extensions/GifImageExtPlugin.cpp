
#include <QImage>
#include <QPixmap>
#include <QMovie>

#include "GifImageExtPlugin.h"

namespace realn {
  QStringList GifImageExtPlugin::getSupportedExts() const
  {
    return QStringList() << "gif";
  }

  std::unique_ptr<QImage> GifImageExtPlugin::loadImage(const QString& filepath) const
  {
    return std::make_unique<QImage>(filepath);
  }

  std::unique_ptr<QMovie> GifImageExtPlugin::loadAnimation(const QString& filepath) const
  {
    return std::make_unique<QMovie>(filepath);
  }

  std::unique_ptr<QPixmap> GifImageExtPlugin::createThumbnail(const QString& filepath, QSize size) const
  {
    auto pixmap = QPixmap(filepath);
    return std::make_unique<QPixmap>(std::move(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation)));
  }
}