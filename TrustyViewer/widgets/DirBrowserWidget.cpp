
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileSystemModel>

#include "DirBrowserWidget.h"
#include "../models/ImageFileSystemModel.h"

namespace realn {
  DirBrowserWidget::DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase) 
    : database(mediaDatabase)
  {
    setMinimumWidth(200);

    model = new ImageFileSystemModel(database);

    rootLabel = new QLabel();

    treeView = new QTreeView();
    treeView->setModel(model);

    connect(database.get(), &MediaDatabase::databaseRebuild, model, &ImageFileSystemModel::reloadDatabase);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::selectionChanged);

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
    return QFileInfo(model->getItemForIndex(index)->getFilePath());
  }

  void DirBrowserWidget::setupRoot(QString rootDir)
  {
    rootLabel->setText(rootDir);

    database->rebuid(rootDir);

    emit rootChanged(rootDir);
  }
}