
#include <QFileInfo>
#include <QBoxLayout>
#include <QImage>
#include <QEvent>

#include "ImageMediaWidget.h"
#include "AnimationMediaWidget.h"
#include "VideoMediaWidget.h"

#include "MediaContentWidget.h"

namespace realn {
  const int MEDIA_IMAGE = 0;
  const int MEDIA_ANIM = 1;
  const int MEDIA_VIDEO = 2;

  MediaContentWidget::MediaContentWidget(std::shared_ptr<ExtPluginList> plugins)
    : plugins(plugins)
  {
    setMinimumSize(640, 480);
    stack = new QStackedWidget();

    addMediaWidget<ImageMediaWidget>();
    addMediaWidget<AnimationMediaWidget>();
    addMediaWidget<VideoMediaWidget>();

    auto layout = new QHBoxLayout();
    layout->addWidget(stack);
    setLayout(layout);
  }

  bool MediaContentWidget::loadMedia(const MediaItem::ptr_t item) {
    if (loadedItem.lock() == item)
      return true;

    if (!item)
      return false;

    if (item->isDirectory())
      return false;

    QFileInfo fileInfo(item->getFilePath());
    if (!fileInfo.exists() || fileInfo.isDir())
      return false;

    auto fileext = fileInfo.completeSuffix();

    auto plugin = plugins->getPluginForExt(fileext);
    if (!plugin)
      return false;

    for (auto& widget : mediaWidgets)
      widget->clearMedia();

    auto typeId = getMediaTypeId(fileext, plugin);

    if (!loadMediaPriv(typeId, item, plugin))
      return false;

    loadedItem = item;
    return true;
  }

  void MediaContentWidget::addMediaWidget(mpwidget_ptr_t widget)
  {
    widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    stack->addWidget(widget);
    mediaWidgets.push_back(widget);
  }

  bool MediaContentWidget::loadMediaPriv(int mediaTypeId, MediaItem::ptr_t item, std::shared_ptr<ExtPlugin> plugin)
  {
    stack->setCurrentIndex(mediaTypeId);
    return mediaWidgets[mediaTypeId]->loadMedia(item, plugin);
  }

  int MediaContentWidget::getMediaTypeId(const QString& fileExt, std::shared_ptr<ExtPlugin> plugin)
  {
    if (plugin->isVideo(fileExt))
      return MEDIA_VIDEO;
    if (plugin->isAnimated(fileExt))
      return MEDIA_ANIM;
    return MEDIA_IMAGE;
  }
}