#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QMovie>

#include "BaseMediaContentPlayerWidget.h"

class QTimer;

namespace realn {
  class VideoButtonsWidget;

  class AnimationMediaWidget : public BaseMediaContentPlayerWidget {
    Q_OBJECT;
  public:
    AnimationMediaWidget();
    ~AnimationMediaWidget();

    // Inherited via BaseMediaContentPlayerWidget
    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) override;
    virtual void clearMedia() override;

  public slots:
    void play();
    void pause();
    void stop();

  private slots:
    void onSliderPositionChanged();
    void onSliderPositionPressed();
    void onSliderPositionReleased();
    void updateSliderPosition();
    void onTimerTimeout();

  private:
    int getSliderPositionForAnimation() const;
    float getAnimationPositionForSlider() const;

    QPointer<QLabel> animationPlayer;
    QPointer<VideoButtonsWidget> videoButtonsWidget;
    std::unique_ptr<QMovie> movie;
    std::unique_ptr<QTimer> timer;
  };
}