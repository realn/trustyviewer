#pragma once

#include <QPointer>
#include <QWidget>
#include <QTreeView>
#include <QLabel>

#include "TreeViewEx.h"
#include "../models/ImageFileSystemModel.h"
#include "../MediaDatabase.h"

class QAction;
class QContextMenuEvent;

namespace realn {
  class MediaItemStorage;

  class DirBrowserWidget : public QWidget {
    Q_OBJECT

  public:
    DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<MediaItemStorage> storage);

    MediaItem::ptr_t getSelectedItem() const;
    MediaItem::itemvector_t getSelectedItems() const;
    QFileInfo getSelectedFileInfo() const;

    void expandToItem(MediaItem::ptr_t item);

  signals:
    void rootChanged(QString newRoot);
    void selectionChanged();
    void selectedItemChanged(MediaItem::ptr_t item);
    void deleteItemRequested(MediaItem::ptr_t item);
    void moveItemRequested(MediaItem::ptr_t item);
    void newFolderItemRequested(MediaItem::ptr_t parent);
    void selectionCleared();

  public slots:
    void pickNewRoot();
    void setSelectedItem(MediaItem::ptr_t item);
    void setSelectedItemNoEmit(MediaItem::ptr_t item);
    void clearSelection();
    
  private slots:
    void emitItemSelection();
    void disableTreeView();
    void enableTreeView();
    void onDeleteItem();
    void onMoveItem();
    void onNewFolderItem();
    void onModelReset();

  private:
    void createUI();
    void createActions();
    void setupRoot(QString rootDir);
    void contextMenuEvent(QContextMenuEvent* event) override;
    void setSelectedItemPriv(MediaItem::ptr_t item, bool emitSignal);

    QPointer<QLabel> rootLabel;
    QPointer<TreeViewEx> treeView;
    QPointer<ImageFileSystemModel> model;
    QPointer<QAction> actionMove;
    QPointer<QAction> actionDelete;
    QPointer<QAction> actionNewFolder;
    std::shared_ptr<MediaDatabase> database;
  };
}