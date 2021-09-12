#pragma once

#include "ExtPlugin.h"

namespace realn {
  class StdVideoExtPlugin : public ExtPlugin {
  public:
    // Inherited via ExtPlugin
    QStringList getSupportedExts() const override;

    bool isVideo(const QString& fileExt) const override;

    std::unique_ptr<QMediaContent> loadVideo(const QString& filepath) const override;

    std::unique_ptr<QPixmap> createThumbnail(const QString& filepath, QSize size) const override;
  };
}