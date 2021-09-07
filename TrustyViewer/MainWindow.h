#pragma once

#include <QMainWindow>

#include "widgets/ImageView.h"
#include "widgets/DirBrowserWidget.h"

namespace realn {
  class MainWindow : public QMainWindow {
    Q_OBJECT;
  public:
    MainWindow(std::shared_ptr<ExtPluginList> plugins);

  private slots:
    void setImageToImageView();

  private:
    void createUI();

    QPointer<ImageView> view;
    QPointer<DirBrowserWidget> dirBrowser;
  };
}