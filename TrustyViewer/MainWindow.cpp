
#include <QSplitter>
#include <QTreeView>
#include <QLabel>

#include "Utils.h"
#include "MainWindow.h"

namespace realn {
  MainWindow::MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<ThumbnailWorker> worker) {

    view = new MediaContentWidget(plugins);
    dirBrowser = new DirBrowserWidget(mediaDatabase);
    thumbnailView = new ThumbnailView(plugins, worker);

    createUI();
  }

  void MainWindow::setMediaFromItem(MediaItem::ptr_t item) {
    if (!item)
      return;

    view->loadMedia(item);
    dirBrowser->setSelectedItem(item);
    thumbnailView->setSelectedItem(item);
  }

  void MainWindow::createUI()
  {
    addDock(dirBrowser, "Library Explorer", Qt::LeftDockWidgetArea, true);
    addDock(thumbnailView, "Thumbnails", Qt::RightDockWidgetArea, true);

    setCentralWidget(view);

    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, thumbnailView, &ThumbnailView::setRootByItem));
    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, this, &MainWindow::setMediaFromItem));
    cC(connect(thumbnailView, &ThumbnailView::selectedItemChanged, this, &MainWindow::setMediaFromItem));
  }

  void MainWindow::addDock(QWidget* widget, const QString& name, Qt::DockWidgetArea dockArea, bool visible)
  {
    auto dock = new QDockWidget(name, this);

    dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
    dock->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
    dock->setWidget(widget);
    dock->setVisible(visible);

    docks.push_back(dock);

    addDockWidget(dockArea, dock);
  }
}