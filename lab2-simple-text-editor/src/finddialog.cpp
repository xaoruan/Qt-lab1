#include "finddialog.h"
#include "ui_finddialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QStyle>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("查找"));
    ui->messageLabel->hide();

    connect(ui->findNextButton, &QPushButton::clicked,
            this, &FindDialog::requestFind);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    connect(ui->findLineEdit, &QLineEdit::returnPressed,
            this, &FindDialog::requestFind);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::searchText() const
{
    return ui->findLineEdit->text();
}

bool FindDialog::caseSensitive() const
{
    return ui->caseSensitiveCheckBox->isChecked();
}

bool FindDialog::searchBackward() const
{
    return ui->backwardRadioButton->isChecked();
}

void FindDialog::showMessage(const QString &message, bool error)
{
    ui->messageLabel->setText(message);
    ui->messageLabel->setProperty("error", error);
    ui->messageLabel->style()->unpolish(ui->messageLabel);
    ui->messageLabel->style()->polish(ui->messageLabel);
    ui->messageLabel->setVisible(!message.isEmpty());
}

void FindDialog::setSearchText(const QString &text)
{
    ui->findLineEdit->setText(text);
}

void FindDialog::showAndFocus(const QString &selectedText)
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

void FindDialog::requestFind()
{
    if (searchText().isEmpty()) {
        showMessage(tr("请输入查找内容。"), true);
        return;
    }
    showMessage(QString());
    emit findRequested(searchText(), caseSensitive(), searchBackward());
}
