
#include "MainWindow.h"

#include "Application.h"

namespace realn {
  Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
  {
    mainWindow = std::make_unique<MainWindow>();
    mainWindow->show();
  }

  Application::~Application() = default;
}