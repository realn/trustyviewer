#pragma once

#include <QPointer>
#include <QWidget>
#include <QTreeView>
#include <QLabel>

#include "../models/ImageFileSystemModel.h"
#include "../MediaDatabase.h"

namespace realn {
  class DirBrowserWidget : public QWidget {
    Q_OBJECT

  public:
    DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase);

    MediaItem::ptr_t getSelectedItem() const;
    QFileInfo getSelectedFileInfo() const;

  signals:
    void rootChanged(QString newRoot);
    void selectionChanged();
    void selectedItemChanged(MediaItem::ptr_t item);

  public slots:
    void pickNewRoot();
    void setSelectedItem(MediaItem::ptr_t item);
    
  private slots:
    void emitItemSelection();
    void disableTreeView();
    void enableTreeView();

  private:
    void setupRoot(QString rootDir);

    QPointer<QLabel> rootLabel;
    QPointer<QTreeView> treeView;

    QPointer<ImageFileSystemModel> model;
    std::shared_ptr<MediaDatabase> database;
  };
}