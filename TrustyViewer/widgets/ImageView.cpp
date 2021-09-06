
#include <QBoxLayout>
#include <QImage>
#include <QEvent>

#include "ImageView.h"

namespace realn {
  ImageView::ImageView() {

    setMinimumSize(640, 480);

    auto layout = new QHBoxLayout();

    label = new QLabel();
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setScaledContents(false);
    label->setBackgroundRole(QPalette::Base);
    label->setAlignment(Qt::AlignmentFlag::AlignCenter);

    layout->addWidget(label);

    setLayout(layout);
  }
  void ImageView::setImage(QFileInfo fileInfo)
  {
    if (fileInfo.isDir())
      return;

    image = QPixmap(fileInfo.absoluteFilePath());
    ResetImage();
  }

  void ImageView::resizeEvent(QResizeEvent* event)
  {
    ResetImage();
    QWidget::resizeEvent(event);
  }

  void ImageView::ResetImage()
  {
    label->setPixmap(image.scaled(label->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
  }
}