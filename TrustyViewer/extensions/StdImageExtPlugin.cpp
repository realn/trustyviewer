
#include "StdImageExtPlugin.h"

namespace realn {
  StdImageExtPlugin::StdImageExtPlugin()
  {
  }
  QStringList StdImageExtPlugin::getSupportedExts() const
  {
    return QStringList() << "jpg" << "jpeg" << "png" << "bmp";
  }
  bool StdImageExtPlugin::isAnimated(const QString& ext) const
  {
    return false;
  }
  std::unique_ptr<QImage> StdImageExtPlugin::loadImage(const QString& filepath) const
  {
    auto result = std::make_unique<QImage>();

    if (!result->load(filepath))
      return nullptr;

    return result;
  }

  std::unique_ptr<QMovie> StdImageExtPlugin::loadMovie(const QString& filepath) const
  {
    return std::unique_ptr<QMovie>();
  }
}