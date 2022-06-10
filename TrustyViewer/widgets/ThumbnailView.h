#pragma once

#include <QWidget>
#include <QListView>
#include <QPointer>

#include "../widgets/ListViewEx.h"
#include "../models/ThumbnailModel.h"
#include "../extensions/ExtPlugin.h"
#include "../MediaItem.h"

class QAction;

namespace realn {
  class MediaItemStorage;

  class ThumbnailView : public QWidget {
    Q_OBJECT;
  public:
    ThumbnailView(std::shared_ptr<MediaDatabase> database, std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<ThumbnailWorker> worker, std::shared_ptr<MediaItemStorage> storage);
    ~ThumbnailView();

    MediaItem::ptr_t getSelectedItem() const;

    QPointer<ThumbnailModel> getThumbnailModel() const;

  signals:
    void selectedItemChanged(MediaItem::ptr_t item);
    void selectionCleared();
    void deleteItemRequested(MediaItem::ptr_t item);
    void moveItemRequested(MediaItem::ptr_t item);

  public slots:
    void setSelectedItem(MediaItem::ptr_t item);
    void setSelectedItemNoEmit(MediaItem::ptr_t item);
    void clearSelection();
    void setRootByItem(MediaItem::ptr_t item);
    void refresh();

  private slots:
    void emitSelectionChanged();
    void onMoveItem();
    void onDeleteItem();
    void showContextMenu(const QPoint& pos);

  private:
    void setSelectedItemPriv(MediaItem::ptr_t item, bool emitSignal);
    void createActions();

    QPointer<ListViewEx> listView;
    QPointer<ThumbnailModel> model;
    QPointer<QAction> actionMove;
    QPointer<QAction> actionDelete;
    QPointer<QLineEdit> debugEdit;
    QPoint mouseMove;
  };
}