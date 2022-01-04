
#include "DummyVideoSurface.h"

namespace realn {
  // Inherited via QAbstractVideoSurface

  QList<QVideoFrame::PixelFormat> DummyVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
  {
    return QList<QVideoFrame::PixelFormat>() << QVideoFrame::PixelFormat::Format_ARGB32;
  }

  bool DummyVideoSurface::present(const QVideoFrame& frame)
  {
    outputFrame = frame.image();
    emit imagePresented();
    return true;
  }
}