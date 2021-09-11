#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>
#include <QImage>
#include <QMovie>

#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"

namespace realn {
  class ImageView : public QWidget {
    Q_OBJECT;
  public:
    ImageView(std::shared_ptr<ExtPluginList> plugins);

    void setImageFromPath(const QString& filePath);
    void setImageFromItem(MediaItem::ptr_t item);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void loadScaledImage();
    void rescaleImage();

    QPointer<QLabel> label;
    std::unique_ptr<QImage> image;
    std::unique_ptr<QMovie> movie;

    QPixmap shownImage;
    
    std::shared_ptr<ExtPluginList> plugins;
  };
}