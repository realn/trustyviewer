#pragma once

#include <QWidget>
#include <QImage>

#include "BaseMediaContentPlayerWidget.h"

namespace realn {
  class ImageMediaWidget : public BaseMediaContentPlayerWidget {
    Q_OBJECT;
  public:
    ImageMediaWidget();

    // Inherited via BaseMediaContentPlayerWidget
    virtual bool loadMedia(MediaItem::ptr_t mediaItem, std::shared_ptr<ExtPlugin> plugin) override;
    virtual void clearMedia() override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    std::unique_ptr<QImage> image;
  };
}