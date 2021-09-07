
#include <QBoxLayout>
#include <QImage>
#include <QEvent>

#include "ImageView.h"

namespace realn {
  ImageView::ImageView(std::shared_ptr<ExtPluginList> plugins)
    : plugins(plugins)
  {

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

    auto filepath = fileInfo.absoluteFilePath();
    auto fileext = fileInfo.completeSuffix();

    auto plugin = plugins->getPluginForExt(fileext);
    if (!plugin)
      return;

    image = plugin->loadImage(filepath);
    shownImage = QPixmap::fromImage(*image);
    ResetImage();
  }

  void ImageView::resizeEvent(QResizeEvent* event)
  {
    ResetImage();
    QWidget::resizeEvent(event);
  }

  void ImageView::ResetImage()
  {
    label->setPixmap(shownImage.scaled(label->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
  }
}