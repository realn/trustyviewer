#pragma once

#include <vector>

#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QFileInfo>
#include <QImage>
#include <QMovie>
#include <QStackedWidget>

#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"

#include "BaseMediaContentPlayerWidget.h"

namespace realn {
  class MediaContentWidget : public QWidget {
    Q_OBJECT;
  public:
    MediaContentWidget(std::shared_ptr<ExtPluginList> plugins);

    bool loadMedia(MediaItem::ptr_t item);

  private:
    using mpwidget_ptr_t = QPointer<BaseMediaContentPlayerWidget>;
    using mpwidget_vec_t = std::vector<mpwidget_ptr_t>;
    
    void addMediaWidget(mpwidget_ptr_t widget);
    bool loadMediaPriv(int mediaTypeId, MediaItem::ptr_t item, std::shared_ptr<ExtPlugin> plugin);
    static int getMediaTypeId(const QString& fileExt, std::shared_ptr<ExtPlugin> plugin);

    template<class _T, class = std::enable_if_t<std::is_base_of_v<BaseMediaContentPlayerWidget, _T>>>
    void addMediaWidget() { addMediaWidget(new _T()); }

    QPointer<QStackedWidget> stack;
    mpwidget_vec_t mediaWidgets;

    std::shared_ptr<ExtPluginList> plugins;
  };
}