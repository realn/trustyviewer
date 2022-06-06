
#include <QImage>
#include <QPixmap>
#include <QPainter>

#include "ImageUtils.h"

namespace realn {
  namespace {
    QPoint toPoint(const QSize& size) {
      return QPoint(size.width(), size.height());
    }
  }

  QPixmap loadScaled(const QString& filePath, const QSize& targetSize, const QPixmap& defaultResult) {
    auto img = QImage();
    if (!img.load(filePath))
      return defaultResult;

    img = img.scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    return QPixmap::fromImage(img);
  }

  QPixmap createFilledThumbnail(const QPixmap& image, const QSize& targetSize) {
    auto thumbnail = QPixmap(targetSize);
    thumbnail.fill(QColor::fromRgb(230, 230, 230));

    auto pos = (toPoint(thumbnail.size()) - toPoint(image.size())) / 2;
    {
      QPainter pnt(&thumbnail);
      pnt.drawPixmap(pos, image);
    }

    return thumbnail;
  }
}
