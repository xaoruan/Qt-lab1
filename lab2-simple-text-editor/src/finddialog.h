#pragma once

#include <QDialog>

namespace Ui
{
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog() override;

    QString searchText() const;
    bool caseSensitive() const;
    bool searchBackward() const;
    void showMessage(const QString &message, bool error = false);
    void setSearchText(const QString &text);

signals:
    void findRequested(const QString &text, bool caseSensitive, bool backward);

public slots:
    void showAndFocus(const QString &selectedText = QString());

private slots:
    void requestFind();

private:
    Ui::FindDialog *ui;
};
