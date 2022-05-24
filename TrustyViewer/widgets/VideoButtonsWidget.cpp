
#include <QBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QPushButton>

#include "VideoButtonsWidget.h"

namespace realn {
  constexpr auto MAX_VIDEO_POS = 1.0f;
  constexpr auto MAX_SLIDER_POS = 1000;

  constexpr auto MAX_VOLUME_SLIDER_POS = 100;

  constexpr auto toSliderPos(float videoPos) {
    return static_cast<int>(videoPos * MAX_SLIDER_POS);
  }
  constexpr auto toVideoPos(int sliderPos) {
    return static_cast<float>(sliderPos) / MAX_SLIDER_POS;
  }


  VideoButtonsWidget::VideoButtonsWidget() {
    playButton = new QPushButton("P");
    playButton->setContentsMargins(0, 0, 0, 0);
    playButton->setMinimumSize(24, 24);
    playButton->setBaseSize(24, 24);
    playButton->setFixedSize(24, 24);

    connect(playButton, &QPushButton::clicked, this, &VideoButtonsWidget::onPlayButtonPressed);

    stopButton = new QPushButton("S");
    stopButton->setContentsMargins(0, 0, 0, 0);
    stopButton->setMinimumSize(10, 10);
    stopButton->setMinimumSize(24, 24);
    stopButton->setBaseSize(24, 24);
    stopButton->setFixedSize(24, 24);

    connect(stopButton, &QPushButton::clicked, this, &VideoButtonsWidget::onStopButtonPressed);

    volumeButton = new QPushButton("V");
    volumeButton->setContentsMargins(0, 0, 0, 0);
    volumeButton->setMinimumSize(10, 10);
    volumeButton->setMinimumSize(24, 24);
    volumeButton->setBaseSize(24, 24);
    volumeButton->setFixedSize(24, 24);

    connect(volumeButton, &QPushButton::clicked, this, &VideoButtonsWidget::onVolumeButtonPressed);

    sliderWidget = new QSlider(Qt::Horizontal, this);
    sliderWidget->setRange(0, MAX_SLIDER_POS);

    connect(sliderWidget, &QSlider::sliderPressed, this, &VideoButtonsWidget::onSliderPressed);
    connect(sliderWidget, &QSlider::sliderReleased, this, &VideoButtonsWidget::onSliderReleased);
    connect(sliderWidget, &QSlider::valueChanged, this, &VideoButtonsWidget::onSliderValueChanged);

    volumeWidget = new QSlider(Qt::Vertical, this);
    volumeWidget->setFixedSize(24, 200);
    volumeWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    connect(volumeWidget, &QSlider::valueChanged, this, &VideoButtonsWidget::onVolumeSliderValueChanged);

    timer = std::make_unique<QTimer>();
    timer->setInterval(std::chrono::milliseconds(50));
    timer->setSingleShot(false);
    connect(timer.get(), &QTimer::timeout, this, &VideoButtonsWidget::onTimerTimeout);

    auto layout = new QHBoxLayout();
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(sliderWidget);
    layout->addWidget(volumeButton);
    setLayout(layout);

    timer->start();
  }

  VideoButtonsWidget::~VideoButtonsWidget() = default;

  float VideoButtonsWidget::getVideoPosition() const {
    return toVideoPos(sliderWidget->value());
  }

  void VideoButtonsWidget::setVideoPosition(float pos) {
    if (!sliderWidget->isSliderDown())
      sliderWidget->setValue(toSliderPos(pos));
  }

  int VideoButtonsWidget::getVolume() const {
    return volumeWidget->value();;
  }

  void VideoButtonsWidget::setVolume(int value) {
    volumeWidget->setValue(value);
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
    if (sliderWidget->isSliderDown())
      emit sliderPositionChanged();
  }

  void VideoButtonsWidget::onTimerTimeout() {
    emit videoPositionTimeout();
  }

  void VideoButtonsWidget::onPlayButtonPressed() {
    emit playClicked();
  }

  void VideoButtonsWidget::onStopButtonPressed() {
    emit stopClicked();
  }

  void VideoButtonsWidget::onVolumeButtonPressed() {
    if (volumeWidget->isVisible()) {
      volumeWidget->hide();
      return;
    }

    auto rect = volumeButton->geometry();
    auto point = this->mapToGlobal(rect.topLeft());
    point.ry() -= volumeWidget->height();
    volumeWidget->setGeometry(QRect(point, volumeWidget->size()));
    volumeWidget->show();
  }

  void VideoButtonsWidget::onVolumeSliderValueChanged() {
    emit volumeSliderPositionChanged();
  }
}