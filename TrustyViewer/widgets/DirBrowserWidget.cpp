
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

    connect(database.get(), &MediaDatabase::rebuildingDatabase, this, &DirBrowserWidget::disableTreeView);
    connect(database.get(), &MediaDatabase::databaseRebuild, model, &ImageFileSystemModel::reloadDatabase);
    connect(database.get(), &MediaDatabase::databaseRebuild, this, &DirBrowserWidget::enableTreeView);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::emitItemSelection);

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

    setLayout(layout);
  }

  void DirBrowserWidget::setSelectedItem(MediaItem::ptr_t item)
  {
    if (!item || item == getSelectedItem()) {
      return;
    }

    auto index = model->getIndexForItem(item);
    treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
  }

  void DirBrowserWidget::disableTreeView() {
    treeView->setEnabled(false);
  }

  void DirBrowserWidget::enableTreeView() {
    treeView->setEnabled(true);
  }

  void DirBrowserWidget::pickNewRoot() {
    auto rootDir = QFileDialog::getExistingDirectory(this, "Choose new root", QString(), QFileDialog::ShowDirsOnly);
    if (rootDir.isEmpty())
      return;

    setupRoot(rootDir);
  }

  MediaItem::ptr_t DirBrowserWidget::getSelectedItem() const
  {
    auto indices = treeView->selectionModel()->selectedIndexes();
    if (indices.empty())
      return nullptr;
    const auto& index = indices.first();
    return model->getItemForIndex(index);
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

  void DirBrowserWidget::emitItemSelection() {
    emit selectionChanged();
    emit selectedItemChanged(getSelectedItem());
  }
}