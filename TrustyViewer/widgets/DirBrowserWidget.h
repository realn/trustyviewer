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

    QFileInfo getSelectedFileInfo() const;

  signals:
    void rootChanged(QString newRoot);
    void selectionChanged();

  public slots:
    void pickNewRoot();

  private:
    void setupRoot(QString rootDir);

    QPointer<QLabel> rootLabel;
    QPointer<QTreeView> treeView;

    QPointer<ImageFileSystemModel> model;
    std::shared_ptr<MediaDatabase> database;
  };
}