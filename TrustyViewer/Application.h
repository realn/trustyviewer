#pragma once

#include <memory>

#include <QApplication>

namespace realn {
  class MainWindow;

  class Application : public QApplication {
    Q_OBJECT
  public:
    Application(int& argc, char** argv);
    ~Application();

  private:
    std::unique_ptr<MainWindow> mainWindow;
  };
}