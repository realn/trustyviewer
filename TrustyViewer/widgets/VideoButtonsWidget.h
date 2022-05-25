#pragma once

#include <QWidget>
#include <QPointer>

class QSlider;
class QPushButton;
class QFrame;
class QLabel;

namespace realn {
  enum class VideoState {
    PLAYING,
    PAUSED,
    STOPPED
  };

  class VideoButtonsWidget : public QWidget {
    Q_OBJECT;
  public:
    VideoButtonsWidget();
    ~VideoButtonsWidget();

    // 0.0f - 1.0f range
    float getVideoPosition() const;
    void setVideoPosition(float pos);

    // 0 - 100 range
    int getVolume() const;
    void setVolume(int value);

    bool isSliderPressed() const;

    VideoState getState() const;

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
    void onMuteButtonPressed();

  protected:
    virtual void closeEvent(QCloseEvent*) override;
    virtual void hideEvent(QHideEvent*) override;

  private:
    void updateVolumeLabel();

    QPointer<QPushButton> playButton;
    QPointer<QPushButton> stopButton;
    QPointer<QPushButton> muteButton;
    QPointer<QPushButton> volumeButton;
    QPointer<QSlider> sliderWidget;
    QPointer<QSlider> volumeWidget;
    QPointer<QLabel> volumeLabelWidget;
    std::unique_ptr<QTimer> timer;
    VideoState state = VideoState::STOPPED;
  };
}
