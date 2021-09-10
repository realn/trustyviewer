#pragma once

#include <QWidget>
#include <QListView>
#include <QPointer>

#include "../models/ThumbnailModel.h"
#include "../MediaItem.h"

namespace realn {
  class ThumbnailView : public QWidget {
    Q_OBJECT;
  public:
    ThumbnailView();

  public slots:
    void setSelectedItem(MediaItem::ptr_t item);

  private:
    QPointer<QListView> listView;
    QPointer<ThumbnailModel> model;
  };
}