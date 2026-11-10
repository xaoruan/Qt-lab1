#pragma once

#include <QMainWindow>

class AboutDialog;
class CodeEditor;
class FindDialog;
class QLabel;
class QTextCharFormat;
class ReplaceDialog;
class QTimer;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    CodeEditor *editor() const;
    QString currentFilePath() const;
    bool saveToPath(const QString &filePath, bool showError = true);

public slots:
    void newFile();
    void openFileDialog();
    bool openFile(const QString &filePath);
    bool save();
    bool saveAs();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateModifiedState();
    void updateActionStates();
    void updateStatusBar();
    void updateCursorStatus();
    void chooseFont();
    void chooseFontColor();
    void chooseTextBackground();
    void chooseEditorBackground();
    void showFindDialog();
    void showReplaceDialog();
    void showAboutDialog();
    void toggleAutoSave(bool enabled);
    void autoSaveDocument();

private:
    bool maybeSave();
    bool findText(
        const QString &query,
        bool caseSensitive,
        bool backward,
        bool fromReplaceDialog = false);
    void replaceCurrent(
        const QString &query,
        const QString &replacement,
        bool caseSensitive,
        bool backward);
    void replaceAll(
        const QString &query,
        const QString &replacement,
        bool caseSensitive);
    void applyCharFormat(const QTextCharFormat &format);
    void setCurrentFilePath(const QString &filePath);
    void setIcons();
    void connectActions();
    void showSearchMessage(const QString &message, bool error = false);
    QString selectedSingleLineText() const;

    Ui::MainWindow *ui;
    FindDialog *m_findDialog;
    ReplaceDialog *m_replaceDialog;
    AboutDialog *m_aboutDialog;
    QLabel *m_lengthAndLinesLabel;
    QLabel *m_cursorPositionLabel;
    QTimer *m_autoSaveTimer;
    QString m_currentFilePath;
};
