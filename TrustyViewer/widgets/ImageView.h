#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>
#include <QImage>
#include <QMovie>

#include "../extensions/ExtPlugin.h"

namespace realn {
  class ImageView : public QWidget {
    Q_OBJECT;
  public:
    ImageView(std::shared_ptr<ExtPluginList> plugins);

    void setImage(QFileInfo fileInfo);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void ResetImage();

    QPointer<QLabel> label;
    std::unique_ptr<QImage> image;
    std::unique_ptr<QMovie> movie;

    QPixmap shownImage;
    
    std::shared_ptr<ExtPluginList> plugins;
  };
}