#pragma once

#include <QWidget>

#include "../MediaItem.h"
#include "../extensions/ExtPlugin.h"

namespace realn {
  class BaseMediaContentPlayerWidget : public QWidget {
    Q_OBJECT;
  public:
    BaseMediaContentPlayerWidget();

    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) = 0;
    virtual void clearMedia() = 0;
  };
}
