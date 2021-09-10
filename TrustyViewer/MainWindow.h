#pragma once

#include <vector>

#include <QMainWindow>
#include <QDockWidget>

#include "widgets/ImageView.h"
#include "widgets/DirBrowserWidget.h"
#include "widgets/ThumbnailView.h"

namespace realn {
  class MainWindow : public QMainWindow {
    Q_OBJECT;
  public:
    MainWindow(std::shared_ptr<ExtPluginList> plugins, std::shared_ptr<MediaDatabase> mediaDatabase);

  private slots:
    void setImageToImageView();

  private:
    using dockptr_t = QPointer<QDockWidget>;

    void createUI();
    void addDock(QWidget* widget, const QString& name, Qt::DockWidgetArea dockArea, bool visible);

    QPointer<ImageView> view;
    QPointer<DirBrowserWidget> dirBrowser;
    QPointer<ThumbnailView> thumbnailView;
    std::vector<dockptr_t> docks;
  };
}