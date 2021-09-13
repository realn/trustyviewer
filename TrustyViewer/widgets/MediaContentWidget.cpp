
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

    animatedContent = new AnimationMediaWidget();
    animatedContent->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    videoPlayer = std::make_unique<QMediaPlayer>();
    videoWidget = new QVideoWidget();
    videoPlayer->setVideoOutput(videoWidget);

    stack = new QStackedWidget();
    stack->addWidget(imageContent);
    stack->addWidget(animatedContent);
    stack->addWidget(videoWidget);

    stack->setCurrentIndex(0);

    auto layout = new QHBoxLayout();
    layout->addWidget(stack);
    setLayout(layout);
  }

  void MediaContentWidget::setMediaFromPath(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || fileInfo.isDir())
      return;

    auto fileext = fileInfo.completeSuffix();

    auto plugin = plugins->getPluginForExt(fileext);
    if (!plugin)
      return;

    animatedContent->clearMovie();
    videoPlayer->stop();
    if (plugin->isVideo(fileext)) {
      stack->setCurrentIndex(2);
      video = plugin->loadVideo(filePath);
      videoPlayer->setMedia(*video);
      videoPlayer->play();
    }
    else if (plugin->isAnimated(fileext)) {
      stack->setCurrentIndex(1);
      auto movie = plugin->loadAnimation(filePath);
      animatedContent->setMovie(std::move(movie));
    }
    else {
      stack->setCurrentIndex(0);
      auto image = plugin->loadImage(filePath);
      imageContent->setImage(std::move(image));
    }
  }

  void MediaContentWidget::setMediaFromItem(MediaItem::ptr_t item)
  {
    if (!item)
      return;
    if (item->isDirectory())
      return;

    setMediaFromPath(item->getFilePath());
  }
}