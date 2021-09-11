#pragma once

#include <QWidget>
#include <QImage>

namespace realn {
  class ImageMediaWidget : public QWidget {
    Q_OBJECT;
  public:
    ImageMediaWidget();

    void setImage(std::unique_ptr<QImage> value);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    std::unique_ptr<QImage> image;
  };
}