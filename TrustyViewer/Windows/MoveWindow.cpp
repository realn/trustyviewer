
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>
#include <QTreeView>
#include <QBoxLayout>
#include <QDialogButtonBox>

#include "../AppSettings.h"
#include "../models/ImageFileSystemModel.h"
#include "MoveWindow.h"
#include "../Utils.h"

namespace realn {
  MoveWindow::MoveWindow(std::shared_ptr<MediaDatabase> database, std::shared_ptr<MediaItemStorage> storage, QWidget* parent)
    : QDialog(parent) {

    pathWidget = new QLineEdit();
    pathWidget->setReadOnly(true);

    model = new ImageFileSystemModel(database, storage, nullptr, MediaItemFilter::OnlyDirectories);

    browseWidget = new QTreeView();
    browseWidget->setModel(model);

    cC(connect(browseWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MoveWindow::onSelectionChanged));

    auto layout = new QVBoxLayout();

    layout->addWidget(pathWidget, 0);
    layout->addWidget(browseWidget, 1);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons, 0);

    cC(connect(buttons, &QDialogButtonBox::accepted, this, &MoveWindow::accept));
    cC(connect(buttons, &QDialogButtonBox::rejected, this, &MoveWindow::reject));

    setLayout(layout);

    AppSettings settings;
    settings.readWindow("movewindow", this);
  }

  MoveWindow::~MoveWindow() {
    AppSettings settings;
    settings.writeWindow("movewindow", this);
  }

  QDialog::DialogCode MoveWindow::showDialog() {
    auto result = exec();

    return static_cast<QDialog::DialogCode>(result);
  }

  MediaItem::ptr_t MoveWindow::getNewParent() const {
    return getSelectedItem();
  }

  MediaItem::ptr_t MoveWindow::getSelectedItem() const {
    auto index = browseWidget->currentIndex();
    if (!index.isValid())
      return nullptr;
    return model->getItemForIndex(index);
  }

  void MoveWindow::setSelectedItem(MediaItem::ptr_t item) {
    if (!item)
      return;
    if (!item->isDirectory())
      item = item->getParent();

    auto index = model->getIndexForItem(item);
    if (index.isValid())
      browseWidget->setCurrentIndex(index);
  }

  void MoveWindow::closeEvent(QCloseEvent* event) {
    QWidget::closeEvent(event);
  }

  void MoveWindow::expandToItem(MediaItem::ptr_t item) {
    if (!item)
      return;

    MediaItem::itemvector_t list;
    do {
      list.insert(list.begin(), item);
      item = item->getParent();
    } while (item);


    for (auto& item : list) {
      auto tv = browseWidget;
      auto tm = model;

      QTimer::singleShot(0, [tv, tm, item]() {
        if (tv && tm) {
          auto index = tm->getIndexForItem(item);
          tv->expand(index);
        }
                         });
    }
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