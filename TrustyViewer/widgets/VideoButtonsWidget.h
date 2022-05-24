#pragma once

#include <QWidget>
#include <QPointer>

class QSlider;
class QPushButton;

namespace realn {
  class VideoButtonsWidget : public QWidget {
    Q_OBJECT;
  public:
    VideoButtonsWidget();
    ~VideoButtonsWidget();

    // 0.0f - 1.0f range
    float getVideoPosition() const;
    void setVideoPosition(float pos);

    int getVolume() const;
    void setVolume(int value);

    bool isSliderPressed() const;

  signals:
    void playClicked();
    void stopClicked();
    void pauseClicked();
    void sliderPositionPressed();
    void sliderPositionReleased();
    void sliderPositionChanged();
    void videoPositionTimeout();
    void volumeSliderPositionChanged();

  private slots:
    void onSliderPressed();
    void onSliderReleased();
    void onSliderValueChanged();
    void onTimerTimeout();
    void onPlayButtonPressed();
    void onStopButtonPressed();
    void onVolumeButtonPressed();
    void onVolumeSliderValueChanged();

  private:
    QPointer<QPushButton> playButton;
    QPointer<QPushButton> stopButton;
    QPointer<QPushButton> muteButton;
    QPointer<QPushButton> volumeButton;
    QPointer<QSlider> sliderWidget;
    QPointer<QSlider> volumeWidget;
    std::unique_ptr<QTimer> timer;
  };
}
