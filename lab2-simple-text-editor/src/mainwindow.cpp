#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "codeeditor.h"
#include "fileservice.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "textsearch.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPalette>
#include <QPushButton>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTimer>

#include <algorithm>

namespace
{
QString defaultDocumentsDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

bool sameText(const QString &left, const QString &right, bool caseSensitive)
{
    return QString::compare(
               left,
               right,
               caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)
        == 0;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_findDialog(new FindDialog(this))
    , m_replaceDialog(new ReplaceDialog(this))
    , m_aboutDialog(new AboutDialog(this))
    , m_lengthAndLinesLabel(new QLabel(this))
    , m_cursorPositionLabel(new QLabel(this))
    , m_autoSaveTimer(new QTimer(this))
{
    ui->setupUi(this);
    setIcons();

    m_lengthAndLinesLabel->setObjectName(QStringLiteral("lengthAndLinesLabel"));
    m_cursorPositionLabel->setObjectName(QStringLiteral("cursorPositionLabel"));
    m_lengthAndLinesLabel->setMinimumWidth(190);
    m_cursorPositionLabel->setMinimumWidth(150);
    m_lengthAndLinesLabel->setAlignment(Qt::AlignCenter);
    m_cursorPositionLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_lengthAndLinesLabel);
    statusBar()->addPermanentWidget(m_cursorPositionLabel);

    m_autoSaveTimer->setInterval(30 * 1000);
    m_autoSaveTimer->setSingleShot(false);

    connectActions();
    setCurrentFilePath(QString());
    updateStatusBar();
    updateActionStates();

    if (ui->actionAutoSave->isChecked()) {
        m_autoSaveTimer->start();
    }
    statusBar()->showMessage(tr("就绪"), 2500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

CodeEditor *MainWindow::editor() const
{
    return ui->editor;
}

QString MainWindow::currentFilePath() const
{
    return m_currentFilePath;
}

void MainWindow::setIcons()
{
    const auto setIcon = [](QAction *action, const QString &name) {
        action->setIcon(QIcon(QStringLiteral(":/icons/%1.svg").arg(name)));
    };
    setIcon(ui->actionNew, QStringLiteral("new"));
    setIcon(ui->actionOpen, QStringLiteral("open"));
    setIcon(ui->actionSave, QStringLiteral("save"));
    setIcon(ui->actionSaveAs, QStringLiteral("save_as"));
    setIcon(ui->actionUndo, QStringLiteral("undo"));
    setIcon(ui->actionRedo, QStringLiteral("redo"));
    setIcon(ui->actionCut, QStringLiteral("cut"));
    setIcon(ui->actionCopy, QStringLiteral("copy"));
    setIcon(ui->actionPaste, QStringLiteral("paste"));
    setIcon(ui->actionFind, QStringLiteral("find"));
    setIcon(ui->actionReplace, QStringLiteral("replace"));
    setIcon(ui->actionWordWrap, QStringLiteral("wordwrap"));
    setIcon(ui->actionShowLineNumbers, QStringLiteral("linenumber"));
    setIcon(ui->actionFontColor, QStringLiteral("fontcolor"));
    setIcon(ui->actionEditorBackground, QStringLiteral("background"));
    setIcon(ui->actionAbout, QStringLiteral("about"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/app.svg")));
}

void MainWindow::connectActions()
{
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    connect(ui->actionUndo, &QAction::triggered, ui->editor, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->editor, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->editor, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->editor, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->editor, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->editor, &QPlainTextEdit::selectAll);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    connect(ui->actionWordWrap, &QAction::toggled, ui->editor, [this](bool enabled) {
        ui->editor->setLineWrapMode(
            enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    });
    connect(ui->actionShowLineNumbers, &QAction::toggled,
            ui->editor, &CodeEditor::setLineNumbersVisible);
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::chooseFontColor);
    connect(ui->actionTextBackground, &QAction::triggered,
            this, &MainWindow::chooseTextBackground);
    connect(ui->actionEditorBackground, &QAction::triggered,
            this, &MainWindow::chooseEditorBackground);
    connect(ui->actionAutoSave, &QAction::toggled, this, &MainWindow::toggleAutoSave);

    connect(ui->actionToolBar, &QAction::toggled, ui->toolBar, &QWidget::setVisible);
    connect(ui->actionStatusBar, &QAction::toggled, statusBar(), &QWidget::setVisible);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateModifiedState);
    connect(ui->editor->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::updateStatusBar);
    connect(ui->editor->document(), &QTextDocument::undoAvailable,
            this, &MainWindow::updateActionStates);
    connect(ui->editor->document(), &QTextDocument::redoAvailable,
            this, &MainWindow::updateActionStates);
    connect(ui->editor, &QPlainTextEdit::copyAvailable,
            this, &MainWindow::updateActionStates);
    connect(ui->editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateCursorStatus);
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updateActionStates);

    connect(m_findDialog, &FindDialog::findRequested,
            this, [this](const QString &text, bool caseSensitive, bool backward) {
                findText(text, caseSensitive, backward);
            });
    connect(m_replaceDialog, &ReplaceDialog::findRequested,
            this, [this](const QString &text, bool caseSensitive, bool backward) {
                findText(text, caseSensitive, backward, true);
            });
    connect(m_replaceDialog, &ReplaceDialog::replaceRequested,
            this, [this](
                      const QString &text,
                      const QString &replacement,
                      bool caseSensitive,
                      bool backward) {
                replaceCurrent(text, replacement, caseSensitive, backward);
            });
    connect(m_replaceDialog, &ReplaceDialog::replaceAllRequested,
            this, [this](
                      const QString &text,
                      const QString &replacement,
                      bool caseSensitive) {
                replaceAll(text, replacement, caseSensitive);
            });
    connect(m_autoSaveTimer, &QTimer::timeout,
            this, &MainWindow::autoSaveDocument);
}

void MainWindow::newFile()
{
    if (!maybeSave()) {
        return;
    }
    ui->editor->clear();
    ui->editor->document()->clearUndoRedoStacks();
    setCurrentFilePath(QString());
    updateStatusBar();
    updateActionStates();
    statusBar()->showMessage(tr("已新建文件"), 2500);
}

void MainWindow::openFileDialog()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开文本文件"),
        defaultDocumentsDirectory(),
        tr("文本与源代码文件 (*.txt *.md *.cpp *.h *.hpp *.ui *.qss);;所有文件 (*)"));
    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

bool MainWindow::openFile(const QString &filePath)
{
    QString text;
    QString error;
    if (!FileService::readTextFile(filePath, &text, &error)) {
        QMessageBox::critical(
            this,
            tr("打开失败"),
            tr("无法打开文件：\n%1\n\n%2").arg(QDir::toNativeSeparators(filePath), error));
        return false;
    }
    if (!maybeSave()) {
        return false;
    }

    ui->editor->setPlainText(text);
    ui->editor->document()->clearUndoRedoStacks();
    setCurrentFilePath(filePath);
    QTextCursor cursor(ui->editor->document());
    cursor.movePosition(QTextCursor::Start);
    ui->editor->setTextCursor(cursor);
    updateStatusBar();
    updateActionStates();
    statusBar()->showMessage(tr("已打开 %1").arg(QFileInfo(filePath).fileName()), 3000);
    return true;
}

bool MainWindow::save()
{
    return m_currentFilePath.isEmpty() ? saveAs()
                                       : saveToPath(m_currentFilePath);
}

bool MainWindow::saveAs()
{
    QString initialPath = m_currentFilePath;
    if (initialPath.isEmpty()) {
        initialPath = QDir(defaultDocumentsDirectory()).filePath(tr("新建文本文件.txt"));
    }
    const QString filePath = QFileDialog::getSaveFileName(
        this, tr("另存为"), initialPath, tr("文本文件 (*.txt);;所有文件 (*)"));
    if (filePath.isEmpty()) {
        return false;
    }
    return saveToPath(filePath);
}

bool MainWindow::saveToPath(const QString &filePath, bool showError)
{
    QString error;
    if (!FileService::writeTextFile(filePath, ui->editor->toPlainText(), &error)) {
        if (showError) {
            QMessageBox::critical(
                this,
                tr("保存失败"),
                tr("无法保存文件：\n%1\n\n%2").arg(QDir::toNativeSeparators(filePath), error));
        }
        return false;
    }
    setCurrentFilePath(filePath);
    updateActionStates();
    statusBar()->showMessage(tr("已保存 %1").arg(QFileInfo(filePath).fileName()), 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->editor->document()->isModified()) {
        return true;
    }
    QMessageBox messageBox(
        QMessageBox::Warning,
        tr("未保存的更改"),
        tr("当前文件已修改，是否保存更改？"),
        QMessageBox::NoButton,
        this);
    QAbstractButton *saveButton =
        messageBox.addButton(tr("保存"), QMessageBox::AcceptRole);
    QAbstractButton *discardButton =
        messageBox.addButton(tr("不保存"), QMessageBox::DestructiveRole);
    messageBox.addButton(tr("取消"), QMessageBox::RejectRole);
    messageBox.setDefaultButton(qobject_cast<QPushButton *>(saveButton));
    messageBox.exec();

    if (messageBox.clickedButton() == saveButton) {
        return save();
    }
    return messageBox.clickedButton() == discardButton;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setCurrentFilePath(const QString &filePath)
{
    m_currentFilePath = filePath;
    ui->editor->document()->setModified(false);
    const QString name = FileService::displayName(filePath);
    setWindowTitle(tr("%1[*] - 简单文本编辑器").arg(name));
    setWindowModified(false);
    ui->editor->setToolTip(
        filePath.isEmpty() ? tr("尚未保存") : QDir::toNativeSeparators(filePath));
}

void MainWindow::updateModifiedState()
{
    setWindowModified(ui->editor->document()->isModified());
    updateActionStates();
}

void MainWindow::updateActionStates()
{
    QTextDocument *document = ui->editor->document();
    const QTextCursor cursor = ui->editor->textCursor();
    const bool hasDocumentText = !document->isEmpty();
    const QMimeData *clipboardData = QApplication::clipboard()->mimeData();

    ui->actionUndo->setEnabled(document->isUndoAvailable());
    ui->actionRedo->setEnabled(document->isRedoAvailable());
    ui->actionCut->setEnabled(cursor.hasSelection());
    ui->actionCopy->setEnabled(cursor.hasSelection());
    ui->actionPaste->setEnabled(clipboardData && clipboardData->hasText());
    ui->actionSelectAll->setEnabled(hasDocumentText);
}

void MainWindow::updateStatusBar()
{
    const QString text = ui->editor->toPlainText();
    m_lengthAndLinesLabel->setText(
        tr("length: %1    lines: %2")
            .arg(text.size())
            .arg(ui->editor->document()->blockCount()));
    updateCursorStatus();
}

void MainWindow::updateCursorStatus()
{
    const QTextCursor cursor = ui->editor->textCursor();
    m_cursorPositionLabel->setText(
        tr("Ln: %1    Col: %2")
            .arg(cursor.blockNumber() + 1)
            .arg(cursor.positionInBlock() + 1));
}

void MainWindow::chooseFont()
{
    bool accepted = false;
    QFont initialFont = ui->editor->currentCharFormat().font();
    if (initialFont.family().isEmpty()) {
        initialFont = ui->editor->font();
    }
    const QFont font = QFontDialog::getFont(
        &accepted, initialFont, this, tr("选择字体"));
    if (!accepted) {
        return;
    }
    QTextCharFormat format;
    format.setFont(font);
    applyCharFormat(format);
}

void MainWindow::chooseFontColor()
{
    QColor initial = ui->editor->currentCharFormat().foreground().color();
    if (!initial.isValid()) {
        initial = ui->editor->palette().color(QPalette::Text);
    }
    const QColor color = QColorDialog::getColor(initial, this, tr("选择字体颜色"));
    if (!color.isValid()) {
        return;
    }
    QTextCharFormat format;
    format.setForeground(color);
    applyCharFormat(format);
}

void MainWindow::chooseTextBackground()
{
    const QColor initial =
        ui->editor->currentCharFormat().background().color();
    const QColor color = QColorDialog::getColor(
        initial.isValid() ? initial : Qt::white,
        this,
        tr("选择字体背景色"));
    if (!color.isValid()) {
        return;
    }
    QTextCharFormat format;
    format.setBackground(color);
    applyCharFormat(format);
}

void MainWindow::chooseEditorBackground()
{
    QPalette palette = ui->editor->palette();
    const QColor color = QColorDialog::getColor(
        palette.color(QPalette::Base), this, tr("选择编辑器背景色"));
    if (!color.isValid()) {
        return;
    }
    palette.setColor(QPalette::Base, color);
    ui->editor->setPalette(palette);
    statusBar()->showMessage(tr("已更新编辑器背景色"), 2500);
}

void MainWindow::applyCharFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.mergeCharFormat(format);
    } else {
        ui->editor->mergeCurrentCharFormat(format);
    }
    ui->editor->setFocus();
}

QString MainWindow::selectedSingleLineText() const
{
    const QTextCursor cursor = ui->editor->textCursor();
    if (!cursor.hasSelection()) {
        return QString();
    }
    const QString selected = cursor.selectedText();
    return selected.contains(QChar::ParagraphSeparator) ? QString() : selected;
}

void MainWindow::showFindDialog()
{
    m_findDialog->showAndFocus(selectedSingleLineText());
}

void MainWindow::showReplaceDialog()
{
    m_replaceDialog->showAndFocus(selectedSingleLineText());
}

bool MainWindow::findText(
    const QString &query,
    bool caseSensitive,
    bool backward,
    bool fromReplaceDialog)
{
    if (query.isEmpty()) {
        showSearchMessage(tr("请输入查找内容。"), true);
        return false;
    }

    const QTextCursor current = ui->editor->textCursor();
    int fromPosition = current.position();
    if (current.hasSelection()) {
        fromPosition = backward ? current.selectionStart() : current.selectionEnd();
    }

    TextSearchOptions options;
    options.caseSensitive = caseSensitive;
    options.backward = backward;
    options.wrapAround = true;
    const TextMatch match = TextSearch::find(
        ui->editor->document(), fromPosition, query, options);
    if (!match.found) {
        showSearchMessage(
            tr("未找到“%1”。").arg(query),
            true);
        statusBar()->showMessage(tr("查找完成：未找到匹配内容"), 3000);
        return false;
    }

    QTextCursor cursor(ui->editor->document());
    cursor.setPosition(match.start);
    cursor.setPosition(match.start + match.length, QTextCursor::KeepAnchor);
    ui->editor->setTextCursor(cursor);
    ui->editor->centerCursor();
    if (match.wrapped) {
        showSearchMessage(tr("已到达文档边界，并从另一端继续查找。"));
        statusBar()->showMessage(tr("已循环查找：%1").arg(query), 3000);
    } else {
        showSearchMessage(QString());
        statusBar()->showMessage(tr("已找到：%1").arg(query), 2500);
    }
    Q_UNUSED(fromReplaceDialog)
    return true;
}

void MainWindow::replaceCurrent(
    const QString &query,
    const QString &replacement,
    bool caseSensitive,
    bool backward)
{
    QTextCursor cursor = ui->editor->textCursor();
    if (!cursor.hasSelection()
        || !sameText(cursor.selectedText(), query, caseSensitive)) {
        if (findText(query, caseSensitive, backward, true)) {
            m_replaceDialog->showMessage(tr("已选中匹配项，再次点击“替换”即可。"));
        }
        return;
    }

    const int replacementStart = cursor.selectionStart();
    cursor.insertText(replacement);
    cursor.setPosition(replacementStart + replacement.size());
    ui->editor->setTextCursor(cursor);
    m_replaceDialog->showMessage(tr("已替换 1 处。"));
    statusBar()->showMessage(tr("已替换 1 处"), 2500);
    findText(query, caseSensitive, backward, true);
}

void MainWindow::replaceAll(
    const QString &query,
    const QString &replacement,
    bool caseSensitive)
{
    if (query.isEmpty()) {
        m_replaceDialog->showMessage(tr("请输入查找内容。"), true);
        return;
    }
    const int count = TextSearch::replaceAll(
        ui->editor->document(), query, replacement, caseSensitive);
    if (count == 0) {
        m_replaceDialog->showMessage(
            tr("未找到“%1”，没有执行替换。").arg(query), true);
        statusBar()->showMessage(tr("全部替换：未找到匹配内容"), 3000);
        return;
    }
    m_replaceDialog->showMessage(tr("全部替换完成，共替换 %1 处。").arg(count));
    statusBar()->showMessage(tr("全部替换完成：%1 处").arg(count), 3000);
    updateStatusBar();
    updateActionStates();
}

void MainWindow::showSearchMessage(const QString &message, bool error)
{
    m_findDialog->showMessage(message, error);
    m_replaceDialog->showMessage(message, error);
}

void MainWindow::showAboutDialog()
{
    m_aboutDialog->exec();
}

void MainWindow::toggleAutoSave(bool enabled)
{
    if (enabled) {
        m_autoSaveTimer->start();
        statusBar()->showMessage(tr("已开启每 30 秒自动保存"), 3000);
    } else {
        m_autoSaveTimer->stop();
        statusBar()->showMessage(tr("已关闭自动保存"), 3000);
    }
}

void MainWindow::autoSaveDocument()
{
    if (!ui->editor->document()->isModified()) {
        return;
    }
    if (m_currentFilePath.isEmpty()) {
        statusBar()->showMessage(tr("自动保存已跳过：文件尚未首次保存"), 2500);
        return;
    }
    if (saveToPath(m_currentFilePath, false)) {
        statusBar()->showMessage(tr("已自动保存"), 2500);
    } else {
        statusBar()->showMessage(tr("自动保存失败，请手动检查文件"), 5000);
    }
}
