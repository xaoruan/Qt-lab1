#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("关于简单文本编辑器"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
