
#include <QAction>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>

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
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
    treeView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    connect(database.get(), &MediaDatabase::rebuildingDatabase, this, &DirBrowserWidget::disableTreeView);
    connect(database.get(), &MediaDatabase::databaseRebuild, model, &ImageFileSystemModel::reloadDatabase);
    connect(database.get(), &MediaDatabase::databaseRebuild, this, &DirBrowserWidget::enableTreeView);
    connect(database.get(), &MediaDatabase::itemWillBeRemoved, model, &ImageFileSystemModel::beginRemoveItem);
    connect(database.get(), &MediaDatabase::itemRemoved, model, &ImageFileSystemModel::endRemoveItem);

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::setupNewSelection);

    createUI();
    createTreeViewActions();
  }

  void DirBrowserWidget::setSelectedItem(MediaItem::ptr_t item)
  {
    if (!item || item == getSelectedItem()) {
      return;
    }

    auto index = model->getIndexForItem(item);
    treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
  }

  void DirBrowserWidget::clearSelection() {
    treeView->selectionModel()->clearSelection();
    emit selectionCleared();
  }

  void DirBrowserWidget::disableTreeView() {
    treeView->setEnabled(false);
  }

  void DirBrowserWidget::enableTreeView() {
    treeView->setEnabled(true);
  }

  void DirBrowserWidget::tryDeleteItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    auto text = QString("Delete item %1?").arg(item->getFilePath());
    auto result = QMessageBox::question(this, "Delete item?", text, QMessageBox::Yes, QMessageBox::No);
    if (result == QMessageBox::Yes) {
      clearSelection();

      emit deleteItemRequested(item);
      database->deleteItem(item);
    }
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

  MediaItem::itemvector_t DirBrowserWidget::getSelectedItems() const {
    auto indices = treeView->selectionModel()->selectedIndexes();
    if (indices.empty())
      return MediaItem::itemvector_t();

    return model->getItemsForIndices(indices);
  }

  QFileInfo DirBrowserWidget::getSelectedFileInfo() const
  {
    auto index = treeView->selectionModel()->selectedIndexes().first();
    return QFileInfo(model->getItemForIndex(index)->getFilePath());
  }

  void DirBrowserWidget::createUI() {
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

  void DirBrowserWidget::createTreeViewActions() {
    actionMove = new QAction("Move");

    actionDelete = new QAction("Delete");
    connect(actionDelete, &QAction::triggered, this, &DirBrowserWidget::tryDeleteItem);
    
    treeView->addAction(actionMove);
    treeView->addAction(actionDelete);
  }

  void DirBrowserWidget::setupRoot(QString rootDir)
  {
    rootLabel->setText(rootDir);

    database->rebuid(rootDir);

    emit rootChanged(rootDir);
  }

  void DirBrowserWidget::setupNewSelection() {
    auto item = getSelectedItem();
    
    auto enabled = item != nullptr;

    actionMove->setEnabled(enabled);
    actionDelete->setEnabled(enabled);

    emitItemSelection();
  }

  void DirBrowserWidget::emitItemSelection() {
    emit selectionChanged();
    emit selectedItemChanged(getSelectedItem());
  }
}