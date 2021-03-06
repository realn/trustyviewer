#pragma once

#include <QListView>
#include <QPoint>

#include "DragDropView.h"

namespace realn {
  class ListViewEx : public QListView, public DragDropView {
    Q_OBJECT;
  public:
    explicit ListViewEx(QWidget* parent = nullptr);

    QModelIndex getPointedIndex() const override;

  protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    QPoint cursorPos;
  };
}