
#include <QAction>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>

#include "DirBrowserWidget.h"
#include "../models/ImageFileSystemModel.h"
#include "../Windows/MoveWindow.h"

namespace realn {
  DirBrowserWidget::DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase)
    : database(mediaDatabase) {
    setMinimumWidth(200);

    model = new ImageFileSystemModel(database);

    rootLabel = new QLabel();

    treeView = new QTreeView();
    treeView->setModel(model);
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
    treeView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    connect(database.get(), &MediaDatabase::rebuildingDatabase, this, &DirBrowserWidget::disableTreeView);
    connect(database.get(), &MediaDatabase::databaseRebuild, model, &ImageFileSystemModel::reloadDatabase);
    connect(database.get(), &MediaDatabase::databaseRebuild, this, &DirBrowserWidget::enableTreeView);
    connect(database.get(), &MediaDatabase::itemWillBeRemoved, model, &ImageFileSystemModel::beginRemoveItem);
    connect(database.get(), &MediaDatabase::itemRemoved, model, &ImageFileSystemModel::endRemoveItem);
    connect(database.get(), &MediaDatabase::itemWillBeMoved, model, &ImageFileSystemModel::beginMoveItem);
    connect(database.get(), &MediaDatabase::itemMoved, model, &ImageFileSystemModel::endMoveItem);
    connect(database.get(), &MediaDatabase::itemWillBeAdded, model, &ImageFileSystemModel::beginAddItem);
    connect(database.get(), &MediaDatabase::itemAdded, model, &ImageFileSystemModel::endAddItem);

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::setupNewSelection);

    createUI();
    createTreeViewActions();
  }

  void DirBrowserWidget::setSelectedItem(MediaItem::ptr_t item) {
    if (!item || item == getSelectedItem()) {
      return;
    }

    setUpdatesEnabled(false);
    auto index = model->getIndexForItem(item);
    treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    setUpdatesEnabled(true);
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

  void DirBrowserWidget::onDeleteItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    emit deleteItemRequested(item);
  }

  void DirBrowserWidget::onMoveItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    emit moveItemRequested(item);
  }

  void DirBrowserWidget::onNewFolderItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;
    if (!item->isDirectory())
      return;

    emit newFolderItemRequested(item);
  }

  void DirBrowserWidget::pickNewRoot() {
    auto rootDir = QFileDialog::getExistingDirectory(this, "Choose new root", QString(), QFileDialog::ShowDirsOnly);
    if (rootDir.isEmpty())
      return;

    setupRoot(rootDir);
  }

  MediaItem::ptr_t DirBrowserWidget::getSelectedItem() const {
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

  QFileInfo DirBrowserWidget::getSelectedFileInfo() const {
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
    connect(actionMove, &QAction::triggered, this, &DirBrowserWidget::onMoveItem);

    actionDelete = new QAction("Delete");
    connect(actionDelete, &QAction::triggered, this, &DirBrowserWidget::onDeleteItem);

    actionNewFolder = new QAction("New folder");
    connect(actionNewFolder, &QAction::triggered, this, &DirBrowserWidget::onNewFolderItem);
  }

  void DirBrowserWidget::setupRoot(QString rootDir) {
    rootLabel->setText(rootDir);

    database->rebuid(rootDir);

    emit rootChanged(rootDir);
  }

  void DirBrowserWidget::contextMenuEvent(QContextMenuEvent* event) {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    actionMove->setEnabled(true);
    actionDelete->setEnabled(true);

    QMenu menu(this);
    if (item->isDirectory())
      menu.addAction(actionNewFolder);
    menu.addAction(actionMove);
    menu.addAction(actionDelete);

    menu.exec(event->globalPos());
  }

  void DirBrowserWidget::setupNewSelection() {
    emitItemSelection();
  }

  void DirBrowserWidget::emitItemSelection() {
    emit selectionChanged();
    emit selectedItemChanged(getSelectedItem());
  }
}