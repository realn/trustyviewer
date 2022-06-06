#pragma once

#include <QString>
#include <QSize>

class QPixmap;

namespace realn {
  QPixmap loadScaled(const QString& filePath, const QSize& targetSize, const QPixmap& defaultResult);

  QPixmap createFilledThumbnail(const QPixmap& image, const QSize& targetSize);
}