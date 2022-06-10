#pragma once

#include <QPointer>
#include <QDialog>

#include "../MediaItem.h"
#include "../MediaDatabase.h"

class QTreeView;
class QTextEdit;
class QLineEdit;

namespace realn {
  class ImageFileSystemModel;
  class MediaItemStorage;

  class MoveWindow : public QDialog {
    Q_OBJECT;
  public:
    MoveWindow(std::shared_ptr<MediaDatabase> database, std::shared_ptr<MediaItemStorage> storage, QWidget* parent = nullptr);
    ~MoveWindow();

    QDialog::DialogCode showDialog();

    MediaItem::ptr_t getNewParent() const;

    MediaItem::ptr_t getSelectedItem() const;
    void setSelectedItem(MediaItem::ptr_t item);

  private slots:
    void onSelectionChanged();

  protected:
    void closeEvent(QCloseEvent* event) override;

  private:
    void expandToItem(MediaItem::ptr_t item);

    QPointer<QLineEdit> pathWidget;
    QPointer<QTreeView> browseWidget;
    QPointer<ImageFileSystemModel> model;
  };
}
