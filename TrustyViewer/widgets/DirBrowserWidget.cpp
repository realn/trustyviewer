
#include <QAction>
#include <QBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTimer>

#include "DirBrowserWidget.h"
#include "../models/ImageFileSystemModel.h"
#include "../Windows/MoveWindow.h"

namespace realn {
  DirBrowserWidget::DirBrowserWidget(std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<MediaItemStorage> storage)
    : database(mediaDatabase) {
    setMinimumWidth(200);


    rootLabel = new QLabel();

    treeView = new TreeViewEx();
    treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
    treeView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
    treeView->setItemsExpandable(true);
    //treeView->setAnimated(true);
    treeView->setExpandsOnDoubleClick(false);
    treeView->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    treeView->setAcceptDrops(true);
    treeView->setDefaultDropAction(Qt::MoveAction);
    treeView->setDropIndicatorShown(true);

    model = new ImageFileSystemModel(database, storage, treeView);
    treeView->setModel(model);

    connect(database.get(), &MediaDatabase::rebuildingDatabase, this, &DirBrowserWidget::disableTreeView);
    connect(database.get(), &MediaDatabase::databaseRebuild, model, &ImageFileSystemModel::reloadDatabase);
    connect(database.get(), &MediaDatabase::databaseRebuild, this, &DirBrowserWidget::enableTreeView);
    connect(database.get(), &MediaDatabase::itemWillBeRemoved, model, &ImageFileSystemModel::removeItem);
    connect(database.get(), &MediaDatabase::itemWillBeMoved, model, &ImageFileSystemModel::moveItem);
    connect(database.get(), &MediaDatabase::itemWillBeAdded, model, &ImageFileSystemModel::addItem);

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirBrowserWidget::emitItemSelection);

    connect(model, &ImageFileSystemModel::modelReset, this, &DirBrowserWidget::onModelReset);

    createUI();
    createActions();
  }

  void DirBrowserWidget::setSelectedItem(MediaItem::ptr_t item) {
    setSelectedItemPriv(item, true);
  }

  void DirBrowserWidget::setSelectedItemNoEmit(MediaItem::ptr_t item) {
    setSelectedItemPriv(item, false);
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

  void DirBrowserWidget::onModelReset() {
    //auto root = database->getRootItem();
    //if (root)
    //  expandToItem(root);
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

  void DirBrowserWidget::expandToItem(MediaItem::ptr_t item) {
    if (!item)
      return;

    MediaItem::itemvector_t list;
    do {
      list.insert(list.begin(), item);
      item = item->getParent();
    } while (item);


    for (auto& item : list) {
      auto tv = treeView;
      auto tm = model;

      QTimer::singleShot(0, [tv, tm, item]() {
        if (tv && tm) {
          auto index = tm->getIndexForItem(item);
          tv->expand(index);
        }
                         });
    }
  }

  void DirBrowserWidget::createUI() {
    auto layout = new QVBoxLayout();

    {
      auto rootLayout = new QHBoxLayout();
      rootLayout->addWidget(rootLabel, 1);

      auto button = new QPushButton("Change");
      connect(button, &QPushButton::clicked, this, &DirBrowserWidget::pickNewRoot);

      rootLayout->addWidget(button);
      layout->addLayout(rootLayout);
    }

    layout->addWidget(treeView);

    setLayout(layout);
  }

  void DirBrowserWidget::createActions() {
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

  void DirBrowserWidget::setSelectedItemPriv(MediaItem::ptr_t item, bool emitSignal) {
    if (!item || item == getSelectedItem()) {
      return;
    }

    {
      auto index = model->getIndexForItem(item);
      treeView->setCurrentIndex(index);
    }

    expandToItem(item);

    if (emitSignal)
      emitItemSelection();
  }

  void DirBrowserWidget::emitItemSelection() {

    emit selectionChanged();
    emit selectedItemChanged(getSelectedItem());
  }
}