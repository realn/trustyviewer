
#include <QBoxLayout>

#include "ThumbnailView.h"

namespace realn {
  ThumbnailView::ThumbnailView(std::shared_ptr<ExtPluginList> plugins) {

    auto thumbSize = QSize(100, 150);
    auto gridSize = thumbSize + QSize(20, 20);

    model = new ThumbnailModel(plugins, thumbSize);

    listView = new QListView();
    listView->setViewMode(QListView::ViewMode::IconMode);
    listView->setModel(model);
    listView->setFlow(QListView::Flow::LeftToRight);
    listView->setResizeMode(QListView::ResizeMode::Adjust);
    listView->setGridSize(gridSize);

    auto layout = new QHBoxLayout();
    layout->addWidget(listView);
    setLayout(layout);
  }

  void ThumbnailView::setSelectedItem(MediaItem::ptr_t item) {
    if (!item->hasParent()) {
      model->setRootItem(item);
      return;
    }

    auto parent = item->getParent();
    model->setRootItem(parent);
  }
}