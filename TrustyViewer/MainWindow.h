#pragma once

#include <QMainWindow>

#include "widgets/ImageView.h"
#include "widgets/DirBrowserWidget.h"

namespace realn {
  class MainWindow : public QMainWindow {
    Q_OBJECT;
  public:
    MainWindow();

  private slots:
    void setImageToImageView();

  private:
    QWidget* createUI();

    QPointer<ImageView> view;
    QPointer<DirBrowserWidget> dirBrowser;
  };
}