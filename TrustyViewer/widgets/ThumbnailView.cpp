
#include <QBoxLayout>

#include "ThumbnailView.h"

namespace realn {
  ThumbnailView::ThumbnailView() {

    model = new ThumbnailModel();

    listView = new QListView();
    listView->setViewMode(QListView::ViewMode::IconMode);
    listView->setModel(model);

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