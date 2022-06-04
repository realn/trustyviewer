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
    ThumbnailView(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<ThumbnailWorker> worker);

    MediaItem::ptr_t getSelectedItem() const;

    QPointer<ThumbnailModel> getThumbnailModel() const;

  signals:
    void selectedItemChanged(MediaItem::ptr_t item);
    void selectionCleared();

  public slots:
    void setSelectedItem(MediaItem::ptr_t item);
    void setSelectedItemNoEmit(MediaItem::ptr_t item);
    void clearSelection();
    void setRootByItem(MediaItem::ptr_t item);
    void refresh();

  private slots:
    void emitSelectionChanged();

  private:
    void setSelectedItemPriv(MediaItem::ptr_t item, bool emitSignal);

    QPointer<QListView> listView;
    QPointer<ThumbnailModel> model;
  };
}