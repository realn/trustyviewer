#pragma once

#include <QtMultimedia/QAbstractVideoSurface>

namespace realn {
  class DummyVideoSurface : public QAbstractVideoSurface {
    Q_OBJECT;
  public:
    // Inherited via QAbstractVideoSurface
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle) const override;

    bool present(const QVideoFrame& frame) override;

    QImage outputFrame;

  signals:
    void imagePresented();
  };

}