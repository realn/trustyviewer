#pragma once

#include <QPointer>
#include <QWidget>
#include <QTreeView>
#include <QLabel>

#include "../models/ImageFileSystemModel.h"

namespace realn {
  class DirBrowserWidget : public QWidget {
    Q_OBJECT

  public:
    DirBrowserWidget();

    QFileInfo getSelectedFileInfo() const;

  signals:
    void selectionChanged();

  public slots:
    void pickNewRoot();

  private:
    void setupRoot(QString rootDir);

    QPointer<QLabel> rootLabel;
    QPointer<QTreeView> treeView;

    QPointer<ImageFileSystemModel> model;
  };
}