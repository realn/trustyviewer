
#include <QSplitter>
#include <QTreeView>
#include <QLabel>

#include "Utils.h"
#include "MainWindow.h"

namespace realn {
  MainWindow::MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<ThumbnailWorker> worker) {

    view = new ImageView(plugins);
    dirBrowser = new DirBrowserWidget(mediaDatabase);
    thumbnailView = new ThumbnailView(plugins, worker);

    createUI();
  }

  void MainWindow::setImageFromItem(MediaItem::ptr_t item) {
    if (!item)
      return;

    QFileInfo info(item->getFilePath());
    if (info.isFile()) {
      view->setImage(info);
    }

    dirBrowser->setSelectedItem(item);
    thumbnailView->setSelectedItem(item);
  }

  void MainWindow::setImageToImageView() {
    auto info = dirBrowser->getSelectedFileInfo();
    if (!info.isFile())
      return;
    view->setImage(info);
  }

  void MainWindow::createUI()
  {
    addDock(dirBrowser, "Library Explorer", Qt::LeftDockWidgetArea, true);
    addDock(thumbnailView, "Thumbnails", Qt::RightDockWidgetArea, true);

    setCentralWidget(view);

    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, thumbnailView, &ThumbnailView::setRootByItem));
    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, this, &MainWindow::setImageFromItem));
    cC(connect(thumbnailView, &ThumbnailView::selectedItemChanged, this, &MainWindow::setImageFromItem));
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