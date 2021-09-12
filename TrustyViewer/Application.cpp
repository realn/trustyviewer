
#include "MainWindow.h"

#include "extensions/StdImageExtPlugin.h"
#include "extensions/GifImageExtPlugin.h"

#include "Application.h"

namespace realn {
  Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
  {

    plugins = std::make_shared<ExtPluginList>();
    plugins->addPlugin(std::make_shared<StdImageExtPlugin>());
    plugins->addPlugin(std::make_shared<GifImageExtPlugin>());

    database = std::make_shared<MediaDatabase>(plugins);

    thumbnailWorker = std::make_shared<ThumbnailWorker>(plugins);

    mainWindow = std::make_unique<MainWindow>(plugins, database, thumbnailWorker);
    mainWindow->show();
  }

  Application::~Application() = default;
}