
#include <QDragMoveEvent>
#include <QMouseEvent>

#include "TreeViewEx.h"


namespace realn {
  TreeViewEx::TreeViewEx(QWidget* parent) 
    : QTreeView(parent)
  {
  }

  QModelIndex TreeViewEx::getPointedIndex() const {
    return indexAt(cursorPos);
  }

  void TreeViewEx::dragMoveEvent(QDragMoveEvent* event) {
    cursorPos = event->pos();
    QTreeView::dragMoveEvent(event);
  }

  void TreeViewEx::mouseMoveEvent(QMouseEvent* event) {
    cursorPos = event->pos();
    QTreeView::mouseMoveEvent(event);
  }


}
