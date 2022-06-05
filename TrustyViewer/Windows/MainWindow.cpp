
#include <QSplitter>
#include <QTreeView>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>

#include "../AppSettings.h"
#include "../Utils.h"
#include "MoveWindow.h"
#include "TextPromptDialog.h"
#include "MainWindow.h"

namespace realn {
  MainWindow::MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<ThumbnailWorker> worker)
    : database(mediaDatabase) {
    view = new MediaContentWidget(plugins);
    dirBrowser = new DirBrowserWidget(mediaDatabase);
    thumbnailView = new ThumbnailView(plugins, worker);

    createUI();

    AppSettings settings;
    settings.readMainWindow("mainwindow", this);
  }

  MainWindow::~MainWindow() = default;

  void MainWindow::clearMedia() {
    view->clearMedia();
  }

  void MainWindow::showDatabaseRebuildProgress(int done, int total) {
    auto msg = QString("Scanning items: %1/%2").arg(done).arg(total);
    statusBar->showMessage(msg);
  }

  void MainWindow::showDatabaseRebuildDone() {
    statusBar->showMessage("Done scanning.", 5000);
  }

  void MainWindow::showStatusMessage(QString msg) {
    statusBar->showMessage(msg);
  }

  void MainWindow::tryNewFolder(MediaItem::ptr_t parent) {
    if (!parent->isDirectory())
      return;

    auto dialog = qt_make_unique<TextPromptDialog>(QString("Enter name for new folder."), this);
    if (dialog->showDialog()) {
      database->makeFolderItem(dialog->getText(), parent);
    }
  }

  void MainWindow::tryMoveItem(MediaItem::ptr_t item) {
    auto dialog = qt_make_unique<MoveWindow>(database, this);
    dialog->setSelectedItem(item);
    if (dialog->showDialog() == QDialog::Accepted) {
      clearSelections();
      clearMedia();

      database->moveItem(item, dialog->getNewParent());
    }
  }

  void MainWindow::tryDeleteItem(MediaItem::ptr_t item) {
    auto text = QString("Delete item %1?").arg(item->getFilePath());
    auto result = QMessageBox::question(this, "Delete item?", text, QMessageBox::Yes, QMessageBox::No);
    if (result == QMessageBox::Yes) {
      clearSelections();
      clearMedia();

      database->deleteItem(item);
    }
  }

  void MainWindow::closeEvent(QCloseEvent* event) {
    AppSettings settings;
    settings.writeMainWindow("mainwindow", this);
    QMainWindow::closeEvent(event);
  }

  void MainWindow::setMediaFromItem(MediaItem::ptr_t item) {
    if (!item) {
      return;
    }

    view->loadMedia(item);
    //dirBrowser->setSelectedItem(item);
    //thumbnailView->setSelectedItem(item);
  }

  void MainWindow::createUI() {
    addDock(dirBrowser, "libraryview", "Library Explorer", Qt::LeftDockWidgetArea, true);
    addDock(thumbnailView, "thumbnailview", "Thumbnails", Qt::RightDockWidgetArea, true);

    setCentralWidget(view);

    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, this, &MainWindow::setMediaFromItem));
    cC(connect(dirBrowser, &DirBrowserWidget::selectedItemChanged, thumbnailView, &ThumbnailView::setSelectedItemNoEmit));
    cC(connect(dirBrowser, &DirBrowserWidget::selectionCleared, this, &MainWindow::clearMedia));
    cC(connect(dirBrowser, &DirBrowserWidget::moveItemRequested, this, &MainWindow::tryMoveItem));
    cC(connect(dirBrowser, &DirBrowserWidget::deleteItemRequested, this, &MainWindow::tryDeleteItem));
    cC(connect(dirBrowser, &DirBrowserWidget::newFolderItemRequested, this, &MainWindow::tryNewFolder));

    cC(connect(thumbnailView, &ThumbnailView::selectedItemChanged, this, &MainWindow::setMediaFromItem));
    cC(connect(thumbnailView, &ThumbnailView::selectedItemChanged, dirBrowser, &DirBrowserWidget::setSelectedItemNoEmit));
    cC(connect(thumbnailView, &ThumbnailView::selectionCleared, this, &MainWindow::clearMedia));
    cC(connect(thumbnailView, &ThumbnailView::moveItemRequested, this, &MainWindow::tryMoveItem));
    cC(connect(thumbnailView, &ThumbnailView::deleteItemRequested, this, &MainWindow::tryDeleteItem));

    cC(connect(database.get(), &MediaDatabase::itemWillBeRemoved, thumbnailView->getThumbnailModel(), &ThumbnailModel::removeItem));
    cC(connect(database.get(), &MediaDatabase::itemWillBeMoved, thumbnailView->getThumbnailModel(), &ThumbnailModel::moveItem));

    cC(connect(database.get(), &MediaDatabase::rebuildProgressUpdated, this, &MainWindow::showDatabaseRebuildProgress));
    cC(connect(database.get(), &MediaDatabase::databaseRebuild, this, &MainWindow::showDatabaseRebuildDone));

    statusBar = new QStatusBar();
    setStatusBar(statusBar);
  }

  void MainWindow::addDock(QWidget* widget, const QString& id, const QString& name, Qt::DockWidgetArea dockArea, bool visible) {
    auto dock = new QDockWidget(name, this);
    dock->setObjectName(id);

    dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
    dock->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
    dock->setWidget(widget);
    dock->setVisible(visible);

    docks[id] = dock;

    addDockWidget(dockArea, dock);
  }

  void MainWindow::clearSelections() {
    dirBrowser->clearSelection();
    thumbnailView->clearSelection();
  }
}