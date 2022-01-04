
#include <QImage>
#include <QPixmap>
#include <QMovie>

#include "GifImageExtPlugin.h"

namespace realn {
  QStringList GifImageExtPlugin::getSupportedExts() const {
    return QStringList() << "gif";
  }

  bool GifImageExtPlugin::isAnimated(const QString& ext) const {
    return getSupportedExts().contains(ext);
  }

  std::unique_ptr<QImage> GifImageExtPlugin::loadImage(const QString& filepath) const {
    return std::make_unique<QImage>(filepath);
  }

  std::unique_ptr<QMovie> GifImageExtPlugin::loadAnimation(const QString& filepath) const {
    return std::make_unique<QMovie>(filepath);
  }

  GifImageExtPlugin::result_t GifImageExtPlugin::createThumbnail(const QString& filepath, QSize size) const {
    auto pixmap = QPixmap(filepath);
    return result_t{ std::make_unique<QPixmap>(std::move(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation))), Errors::NoError };
  }
}