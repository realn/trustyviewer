
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QBoxLayout>

#include "../Utils.h"
#include "TextPromptDialog.h"

namespace realn {
  TextPromptDialog::TextPromptDialog(const QString& text, QWidget* parent)
    : QDialog(parent) {
    setMinimumSize(300, 50);

    inputWidget = new QLineEdit();

    auto buttons = new QDialogButtonBox();
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);

    cC(connect(buttons, &QDialogButtonBox::accepted, this, &TextPromptDialog::accept));
    cC(connect(buttons, &QDialogButtonBox::rejected, this, &TextPromptDialog::reject));

    auto layout = new QVBoxLayout();
    layout->addWidget(inputWidget);
    layout->addWidget(buttons);
    setLayout(layout);
  }

  TextPromptDialog::~TextPromptDialog() = default;
  bool TextPromptDialog::showDialog() {
    auto result = exec();
    return static_cast<QDialog::DialogCode>(result) == QDialog::DialogCode::Accepted;
  }

  QString TextPromptDialog::getText() const {
    return inputWidget->text();
  }
}
