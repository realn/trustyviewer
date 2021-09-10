
#include "MainWindow.h"

#include "Extensions/StdImageExtPlugin.h"

#include "Application.h"

namespace realn {
  Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
  {

    plugins = std::make_shared<ExtPluginList>();
    plugins->addPlugin(std::make_shared<StdImageExtPlugin>());

    database = std::make_shared<MediaDatabase>(plugins);

    mainWindow = std::make_unique<MainWindow>(plugins, database);
    mainWindow->show();
  }

  Application::~Application() = default;
}