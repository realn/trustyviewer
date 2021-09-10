
#include <QSplitter>
#include <QTreeView>
#include <QLabel>
#include <QDockWidget>

#include "MainWindow.h"

namespace realn {
  MainWindow::MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase) {

    view = new ImageView(plugins);
    dirBrowser = new DirBrowserWidget(mediaDatabase);

    createUI();
  }

  void MainWindow::setImageToImageView() {
    auto info = dirBrowser->getSelectedFileInfo();
    if (!info.isFile())
      return;
    view->setImage(info);
  }

  void MainWindow::createUI()
  {
    auto dock = new QDockWidget("Explorer", this);
    dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(dirBrowser);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    setCentralWidget(view);

    connect(dirBrowser, &DirBrowserWidget::selectionChanged, this, &MainWindow::setImageToImageView);
  }
}