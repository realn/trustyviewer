#pragma once

#include <QMainWindow>

namespace realn {
  class TestWindow : public QMainWindow {
    Q_OBJECT;
  public:
    TestWindow();
    ~TestWindow();
  };
}