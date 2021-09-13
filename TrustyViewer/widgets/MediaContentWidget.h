#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>
#include <QImage>
#include <QMovie>
#include <QStackedWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>

#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"
#include "ImageMediaWidget.h"
#include "AnimationMediaWidget.h"

namespace realn {
  class MediaContentWidget : public QWidget {
    Q_OBJECT;
  public:
    MediaContentWidget(std::shared_ptr<ExtPluginList> plugins);

    void setMediaFromPath(const QString& filePath);
    void setMediaFromItem(MediaItem::ptr_t item);

  private:
    QPointer<QStackedWidget> stack;
    QPointer<ImageMediaWidget> imageContent;
    QPointer<AnimationMediaWidget> animatedContent;
    QPointer<QVideoWidget> videoWidget;

    std::unique_ptr<QMediaPlayer> videoPlayer;
    std::unique_ptr<QMediaContent> video;
    std::shared_ptr<ExtPluginList> plugins;
  };
}