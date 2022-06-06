#pragma once

#include <memory>

#include <QApplication>

#include "extensions/ExtPlugin.h"
#include "workers/ThumbnailWorker.h"
#include "workers/MediaItemWorker.h"

#include "MediaDatabase.h"

namespace realn {
  class MainWindow;
  class TestWindow;

  class Application : public QApplication {
    Q_OBJECT
  public:
    Application(int& argc, char** argv);
    ~Application();

  private:
    std::shared_ptr<MediaDatabase> database;
    std::shared_ptr<ExtPluginList> plugins;

    std::shared_ptr<ThumbnailWorker> thumbnailWorker;
    std::shared_ptr<MediaItemWorker> mediaItemWorker;

    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<TestWindow> testWindow;
  };
}