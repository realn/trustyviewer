
#include <QBoxLayout>
#include <QTimer>

#include "VideoButtonsWidget.h"
#include "AnimationMediaWidget.h"

namespace realn {
  AnimationMediaWidget::AnimationMediaWidget() {
    animationPlayer = new QLabel();
    animationPlayer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
    animationPlayer->setAlignment(Qt::AlignCenter);

    videoButtonsWidget = new VideoButtonsWidget();
    videoButtonsWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionChanged, this, &AnimationMediaWidget::onSliderPositionChanged);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionPressed, this, &AnimationMediaWidget::onSliderPositionPressed);
    connect(videoButtonsWidget, &VideoButtonsWidget::sliderPositionReleased, this, &AnimationMediaWidget::onSliderPositionReleased);
    connect(videoButtonsWidget, &VideoButtonsWidget::videoPositionTimeout, this, &AnimationMediaWidget::updateSliderPosition);
    connect(videoButtonsWidget, &VideoButtonsWidget::playClicked, this, &AnimationMediaWidget::play);
    connect(videoButtonsWidget, &VideoButtonsWidget::pauseClicked, this, &AnimationMediaWidget::pause);
    connect(videoButtonsWidget, &VideoButtonsWidget::stopClicked, this, &AnimationMediaWidget::stop);

    auto layout = new QVBoxLayout();
    layout->addWidget(animationPlayer);
    layout->addWidget(videoButtonsWidget);
    setLayout(layout);

    timer = std::make_unique<QTimer>();
    timer->setSingleShot(false);
    
    connect(timer.get(), &QTimer::timeout, this, &AnimationMediaWidget::onTimerTimeout);
  }

  AnimationMediaWidget::~AnimationMediaWidget() = default;

  bool AnimationMediaWidget::loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) {
    if (!mediaItem || !plugin)
      return false;

    auto newMovie = plugin->loadAnimation(mediaItem->getFilePath());
    if (!newMovie)
      return false;

    movie = std::move(newMovie);
    animationPlayer->setMovie(movie.get());
    movie->setPaused(true);

    timer->setInterval(movie->nextFrameDelay());

    videoButtonsWidget->forcePlay();

    return true;
  }

  void AnimationMediaWidget::clearMedia() {
    timer->stop();
    if (movie)
      movie->stop();
    animationPlayer->setMovie(nullptr);
    movie.reset();
  }

  void AnimationMediaWidget::play() {
    if (movie->currentFrameNumber() == movie->frameCount() - 1)
      movie->jumpToFrame(0);
    timer->start();
  }

  void AnimationMediaWidget::pause() {
    timer->stop();
  }

  void AnimationMediaWidget::stop() {
    timer->stop();
    movie->jumpToFrame(0);
    movie->setPaused(true);
  }

  void AnimationMediaWidget::onSliderPositionChanged() {
    movie->jumpToFrame(getSliderPositionForAnimation());
    movie->setPaused(true);
  }

  void AnimationMediaWidget::onSliderPositionPressed() {
    timer->stop();
  }

  void AnimationMediaWidget::onSliderPositionReleased() {
    movie->jumpToFrame(getSliderPositionForAnimation());
    movie->setPaused(true);
    if (videoButtonsWidget->getState() == VideoState::PLAYING)
      timer->start();
  }

  void AnimationMediaWidget::updateSliderPosition() {
    videoButtonsWidget->setVideoPosition(getAnimationPositionForSlider());
  }

  void AnimationMediaWidget::onTimerTimeout() {
    if (movie->currentFrameNumber() == movie->frameCount() - 1) {
      timer->stop();
      videoButtonsWidget->finishVideo();
    }
    else {
      movie->jumpToNextFrame();
      movie->setPaused(true);
    }
    timer->setInterval(movie->nextFrameDelay());
    
  }

  int AnimationMediaWidget::getSliderPositionForAnimation() const {
    if(!movie)
      return 0;
    auto pos = videoButtonsWidget->getVideoPosition() * (movie->frameCount() - 1);
    return static_cast<int>(pos);
  }

  float AnimationMediaWidget::getAnimationPositionForSlider() const {
    if (!movie)
      return 0.0f;
    return static_cast<float>(movie->currentFrameNumber()) / (movie->frameCount() - 1);
  }
}