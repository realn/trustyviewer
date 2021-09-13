
#include <QBoxLayout>

#include "VideoMediaWidget.h"

namespace realn {
  VideoMediaWidget::VideoMediaWidget()
  {
    videoPlayer = std::make_unique<QMediaPlayer>();
    videoWidget = new QVideoWidget();
    videoPlayer->setVideoOutput(videoWidget);

    auto layout = new QHBoxLayout();
    layout->addWidget(videoWidget);
    setLayout(layout);
  }

  bool VideoMediaWidget::loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin)
  {
    if (!mediaItem || !plugin)
      return false;

    auto newVideo = plugin->loadVideo(mediaItem->getFilePath());
    if (!newVideo)
      return false;

    video = std::move(newVideo);
    videoPlayer->setMedia(*video);
    videoPlayer->play();

    return true;
  }

  void VideoMediaWidget::clearMedia()
  {
    videoPlayer->stop();
    videoPlayer->setMedia(QMediaContent());
  }
}