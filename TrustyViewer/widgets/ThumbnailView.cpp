
#include <QBoxLayout>

#include "ThumbnailView.h"

namespace realn {
  ThumbnailView::ThumbnailView(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<ThumbnailWorker> worker) {

    auto thumbSize = QSize(100, 150);
    auto gridSize = thumbSize + QSize(20, 20);

    model = new ThumbnailModel(plugins, worker, thumbSize);

    listView = new QListView();
    listView->setViewMode(QListView::ViewMode::IconMode);
    listView->setModel(model);
    listView->setFlow(QListView::Flow::LeftToRight);
    listView->setResizeMode(QListView::ResizeMode::Adjust);
    listView->setGridSize(gridSize);
    connect(listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThumbnailView::emitSelectionChanged);

    auto layout = new QHBoxLayout();
    layout->addWidget(listView);
    setLayout(layout);
  }

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

  void ThumbnailView::emitSelectionChanged() {
    auto item = getSelectedItem();
    emit selectedItemChanged(item);
  }
}