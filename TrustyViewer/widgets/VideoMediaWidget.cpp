
#include <QPushButton>
#include <QBoxLayout>

#include "VideoButtonsWidget.h"
#include "VideoMediaWidget.h"

namespace realn {
  VideoMediaWidget::VideoMediaWidget() {
    videoPlayer = std::make_unique<QMediaPlayer>();
    videoWidget = new QVideoWidget();
    videoPlayer->setVideoOutput(videoWidget);
    videoWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    connect(videoPlayer.get(), &QMediaPlayer::positionChanged, this, &VideoMediaWidget::onVideoPositionChanged);

    videoButtonsWidget = new VideoButtonsWidget();

    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionChanged, this, &VideoMediaWidget::onSliderPositionChanged);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionPressed, this, &VideoMediaWidget::onSliderPositionPressed);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionReleased, this, &VideoMediaWidget::onSliderPositionReleased);
    connect(videoButtonsWidget, &VideoButtonsWidget::videoPositionTimeout, this, &VideoMediaWidget::updateSliderPosition);
    connect(videoButtonsWidget, &VideoButtonsWidget::playClicked, this, &VideoMediaWidget::play);
    connect(videoButtonsWidget, &VideoButtonsWidget::playClicked, this, &VideoMediaWidget::pause);
    connect(videoButtonsWidget, &VideoButtonsWidget::stopClicked, this, &VideoMediaWidget::stop);
    connect(videoButtonsWidget, &VideoButtonsWidget::volumeSliderPositionChanged, this, &VideoMediaWidget::onVolumePositionChanged);

    auto layout = new QVBoxLayout();
    layout->addWidget(videoWidget);
    layout->addWidget(videoButtonsWidget);
    setLayout(layout);
  }

  VideoMediaWidget::~VideoMediaWidget() = default;

  bool VideoMediaWidget::loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) {
    if (!mediaItem || !plugin)
      return false;

    auto newVideo = plugin->loadVideo(mediaItem->getFilePath());
    if (!newVideo)
      return false;

    video = std::move(newVideo);
    videoPlayer->setMedia(*video);

    videoButtonsWidget->setVideoPosition(0.0f);
    videoButtonsWidget->setVolume(videoPlayer->volume());

    videoPlayer->play();
    

    return true;
  }

  void VideoMediaWidget::clearMedia() {
    videoPlayer->stop();
    videoPlayer->setMedia(QMediaContent());
  }

  void VideoMediaWidget::play() {
    videoPlayer->play();
  }

  void VideoMediaWidget::pause() {
    videoPlayer->pause();
  }

  void VideoMediaWidget::stop() {
    videoPlayer->stop();
  }

  void VideoMediaWidget::toggleRepeat() {
  }

  void VideoMediaWidget::onSliderPositionPressed() {
    videoPlayer->pause();
  }

  void VideoMediaWidget::onSliderPositionReleased() {
    videoPlayer->setPosition(getSliderPositionForPlayer());
    if(videoButtonsWidget->getState() == VideoState::PLAYING)
      videoPlayer->play();
  }

  void VideoMediaWidget::onSliderPositionChanged() {
    videoPlayer->setPosition(getSliderPositionForPlayer());
  }

  void VideoMediaWidget::updateSliderPosition() {
    if (video)
      videoButtonsWidget->setVideoPosition(getPlayerPositionForSlider());
  }

  void VideoMediaWidget::onVolumePositionChanged() {
    videoPlayer->setVolume(videoButtonsWidget->getVolume());
  }

  qint64 VideoMediaWidget::getSliderPositionForPlayer() const {
    auto pos = videoButtonsWidget->getVideoPosition() * videoPlayer->duration();
    return static_cast<qint64>(pos);
  }

  float VideoMediaWidget::getPlayerPositionForSlider() const {
    return static_cast<float>(videoPlayer->position()) / videoPlayer->duration();
  }

  void VideoMediaWidget::onVideoPositionChanged(qint64 pos) {
    updateSliderPosition();
  }
}