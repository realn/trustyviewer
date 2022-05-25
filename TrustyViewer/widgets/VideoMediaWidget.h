#pragma once

#include <QPointer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QSlider>
#include <QTimer>

#include "BaseMediaContentPlayerWidget.h"

namespace realn {
  class VideoButtonsWidget;

  class VideoMediaWidget : public BaseMediaContentPlayerWidget {
    Q_OBJECT;
  public:
    VideoMediaWidget();
    ~VideoMediaWidget();

    // Inherited via BaseMediaContentPlayerWidget
    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) override;
    virtual void clearMedia() override;

  public slots:
    void play();
    void pause();
    void stop();
    void toggleRepeat();

  private slots:
    void onVideoPositionChanged(qint64 pos);
    void onSliderPositionChanged();
    void onSliderPositionPressed();
    void onSliderPositionReleased();
    void updateSliderPosition();
    void onVolumePositionChanged();

  private:
    qint64 getSliderPositionForPlayer() const;
    float getPlayerPositionForSlider() const;

    QPointer<QVideoWidget> videoWidget;
    QPointer<VideoButtonsWidget> videoButtonsWidget;

    std::unique_ptr<QMediaPlayer> videoPlayer;
    std::unique_ptr<QMediaContent> video;
  };
}