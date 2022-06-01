#pragma once

#include <QPointer>
#include <QDialog>

class QLineEdit;

namespace realn {
  class TextPromptDialog : public QDialog {
    Q_OBJECT;
  public:
    TextPromptDialog(const QString& text, QWidget* parent = nullptr);
    ~TextPromptDialog();

    bool showDialog();

    QString getText() const;

  private:
    QPointer<QLineEdit> inputWidget;
  };
}