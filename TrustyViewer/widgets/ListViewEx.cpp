
#include <QDragMoveEvent>
#include <QMouseEvent>

#include "ListViewEx.h"

namespace realn {
  ListViewEx::ListViewEx(QWidget* parent)
    : QListView(parent)
  {
    setMouseTracking(true);
  }

  void ListViewEx::dragMoveEvent(QDragMoveEvent* event) {
    emit mousePosChanged(event->pos());
    QListView::dragMoveEvent(event);
  }

  void ListViewEx::mouseMoveEvent(QMouseEvent* event) {
    emit mousePosChanged(event->pos());
    QListView::mouseMoveEvent(event);
  }
}
