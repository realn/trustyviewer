#pragma once

#include <QPointer>
#include <QWidget>
#include <QTreeView>
#include <QLabel>

#include "../models/ImageFileSystemModel.h"
#include "../MediaDatabase.h"

class QAction;

namespace realn {
  class DirBrowserWidget : public QWidget {
    Q_OBJECT

  public:
    DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase);

    MediaItem::ptr_t getSelectedItem() const;
    MediaItem::itemvector_t getSelectedItems() const;
    QFileInfo getSelectedFileInfo() const;

  signals:
    void rootChanged(QString newRoot);
    void selectionChanged();
    void selectedItemChanged(MediaItem::ptr_t item);
    void deleteItemRequested(MediaItem::ptr_t item);
    void selectionCleared();

  public slots:
    void pickNewRoot();
    void setSelectedItem(MediaItem::ptr_t item);
    void clearSelection();
    
  private slots:
    void setupNewSelection();
    void emitItemSelection();
    void disableTreeView();
    void enableTreeView();
    void tryDeleteItem();

  private:
    void createUI();
    void createTreeViewActions();
    void setupRoot(QString rootDir);

    QPointer<QLabel> rootLabel;
    QPointer<QTreeView> treeView;
    QPointer<ImageFileSystemModel> model;
    QPointer<QAction> actionMove;
    QPointer<QAction> actionDelete;
    std::shared_ptr<MediaDatabase> database;
  };
}