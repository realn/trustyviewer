#pragma once

#include "ExtPlugin.h"

namespace realn {
  class StdImageExtPlugin : public ExtPlugin {
  public:
    // Inherited via ExtPlugin
    QStringList getSupportedExts() const override;

    std::unique_ptr<QImage> loadImage(const QString& filepath) const override;
    result_t createThumbnail(const QString& filepath, QSize size) const override;
  };
}