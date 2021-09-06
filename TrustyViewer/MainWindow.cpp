
#include <QSplitter>
#include <QTreeView>
#include <QLabel>
#include <QDockWidget>

#include "MainWindow.h"

namespace realn {
  MainWindow::MainWindow() {

    auto widget = createUI();
    setCentralWidget(widget);
    
  }

  void MainWindow::setImageToImageView() {
    auto info = dirBrowser->getSelectedFileInfo();
    if (!info.isFile())
      return;
    view->setImage(info);
  }

  QWidget* MainWindow::createUI()
  {
    view = new ImageView();
    dirBrowser = new DirBrowserWidget();

    auto dock = new QDockWidget("Library", this);
    dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(dirBrowser);

    addDockWidget(Qt::LeftDockWidgetArea, dock);

    connect(dirBrowser, &DirBrowserWidget::selectionChanged, this, &MainWindow::setImageToImageView);

    return view;
  }
}