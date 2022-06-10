
#include <QDragMoveEvent>
#include <QMouseEvent>

#include "ListViewEx.h"

namespace realn {
  ListViewEx::ListViewEx(QWidget* parent)
    : QListView(parent)
  {
    //setMouseTracking(true);
  }

  QModelIndex ListViewEx::getPointedIndex() const {
    return indexAt(cursorPos);
  }

  void ListViewEx::dragMoveEvent(QDragMoveEvent* event) {
    cursorPos = event->pos();
    QListView::dragMoveEvent(event);
  }

  void ListViewEx::mouseMoveEvent(QMouseEvent* event) {
    cursorPos = event->pos();
    QListView::mouseMoveEvent(event);
  }
}
