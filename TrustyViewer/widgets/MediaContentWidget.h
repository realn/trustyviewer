#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>
#include <QImage>
#include <QMovie>
#include <Qmovie>
#include <QStackedWidget>

#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"
#include "ImageMediaWidget.h"

namespace realn {
  class MediaContentWidget : public QWidget {
    Q_OBJECT;
  public:
    MediaContentWidget(std::shared_ptr<ExtPluginList> plugins);

    void setImageFromPath(const QString& filePath);
    void setImageFromItem(MediaItem::ptr_t item);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void loadScaledImage();
    void rescaleImage();

    QPointer<QStackedWidget> stack;
    QPointer<ImageMediaWidget> imageContent;
    QPointer<QLabel> animationPlayer;
    std::unique_ptr<QMovie> movie;

    
    std::shared_ptr<ExtPluginList> plugins;
  };
}