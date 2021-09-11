
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

  MediaItem::ptr_t ThumbnailView::getSelectedItem() const
  {
    auto indices = listView->selectionModel()->selectedIndexes();
    if (indices.empty())
      return nullptr;
    const auto& index = indices.first();
    return model->fromIndex(index);
  }

  void ThumbnailView::setSelectedItem(MediaItem::ptr_t item) {
    if (item == getSelectedItem())
      return;

    auto index = model->getIndexForItem(item);
    listView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
  }

  void ThumbnailView::setRootByItem(MediaItem::ptr_t item) {
    if (!item)
      return;

    if (!item->hasParent() || item->isDirectory()) {
      model->setRootItem(item);
      return;
    }

    auto parent = item->getParent();
    model->setRootItem(parent);
  }

  void ThumbnailView::emitSelectionChanged() {
    auto item = getSelectedItem();
    emit selectedItemChanged(item);
  }
}