#pragma once

#include "ExtPlugin.h"

namespace realn {
  class StdImageExtPlugin : public ExtPlugin {
  public:
    StdImageExtPlugin();

    // Inherited via ExtPlugin
    virtual QStringList getSupportedExts() const override;

    virtual bool isAnimated(const QString& ext) const override;

    virtual std::unique_ptr<QImage> loadImage(const QString& filepath) const override;

    virtual std::unique_ptr<QMovie> loadMovie(const QString& filepath) const override;
  };
}