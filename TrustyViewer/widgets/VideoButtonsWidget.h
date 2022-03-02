#pragma once

#include <QWidget>
#include <QPointer>

class QSlider;

namespace realn {
  class VideoButtonsWidget : public QWidget {
    Q_OBJECT;
  public:
    VideoButtonsWidget();
    ~VideoButtonsWidget();

    // 0.0f - 1.0f range
    float getVideoPosition() const;
    void setVideoPosition(float pos);

    bool isSliderPressed() const;

  signals:
    void playClicked();
    void stopClicked();
    void pauseClicked();
    void sliderPositionPressed();
    void sliderPositionReleased();
    void sliderPositionChanged();
    void videoPositionTimeout();

  private slots:
    void onSliderPressed();
    void onSliderReleased();
    void onSliderValueChanged();
    void onTimerTimeout();

  private:
    QPointer<QSlider> sliderWidget;
    std::unique_ptr<QTimer> timer;
  };
}
