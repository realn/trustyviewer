#pragma once

#include <map>
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
    ~MainWindow() override;

  private slots:
    void setMediaFromItem(MediaItem::ptr_t item);
    void clearMedia();
    void showDatabaseRebuildProgress(int done, int total);
    void showDatabaseRebuildDone();
    void showStatusMessage(QString msg);
    void tryNewFolder(MediaItem::ptr_t parent);
    void tryMoveItem(MediaItem::ptr_t item);
    void tryDeleteItem(MediaItem::ptr_t item);

  protected:
    void closeEvent(QCloseEvent* event) override;

  private:
    using dockptr_t = QPointer<QDockWidget>;
    using dockmap_t = std::map<QString, dockptr_t>;

    void createUI();
    void addDock(QWidget* widget, const QString& id, const QString& name, Qt::DockWidgetArea dockArea, bool visible);
    void clearSelections();

    QPointer<MediaContentWidget> view;
    QPointer<DirBrowserWidget> dirBrowser;
    QPointer<ThumbnailView> thumbnailView;
    QPointer<QStatusBar> statusBar;
    std::shared_ptr<MediaDatabase> database;
    dockmap_t docks;
  };
}