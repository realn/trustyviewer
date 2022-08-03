
#include <QPushButton>
#include <QBoxLayout>
#include <QTime>

#include "VideoButtonsWidget.h"
#include "VideoMediaWidget.h"
#include "../Utils.h"

namespace realn {
  QString formatTime(qint64 durationMs) {
    auto time = QTime::fromMSecsSinceStartOfDay(durationMs);
    return time.toString(Qt::TextDate);
  }

  VideoMediaWidget::VideoMediaWidget() {
    videoPlayer = std::make_unique<QMediaPlayer>();
    videoWidget = new QVideoWidget();
    videoPlayer->setVideoOutput(videoWidget);
    videoWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    cC(connect(videoPlayer.get(), &QMediaPlayer::positionChanged, this, &VideoMediaWidget::onVideoPositionChanged));
    cC(connect(videoPlayer.get(), &QMediaPlayer::stateChanged, this, &VideoMediaWidget::onVideoStateChanged));

    videoButtonsWidget = new VideoButtonsWidget();

    cC(connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionChanged, this, &VideoMediaWidget::onSliderPositionChanged));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionPressed, this, &VideoMediaWidget::onSliderPositionPressed));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionReleased, this, &VideoMediaWidget::onSliderPositionReleased));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::videoPositionTimeout, this, &VideoMediaWidget::updateSliderPosition));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::playClicked, this, &VideoMediaWidget::play));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::pauseClicked, this, &VideoMediaWidget::pause));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::stopClicked, this, &VideoMediaWidget::stop));
    cC(connect(videoButtonsWidget, &VideoButtonsWidget::volumeSliderPositionChanged, this, &VideoMediaWidget::onVolumePositionChanged));

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

    videoPlayer->stop();
    videoPlayer->setMedia(QMediaContent());

    video = std::move(newVideo);
    videoPlayer->setMedia(*video);

    videoButtonsWidget->setVideoPosition(0.0f);
    videoButtonsWidget->setVolume(videoPlayer->volume());
    
    videoButtonsWidget->forcePlay();

    return true;
  }

  void VideoMediaWidget::clearMedia() {
    if (!video)
      return;

    videoPlayer->stop();
    videoPlayer->setMedia(QMediaContent());
    video.reset();
  }

  void VideoMediaWidget::play() {
    if (!video)
      return;
    videoPlayer->play();
  }

  void VideoMediaWidget::pause() {
    if (!video)
      return;
    videoPlayer->pause();
  }

  void VideoMediaWidget::stop() {
    if (!video)
      return;
    videoPlayer->stop();
  }

  void VideoMediaWidget::toggleRepeat() {
  }

  void VideoMediaWidget::onSliderPositionPressed() {
    if (!video)
      return;
    videoPlayer->pause();
  }

  void VideoMediaWidget::onSliderPositionReleased() {
    if (!video)
      return;
    videoPlayer->setPosition(getSliderPositionForPlayer());
    if(videoButtonsWidget->getState() == VideoState::PLAYING)
      videoPlayer->play();
  }

  void VideoMediaWidget::onSliderPositionChanged() {
    if (!video)
      return;
    videoPlayer->setPosition(getSliderPositionForPlayer());
  }

  void VideoMediaWidget::updateSliderPosition() {
    if (!video)
      return;

    videoButtonsWidget->setVideoPosition(getPlayerPositionForSlider());

    auto maxDuration = videoPlayer->duration();
    auto position = videoPlayer->position();

    auto text = formatTime(position) + "/" + formatTime(maxDuration);
    videoButtonsWidget->setProgressLabel(text);
  }

  void VideoMediaWidget::onVolumePositionChanged() {
    if (!video)
      return;
    videoPlayer->setVolume(videoButtonsWidget->getVolume());
  }

  void VideoMediaWidget::onVideoStateChanged(QMediaPlayer::State state) {
    if(state == QMediaPlayer::State::StoppedState)
      videoButtonsWidget->finishVideo();
  }

  qint64 VideoMediaWidget::getSliderPositionForPlayer() const {
    if (!video)
      return 1;
    auto pos = videoButtonsWidget->getVideoPosition() * videoPlayer->duration();
    return static_cast<qint64>(pos);
  }

  float VideoMediaWidget::getPlayerPositionForSlider() const {
    if (!video)
      return 1.0f;
    return static_cast<float>(videoPlayer->position()) / videoPlayer->duration();
  }

  void VideoMediaWidget::onVideoPositionChanged(qint64 pos) {
    updateSliderPosition();
  }
}