
#include <QPixmap>

#include "StdImageExtPlugin.h"

namespace realn {
  QStringList StdImageExtPlugin::getSupportedExts() const
  {
    return QStringList() << "jpg" << "jpeg" << "png" << "bmp";
  }

  std::unique_ptr<QImage> StdImageExtPlugin::loadImage(const QString& filepath) const
  {
    auto result = std::make_unique<QImage>();

    if (!result->load(filepath))
      return nullptr;

    return result;
  }

  StdImageExtPlugin::result_t StdImageExtPlugin::createThumbnail(const QString& filepath, QSize size) const
  {
    auto pix = QPixmap();
    if (!pix.load(filepath))
      return result_t{ nullptr, Errors::FailedToLoad };

    return result_t{ std::make_unique<QPixmap>(std::move(pix.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation))), Errors::NoError };
  }
}