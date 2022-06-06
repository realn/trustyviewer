#pragma once

#include <QListView>

namespace realn {
  class ListViewEx : public QListView {
    Q_OBJECT;
  public:
    explicit ListViewEx(QWidget* parent = nullptr);

  signals:
    void mousePosChanged(const QPoint& pos);

  protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
  };
}