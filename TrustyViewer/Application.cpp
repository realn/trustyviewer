
#include "MainWindow.h"

#include "extensions/StdImageExtPlugin.h"
#include "extensions/GifImageExtPlugin.h"
#include "extensions/StdVideoExtPlugin.h"

#include "Application.h"

namespace realn {
  Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
  {

    plugins = std::make_shared<ExtPluginList>();
    plugins->addPlugin<StdImageExtPlugin>();
    plugins->addPlugin<GifImageExtPlugin>();
    //plugins->addPlugin<StdVideoExtPlugin>();

    database = std::make_shared<MediaDatabase>(plugins);

    thumbnailWorker = std::make_shared<ThumbnailWorker>(plugins);

    mainWindow = std::make_unique<MainWindow>(plugins, database, thumbnailWorker);
    mainWindow->show();
  }

  Application::~Application() = default;
}