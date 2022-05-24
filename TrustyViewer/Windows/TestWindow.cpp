
#include "../widgets/VideoButtonsWidget.h"

#include "TestWindow.h"

namespace realn {
  TestWindow::TestWindow() {
    setCentralWidget(new VideoButtonsWidget());
  }

  TestWindow::~TestWindow() = default;
}