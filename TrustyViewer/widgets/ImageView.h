#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>

namespace realn {
  class ImageView : public QWidget {
    Q_OBJECT;
  public:
    ImageView();

    void setImage(QFileInfo fileInfo);

  protected:
    void resizeEvent(QResizeEvent* event) override;

  private:
    void ResetImage();

    QPointer<QLabel> label;
    QPixmap image;
  };
}