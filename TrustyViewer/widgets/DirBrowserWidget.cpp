
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileSystemModel>

#include "DirBrowserWidget.h"
#include "../models/ImageFileSystemModel.h"

namespace realn {
  DirBrowserWidget::DirBrowserWidget() {
    setMinimumWidth(200);

    rootLabel = new QLabel();
    treeView = new QTreeView();

    auto layout = new QVBoxLayout();

    {
      auto rootLayout = new QHBoxLayout();
      rootLayout->addWidget(rootLabel);

      auto button = new QPushButton("Change");
      connect(button, &QPushButton::clicked, this, &DirBrowserWidget::pickNewRoot);

      rootLayout->addWidget(button);
      layout->addLayout(rootLayout);
    }

    layout->addWidget(treeView);

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::selectionChanged);

    setLayout(layout);
  }

  void DirBrowserWidget::pickNewRoot() {
    auto rootDir = QFileDialog::getExistingDirectory(this, "Choose new root", QString(), QFileDialog::ShowDirsOnly);
    if (rootDir.isEmpty())
      return;

    setupRoot(rootDir);
  }

  QFileInfo DirBrowserWidget::getSelectedFileInfo() const
  {
    auto index = treeView->selectionModel()->selectedIndexes().first();
    return model->getFileInfoForIndex(index);
  }

  void DirBrowserWidget::setupRoot(QString rootDir)
  {
    rootLabel->setText(rootDir);

    model = new ImageFileSystemModel();
    model->setRootPath(rootDir);

    treeView->setModel(model);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::selectionChanged);
  }
}