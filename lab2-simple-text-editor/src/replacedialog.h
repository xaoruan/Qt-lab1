#pragma once

#include <QDialog>

namespace Ui
{
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog() override;

    QString searchText() const;
    QString replacementText() const;
    bool caseSensitive() const;
    bool searchBackward() const;
    void showMessage(const QString &message, bool error = false);
    void setSearchText(const QString &text);

signals:
    void findRequested(const QString &text, bool caseSensitive, bool backward);
    void replaceRequested(
        const QString &text,
        const QString &replacement,
        bool caseSensitive,
        bool backward);
    void replaceAllRequested(
        const QString &text,
        const QString &replacement,
        bool caseSensitive);

public slots:
    void showAndFocus(const QString &selectedText = QString());

private slots:
    void requestFind();
    void requestReplace();
    void requestReplaceAll();

private:
    void emitReplace(bool replaceAll);
    Ui::ReplaceDialog *ui;
};
