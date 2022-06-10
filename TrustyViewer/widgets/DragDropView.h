#pragma once

#include <QModelIndex>

namespace realn {
  class DragDropView {
  public:
    virtual QModelIndex getPointedIndex() const = 0;
  };
}