

#include <QTextEdit>
#include <QLineEdit>
#include <QTreeView>
#include <QBoxLayout>
#include <QDialogButtonBox>

#include "../AppSettings.h"
#include "../models/ImageFileSystemModel.h"
#include "MoveWindow.h"

namespace realn {
  MoveWindow::MoveWindow(std::shared_ptr<MediaDatabase> database, QWidget* parent)
    : QDialog(parent) {

    pathWidget = new QLineEdit();
    pathWidget->setReadOnly(true);

    model = new ImageFileSystemModel(database, MediaItemFilter::OnlyDirectories);

    browseWidget = new QTreeView();
    browseWidget->setModel(model);

    connect(browseWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MoveWindow::onSelectionChanged);

    auto layout = new QVBoxLayout();

    layout->addWidget(pathWidget, 0);
    layout->addWidget(browseWidget, 1);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons, 0);

    connect(buttons, &QDialogButtonBox::accepted, this, &MoveWindow::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MoveWindow::reject);

    setLayout(layout);

    AppSettings settings;
    settings.readWindow("movewindow", this);
  }

  MoveWindow::~MoveWindow() = default;

  QDialog::DialogCode MoveWindow::showDialog() {
    auto result = exec();
    return static_cast<QDialog::DialogCode>(result);
  }
  
  MediaItem::ptr_t MoveWindow::getNewParent() const {
    return getSelectedItem();
  }

  MediaItem::ptr_t MoveWindow::getSelectedItem() const {
    auto indices = browseWidget->selectionModel()->selectedIndexes();
    if (indices.empty())
      return nullptr;
    const auto& index = indices.first();
    return model->getItemForIndex(index);
  }

  void MoveWindow::closeEvent(QCloseEvent* event) {
    AppSettings settings;
    settings.writeWindow("movewindow", this);
    QWidget::closeEvent(event);
  }

  void MoveWindow::onSelectionChanged() {
    auto item = getSelectedItem();
    if (!item) {
      pathWidget->setText("");
      return;
    }

    pathWidget->setText(item->getPathToRoot());
  }
}