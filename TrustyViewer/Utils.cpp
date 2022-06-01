#include "Utils.h"

namespace realn {
  void qt_delete_later(QObject* ptr) {
    if (ptr->parent() != nullptr) {
      if (ptr)
        ptr->deleteLater();
    }
    else {
      delete ptr;
    }
  }
}