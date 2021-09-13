#pragma once

#include <QPointer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>

#include "BaseMediaContentPlayerWidget.h"

namespace realn {
  class VideoMediaWidget : public BaseMediaContentPlayerWidget {
    Q_OBJECT;
  public:
    VideoMediaWidget();

    // Inherited via BaseMediaContentPlayerWidget
    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) override;
    virtual void clearMedia() override;

  private:
    QPointer<QVideoWidget> videoWidget;

    std::unique_ptr<QMediaPlayer> videoPlayer;
    std::unique_ptr<QMediaContent> video;

  };
}