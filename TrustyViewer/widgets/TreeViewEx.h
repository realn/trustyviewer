#pragma once

#include <QTreeView>
#include <QPoint>

#include "DragDropView.h"

namespace realn {
  class TreeViewEx : public QTreeView, public DragDropView {
    Q_OBJECT;
  public:
    explicit TreeViewEx(QWidget* parent = nullptr);

    QModelIndex getPointedIndex() const override;

  protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

  private:
    QPoint cursorPos;
  };
}

