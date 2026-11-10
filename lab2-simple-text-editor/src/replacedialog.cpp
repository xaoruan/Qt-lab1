#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QStyle>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("替换"));
    ui->messageLabel->hide();

    connect(ui->findNextButton, &QPushButton::clicked,
            this, &ReplaceDialog::requestFind);
    connect(ui->replaceButton, &QPushButton::clicked,
            this, &ReplaceDialog::requestReplace);
    connect(ui->replaceAllButton, &QPushButton::clicked,
            this, &ReplaceDialog::requestReplaceAll);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    connect(ui->findLineEdit, &QLineEdit::returnPressed,
            this, &ReplaceDialog::requestFind);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QString ReplaceDialog::searchText() const
{
    return ui->findLineEdit->text();
}

QString ReplaceDialog::replacementText() const
{
    return ui->replaceLineEdit->text();
}

bool ReplaceDialog::caseSensitive() const
{
    return ui->caseSensitiveCheckBox->isChecked();
}

bool ReplaceDialog::searchBackward() const
{
    return ui->backwardRadioButton->isChecked();
}

void ReplaceDialog::showMessage(const QString &message, bool error)
{
    ui->messageLabel->setText(message);
    ui->messageLabel->setProperty("error", error);
    ui->messageLabel->style()->unpolish(ui->messageLabel);
    ui->messageLabel->style()->polish(ui->messageLabel);
    ui->messageLabel->setVisible(!message.isEmpty());
}

void ReplaceDialog::setSearchText(const QString &text)
{
    ui->findLineEdit->setText(text);
}

void ReplaceDialog::showAndFocus(const QString &selectedText)
{
    if (!selectedText.isEmpty() && !selectedText.contains(QLatin1Char('\n'))) {
        ui->findLineEdit->setText(selectedText);
    }
    ui->messageLabel->clear();
    ui->messageLabel->hide();
    show();
    raise();
    activateWindow();
    ui->findLineEdit->setFocus();
    ui->findLineEdit->selectAll();
}

void ReplaceDialog::requestFind()
{
    if (searchText().isEmpty()) {
        showMessage(tr("请输入查找内容。"), true);
        return;
    }
    showMessage(QString());
    emit findRequested(searchText(), caseSensitive(), searchBackward());
}

void ReplaceDialog::requestReplace()
{
    emitReplace(false);
}

void ReplaceDialog::requestReplaceAll()
{
    emitReplace(true);
}

void ReplaceDialog::emitReplace(bool replaceAll)
{
    if (searchText().isEmpty()) {
        showMessage(tr("请输入查找内容。"), true);
        return;
    }
    showMessage(QString());
    if (replaceAll) {
        emit replaceAllRequested(
            searchText(), replacementText(), caseSensitive());
    } else {
        emit replaceRequested(
            searchText(),
            replacementText(),
            caseSensitive(),
            searchBackward());
    }
}
