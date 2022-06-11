
#include <QBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QPushButton>
#include <QWidget>
#include <QFrame>
#include <QLabel>

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

  const auto VIDEO_STATE_MAP = std::map<VideoState, VideoState>{
    { VideoState::PLAYING, VideoState::PAUSED},
    { VideoState::PAUSED, VideoState::PLAYING},
    { VideoState::STOPPED, VideoState::PLAYING }
  };

  VideoButtonsWidget::VideoButtonsWidget() {
    playButton = new QPushButton("P");
    playButton->setContentsMargins(0, 0, 0, 0);
    playButton->setMinimumSize(24, 24);
    playButton->setBaseSize(24, 24);
    playButton->setFixedSize(24, 24);
    playButton->setCheckable(true);

    connect(playButton, &QPushButton::clicked, this, &VideoButtonsWidget::onPlayButtonPressed);

    stopButton = new QPushButton("S");
    stopButton->setContentsMargins(0, 0, 0, 0);
    stopButton->setMinimumSize(24, 24);
    stopButton->setBaseSize(24, 24);
    stopButton->setFixedSize(24, 24);

    connect(stopButton, &QPushButton::clicked, this, &VideoButtonsWidget::onStopButtonPressed);

    loopButton = new QPushButton("L");
    loopButton->setContentsMargins(0, 0, 0, 0);
    loopButton->setMinimumSize(24, 24);
    loopButton->setBaseSize(24, 24);
    loopButton->setFixedSize(24, 24);
    loopButton->setCheckable(true);

    connect(loopButton, &QPushButton::clicked, this, &VideoButtonsWidget::onLoopButtonPressed);

    volumeButton = new QPushButton("V");
    volumeButton->setContentsMargins(0, 0, 0, 0);
    volumeButton->setMinimumSize(10, 10);
    volumeButton->setMinimumSize(24, 24);
    volumeButton->setBaseSize(24, 24);
    volumeButton->setFixedSize(24, 24);

    connect(volumeButton, &QPushButton::clicked, this, &VideoButtonsWidget::onVolumeButtonPressed);

    muteButton = new QPushButton("M");
    muteButton->setContentsMargins(0, 0, 0, 0);
    muteButton->setMinimumSize(24, 24);
    muteButton->setBaseSize(24, 24);
    muteButton->setFixedSize(24, 24);
    muteButton->setCheckable(true);

    connect(muteButton, &QPushButton::clicked, this, &VideoButtonsWidget::onMuteButtonPressed);

    sliderWidget = new QSlider(Qt::Horizontal, this);
    sliderWidget->setRange(0, MAX_SLIDER_POS);

    connect(sliderWidget, &QSlider::sliderPressed, this, &VideoButtonsWidget::onSliderPressed);
    connect(sliderWidget, &QSlider::sliderReleased, this, &VideoButtonsWidget::onSliderReleased);
    connect(sliderWidget, &QSlider::valueChanged, this, &VideoButtonsWidget::onSliderValueChanged);

    volumeWidget = new QSlider(Qt::Vertical, this);
    //volumeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    volumeWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    volumeWidget->setFixedSize(24, 200);
    volumeWidget->setRange(0, MAX_VOLUME_SLIDER_POS);
    volumeWidget->setValue(MAX_VOLUME_SLIDER_POS);
    volumeWidget->setContentsMargins(0, 0, 0, 0);

    connect(volumeWidget, &QSlider::valueChanged, this, &VideoButtonsWidget::onVolumeSliderValueChanged);

    volumeLabelWidget = new QLabel(this);
    volumeLabelWidget->setText("100%");

    timer = std::make_unique<QTimer>();
    timer->setInterval(std::chrono::milliseconds(50));
    timer->setSingleShot(false);
    connect(timer.get(), &QTimer::timeout, this, &VideoButtonsWidget::onTimerTimeout);

    progressLabelWidget = new QLabel();
    progressLabelWidget->setAlignment(Qt::AlignCenter);

    auto layout = new QHBoxLayout();
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(loopButton);
    layout->addWidget(progressLabelWidget);
    layout->addWidget(sliderWidget);
    layout->addWidget(volumeLabelWidget);
    layout->addWidget(volumeButton);
    layout->addWidget(muteButton);
    setLayout(layout);

    timer->start();
  }

  VideoButtonsWidget::~VideoButtonsWidget() {
    volumeWidget->hide();
  }

  float VideoButtonsWidget::getVideoPosition() const {
    return toVideoPos(sliderWidget->value());
  }

  void VideoButtonsWidget::setVideoPosition(float pos) {
    if (!sliderWidget->isSliderDown())
      sliderWidget->setValue(toSliderPos(pos));
  }

  int VideoButtonsWidget::getVolume() const {
    if (muteButton->isChecked())
      return 0;
    return volumeWidget->value();
  }

  void VideoButtonsWidget::setVolume(int value) {
    volumeWidget->setValue(value);
    updateVolumeLabel();
  }

  bool VideoButtonsWidget::isSliderPressed() const {
    return sliderWidget->isSliderDown();
  }

  void VideoButtonsWidget::resetState() {
    state = VideoState::STOPPED;
  }

  VideoState VideoButtonsWidget::getState() const {
    return state;
  }

  void VideoButtonsWidget::forcePlay() {
    stopButton->click();
    playButton->click();
  }

  void VideoButtonsWidget::setProgressLabel(const QString& text) {
    progressLabelWidget->setText(text);
  }

  void VideoButtonsWidget::setVolumeControlVisibility(bool value) {
    volumeButton->setVisible(value);
    muteButton->setVisible(value);
    volumeLabelWidget->setVisible(value);
  }

  void VideoButtonsWidget::finishVideo() {
    state = VideoState::STOPPED;
    if (loopVideo)
      onPlayButtonPressed();
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
    state = VIDEO_STATE_MAP.at(state);
    updateStates();
  }

  void VideoButtonsWidget::onStopButtonPressed() {
    state = VideoState::STOPPED;
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
    volumeWidget->updateGeometry();
  }

  void VideoButtonsWidget::onVolumeSliderValueChanged() {
    emit volumeSliderPositionChanged();
    updateVolumeLabel();
  }

  void VideoButtonsWidget::onMuteButtonPressed() {
    emit volumeSliderPositionChanged();
    updateVolumeLabel();
  }

  void VideoButtonsWidget::onLoopButtonPressed() {
    loopVideo = !loopVideo;
    loopButton->setChecked(loopVideo);
  }

  void VideoButtonsWidget::closeEvent(QCloseEvent* evt) {
    volumeWidget->hide();
    QWidget::closeEvent(evt);
  }

  void VideoButtonsWidget::hideEvent(QHideEvent* evt) {
    volumeWidget->hide();
    QWidget::hideEvent(evt);
  }

  void VideoButtonsWidget::updateVolumeLabel() {
    if (muteButton->isChecked()) {
      volumeLabelWidget->setText("MUTE");
      return;
    }

    int volume = volumeWidget->value();
    volumeLabelWidget->setText(QString::number(volume) + "%");
  }

  void VideoButtonsWidget::updateStates() {
    switch (state) {
    case realn::VideoState::PLAYING:
      playButton->setChecked(false);
      emit playClicked();
      break;

    case realn::VideoState::PAUSED:
      playButton->setChecked(true);
      emit pauseClicked();
      break;

    case realn::VideoState::STOPPED:
      playButton->setChecked(false);
      emit stopClicked();
      setVideoPosition(0.0f);
      break;
    }
  }
}
