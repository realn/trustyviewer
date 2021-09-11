#pragma once

#include <memory>

#include <QApplication>

#include "extensions/ExtPlugin.h"
#include "workers/ThumbnailWorker.h"
#include "MediaDatabase.h"

namespace realn {
  class MainWindow;

  class Application : public QApplication {
    Q_OBJECT
  public:
    Application(int& argc, char** argv);
    ~Application();

  private:
    std::shared_ptr<MediaDatabase> database;
    std::shared_ptr<ExtPluginList> plugins;
    std::shared_ptr<ThumbnailWorker> thumbnailWorker;

    std::unique_ptr<MainWindow> mainWindow;
  };
}