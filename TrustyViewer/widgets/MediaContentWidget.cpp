
#include <QBoxLayout>
#include <QImage>
#include <QEvent>

#include "MediaContentWidget.h"

namespace realn {
  namespace {
    bool lequal(const QSize& left, const QSize& right) {
      if (left.width() <= right.width() && left.height() <= right.height())
        return true;
      return false;
    }
  }

  MediaContentWidget::MediaContentWidget(std::shared_ptr<ExtPluginList> plugins)
    : plugins(plugins)
  {

    setMinimumSize(640, 480);

    imageContent = new ImageMediaWidget();
    imageContent->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    animationPlayer = new QLabel();
    animationPlayer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    animationPlayer->setAlignment(Qt::AlignCenter);

    stack = new QStackedWidget();
    stack->addWidget(imageContent);
    stack->addWidget(animationPlayer);

    stack->setCurrentIndex(0);

    auto layout = new QHBoxLayout();
    layout->addWidget(stack);
    setLayout(layout);
  }

  void MediaContentWidget::setImageFromPath(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || fileInfo.isDir())
      return;

    auto fileext = fileInfo.completeSuffix();

    auto plugin = plugins->getPluginForExt(fileext);
    if (!plugin)
      return;

    if (plugin->isAnimated(fileext)) {
      stack->setCurrentIndex(1);
      animationPlayer->setMovie(nullptr);
      movie = plugin->loadMovie(filePath);
      animationPlayer->setMovie(movie.get());
      movie->start();
    }
    else {
      stack->setCurrentIndex(0);
      auto image = plugin->loadImage(filePath);
      imageContent->setImage(std::move(image));
    }
  }

  void MediaContentWidget::setImageFromItem(MediaItem::ptr_t item)
  {
    if (!item)
      return;
    if (item->isDirectory())
      return;

    setImageFromPath(item->getFilePath());
  }

  void MediaContentWidget::resizeEvent(QResizeEvent* event)
  {
    rescaleImage();
    QWidget::resizeEvent(event);
  }

  void MediaContentWidget::loadScaledImage()
  {
    //label->setPixmap(shownImage.scaled(label->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
  }

  void MediaContentWidget::rescaleImage()
  {
    //const auto& pix = label->pixmap();
    //if (pix && lequal(pix->size(), label->size()))
    //  return;
    //loadScaledImage();
  }
}