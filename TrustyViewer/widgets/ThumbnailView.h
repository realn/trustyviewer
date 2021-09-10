#pragma once

#include <QWidget>
#include <QListView>
#include <QPointer>

#include "../models/ThumbnailModel.h"
#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"

namespace realn {
  class ThumbnailView : public QWidget {
    Q_OBJECT;
  public:
    ThumbnailView(std::shared_ptr<ExtPluginList> plugins);

    MediaItem::ptr_t getSelectedItem() const;

  signals:
    void selectedItemChanged(MediaItem::ptr_t item);

  public slots:
    void setSelectedItem(MediaItem::ptr_t item);
    void setRootByItem(MediaItem::ptr_t item);

  private slots:
    void emitSelectionChanged();

  private:
    QPointer<QListView> listView;
    QPointer<ThumbnailModel> model;
  };
}