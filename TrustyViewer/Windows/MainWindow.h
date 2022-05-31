#pragma once

#include <vector>

#include <QMainWindow>
#include <QDockWidget>

#include "../widgets/MediaContentWidget.h"
#include "../widgets/DirBrowserWidget.h"
#include "../widgets/ThumbnailView.h"

class QStatusBar;

namespace realn {
  class MainWindow : public QMainWindow {
    Q_OBJECT;
  public:
    MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase, std::shared_ptr<ThumbnailWorker> worker);

  private slots:
    void setMediaFromItem(MediaItem::ptr_t item);
    void clearMedia();
    void showDatabaseRebuildProgress(int done, int total);
    void showDatabaseRebuildDone();
    void showStatusMessage(QString msg);

  private:
    using dockptr_t = QPointer<QDockWidget>;

    void createUI();
    void addDock(QWidget* widget, const QString& name, Qt::DockWidgetArea dockArea, bool visible);

    QPointer<MediaContentWidget> view;
    QPointer<DirBrowserWidget> dirBrowser;
    QPointer<ThumbnailView> thumbnailView;
    QPointer<QStatusBar> statusBar;
    std::vector<dockptr_t> docks;
  };
}