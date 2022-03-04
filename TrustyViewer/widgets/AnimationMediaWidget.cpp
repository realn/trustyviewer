
#include <QBoxLayout>

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

    auto layout = new QVBoxLayout();
    layout->addWidget(animationPlayer);
    layout->addWidget(videoButtonsWidget);
    setLayout(layout);
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

    movie->start();

    return true;
  }

  void AnimationMediaWidget::clearMedia() {
    if (movie)
      movie->stop();
    animationPlayer->setMovie(nullptr);
    movie.reset();
  }

  void AnimationMediaWidget::onSliderPositionChanged() {
    movie->jumpToFrame(getSliderPositionForAnimation());
  }

  void AnimationMediaWidget::onSliderPositionPressed() {
    movie->setPaused(true);
  }

  void AnimationMediaWidget::onSliderPositionReleased() {
    movie->setPaused(false);
  }

  void AnimationMediaWidget::updateSliderPosition() {
    videoButtonsWidget->setVideoPosition(getAnimationPositionForSlider());
  }

  int AnimationMediaWidget::getSliderPositionForAnimation() const {
    if(!movie)
      return 0;
    auto pos = videoButtonsWidget->getVideoPosition() * movie->frameCount();
    return static_cast<int>(pos);
  }

  float AnimationMediaWidget::getAnimationPositionForSlider() const {
    if (!movie)
      return 0.0f;
    return static_cast<float>(movie->currentFrameNumber()) / movie->frameCount();
  }
}