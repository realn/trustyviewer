
#include <QRect>
#include <QPainter>

#include "ImageMediaWidget.h"

namespace realn {
  namespace {
    QPoint toPoint(const QSize& size) {
      return QPoint(size.width(), size.height());
    }
  }

  ImageMediaWidget::ImageMediaWidget() = default;

  bool ImageMediaWidget::loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin)
  {
    if (!mediaItem || !plugin)
      return false;

    auto newImage = plugin->loadImage(mediaItem->getFilePath());
    if (!newImage)
      return false;
    
    image = std::move(newImage);
    repaint();

    return true;
  }

  void ImageMediaWidget::clearMedia()
  {
    image.reset();
  }

  void ImageMediaWidget::paintEvent(QPaintEvent* event)
  {
    if (!image)
      return;

    QPainter painter(this);

    auto targetSize = image->size().scaled(size(), Qt::KeepAspectRatio);
    auto pos = toPoint((size() - targetSize) / 2);

    painter.drawImage(QRect(pos, targetSize), *image);
  }
}