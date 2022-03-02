
#include <QBoxLayout>
#include <QSlider>
#include <QTimer>

#include "VideoButtonsWidget.h"

namespace realn {
  constexpr auto MAX_VIDEO_POS = 1.0f;
  constexpr auto MAX_SLIDER_POS = 1000;

  constexpr auto toSliderPos(float videoPos) {
    return static_cast<int>(videoPos * MAX_SLIDER_POS);
  }
  constexpr auto toVideoPos(int sliderPos) {
    return static_cast<float>(sliderPos) / MAX_SLIDER_POS;
  }


  VideoButtonsWidget::VideoButtonsWidget() {
    sliderWidget = new QSlider(Qt::Horizontal, this);
    sliderWidget->setRange(0, MAX_SLIDER_POS);

    connect(sliderWidget, &QSlider::sliderPressed, this, &VideoButtonsWidget::onSliderPressed);
    connect(sliderWidget, &QSlider::sliderReleased, this, &VideoButtonsWidget::onSliderReleased);
    connect(sliderWidget, &QSlider::valueChanged, this, &VideoButtonsWidget::onSliderValueChanged);

    timer = std::make_unique<QTimer>();
    timer->setInterval(std::chrono::milliseconds(50));
    timer->setSingleShot(false);
    connect(timer.get(), &QTimer::timeout, this, &VideoButtonsWidget::onTimerTimeout);

    auto layout = new QHBoxLayout();
    layout->addWidget(sliderWidget);
    setLayout(layout);

    timer->start();
  }

  VideoButtonsWidget::~VideoButtonsWidget() = default;

  float VideoButtonsWidget::getVideoPosition() const {
    return toVideoPos(sliderWidget->value());
  }

  void VideoButtonsWidget::setVideoPosition(float pos) {
    if(!sliderWidget->isSliderDown())
      sliderWidget->setValue(toSliderPos(pos));
  }

  bool VideoButtonsWidget::isSliderPressed() const {
    return sliderWidget->isSliderDown();
  }

  void VideoButtonsWidget::onSliderPressed() {
    emit sliderPositionPressed();
  }

  void VideoButtonsWidget::onSliderReleased() {
    emit sliderPositionReleased();
  }

  void VideoButtonsWidget::onSliderValueChanged() {
    if(sliderWidget->isSliderDown())
      emit sliderPositionChanged();
  }

  void VideoButtonsWidget::onTimerTimeout() {
    emit videoPositionTimeout();
  }
}