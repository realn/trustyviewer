
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QBoxLayout>
#include <QMouseEvent>

#include "ThumbnailView.h"
#include "../Utils.h"

namespace realn {
  ThumbnailView::ThumbnailView(std::shared_ptr<MediaDatabase> database, std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<ThumbnailWorker> worker, std::shared_ptr<MediaItemStorage> storage) {

    auto thumbSize = QSize(100, 150);
    auto gridSize = thumbSize + QSize(20, 20);


    debugEdit = new QLineEdit();

    listView = new ListViewEx();
    listView->setViewMode(QListView::ViewMode::IconMode);
    listView->setFlow(QListView::Flow::LeftToRight);
    listView->setResizeMode(QListView::ResizeMode::Adjust);
    listView->setGridSize(gridSize);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setDragEnabled(true);
    listView->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    listView->setAcceptDrops(true);
    listView->setDropIndicatorShown(true);
    listView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    model = new ThumbnailModel(database, plugins, worker, storage, listView, thumbSize);
    listView->setModel(model);

    cC(connect(listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThumbnailView::emitSelectionChanged));
    cC(connect(listView, &QListView::customContextMenuRequested, this, &ThumbnailView::showContextMenu));
    cC(connect(model, &ThumbnailModel::moveItemRequested, this, &ThumbnailView::moveItemRequested));

    auto layout = new QVBoxLayout();
    layout->addWidget(debugEdit);
    layout->addWidget(listView);
    setLayout(layout);

    createActions();

    //listView->setMouseTracking(true);
    setMouseTracking(true);
  }

  ThumbnailView::~ThumbnailView() = default;

  MediaItem::ptr_t ThumbnailView::getSelectedItem() const {
    auto indices = listView->selectionModel()->selectedIndexes();
    if (indices.empty())
      return nullptr;
    const auto& index = indices.first();
    return model->getMediaItemForIndex(index);
  }

  QPointer<ThumbnailModel> ThumbnailView::getThumbnailModel() const {
    return model;
  }

  void ThumbnailView::setSelectedItem(MediaItem::ptr_t item) {
    setSelectedItemPriv(item, true);
  }

  void ThumbnailView::setSelectedItemNoEmit(MediaItem::ptr_t item) {
    setSelectedItemPriv(item, false);
  }

  void ThumbnailView::clearSelection() {
    listView->selectionModel()->clearSelection();
    emit selectionCleared();
  }

  void ThumbnailView::setRootByItem(MediaItem::ptr_t item) {
    if (!item) {
      return;
    }

    if (!item->hasParent() || item->isDirectory()) {
      model->setRootItem(item);
      return;
    }

    auto parent = item->getParent();
    model->setRootItem(parent);

    clearSelection();
  }

  void ThumbnailView::refresh() {
    model->refreshModel();
  }

  void ThumbnailView::onMoveItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    emit moveItemRequested(item);
  }

  void ThumbnailView::onDeleteItem() {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    emit deleteItemRequested(item);
  }

  void ThumbnailView::showContextMenu(const QPoint& pos) {
    auto item = getSelectedItem();
    if (item == nullptr)
      return;

    actionMove->setEnabled(true);
    actionDelete->setEnabled(true);

    QMenu menu(this);
    menu.addAction(actionMove);
    menu.addAction(actionDelete);

    menu.exec(listView->mapToGlobal(pos));
  }

  void ThumbnailView::setSelectedItemPriv(MediaItem::ptr_t item, bool emitSignal) {
    if (item == getSelectedItem())
      return;
    if (item == model->getRootItem())
      return;
    if (!item)
      return;

    if (item->isDirectory()) {
      model->setRootItem(item);
    }
    else {
      if (item->getParent() != model->getRootItem()) {
        model->setRootItem(item->getParent());
      }

      auto index = model->getIndexForItem(item);
      listView->setCurrentIndex(index);
    }

    if (emitSignal)
      emitSelectionChanged();
  }

  void ThumbnailView::createActions() {
    actionMove = new QAction("Move");
    cC(connect(actionMove, &QAction::triggered, this, &ThumbnailView::onMoveItem));

    actionDelete = new QAction("Delete");
    cC(connect(actionDelete, &QAction::triggered, this, &ThumbnailView::onDeleteItem));
  }

  void ThumbnailView::emitSelectionChanged() {
    auto item = getSelectedItem();
    emit selectedItemChanged(item);
  }
}