
#include <QRect>
#include <QPainter>

#include "ImageMediaWidget.h"

namespace realn {
  namespace {
    QPoint toPoint(const QSize& size) {
      return QPoint(size.width(), size.height());
    }
  }

  ImageMediaWidget::ImageMediaWidget()
  {
  }

  void ImageMediaWidget::setImage(std::unique_ptr<QImage> value)
  {
    image = std::move(value);
    repaint();
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