#pragma once

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QMovie>

#include "BaseMediaContentPlayerWidget.h"

namespace realn {
  class AnimationMediaWidget : public BaseMediaContentPlayerWidget {
    Q_OBJECT;
  public:
    AnimationMediaWidget();

    // Inherited via BaseMediaContentPlayerWidget
    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) override;
    virtual void clearMedia() override;

  private:
    QPointer<QLabel> animationPlayer;

    std::unique_ptr<QMovie> movie;
  };
}