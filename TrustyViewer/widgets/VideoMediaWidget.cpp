
#include <QPushButton>
#include <QBoxLayout>
#include <QTime>

#include "VideoButtonsWidget.h"
#include "VideoMediaWidget.h"

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

    connect(videoPlayer.get(), &QMediaPlayer::positionChanged, this, &VideoMediaWidget::onVideoPositionChanged);
    connect(videoPlayer.get(), SIGNAL("error(Error)"), this, SLOT("onMediaError(Error)"));
    connect(videoPlayer.get(), &QMediaPlayer::stateChanged, this, &VideoMediaWidget::onVideoStateChanged);

    videoButtonsWidget = new VideoButtonsWidget();

    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionChanged, this, &VideoMediaWidget::onSliderPositionChanged);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionPressed, this, &VideoMediaWidget::onSliderPositionPressed);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionReleased, this, &VideoMediaWidget::onSliderPositionReleased);
    connect(videoButtonsWidget, &VideoButtonsWidget::videoPositionTimeout, this, &VideoMediaWidget::updateSliderPosition);
    connect(videoButtonsWidget, &VideoButtonsWidget::playClicked, this, &VideoMediaWidget::play);
    connect(videoButtonsWidget, &VideoButtonsWidget::pauseClicked, this, &VideoMediaWidget::pause);
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
    if (video) {
      videoButtonsWidget->setVideoPosition(getPlayerPositionForSlider());

      auto maxDuration = videoPlayer->duration();
      auto position = videoPlayer->position();

      auto text = formatTime(position) + "/" + formatTime(maxDuration);
      videoButtonsWidget->setProgressLabel(text);
    }
  }

  void VideoMediaWidget::onVolumePositionChanged() {
    videoPlayer->setVolume(videoButtonsWidget->getVolume());
  }

  void VideoMediaWidget::onVideoStateChanged(QMediaPlayer::State state) {
    if(state == QMediaPlayer::State::StoppedState)
      videoButtonsWidget->finishVideo();
  }

  void VideoMediaWidget::onMediaError(QMediaPlayer::Error err) {
    assert(err == QMediaPlayer::Error::NoError);
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