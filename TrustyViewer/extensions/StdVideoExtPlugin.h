#pragma once

#include <memory>
#include <mutex>

#include "ExtPlugin.h"

class QMediaPlayer;

namespace realn {
  class StdVideoExtPlugin : public ExtPlugin {
  public:
    StdVideoExtPlugin();
    ~StdVideoExtPlugin();

    // Inherited via ExtPlugin
    QStringList getSupportedExts() const override;

    bool isVideo(const QString& fileExt) const override;

    std::unique_ptr<QMediaContent> loadVideo(const QString& filepath) const override;

    std::unique_ptr<QPixmap> createThumbnail(const QString& filepath, QSize size) const override;

  private:
    std::unique_ptr<QMediaPlayer> player;
    mutable std::mutex mutex;
  };
}