#include "aboutdialog.h"
#include "codeeditor.h"
#include "finddialog.h"
#include "mainwindow.h"
#include "replacedialog.h"
#include "uifont.h"

#include <QAction>
#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTemporaryDir>
#include <QTextCursor>
#include <QtTest>

class TestUi : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void actionStatesFollowEditorState();
    void statusLabelsTrackTextAndCursor();
    void lineNumbersAndWordWrapToggle();
    void savesAndReopensUtf8Text();
    void searchAndReplaceFromDialogs();
    void autoSaveWritesChangedDocument();
    void rendersMainWindowAndDialogs();
};

void TestUi::initTestCase()
{
    applyApplicationFont();
}

void TestUi::actionStatesFollowEditorState()
{
    MainWindow window;
    CodeEditor *editor = window.editor();
    QAction *cut = window.findChild<QAction *>(QStringLiteral("actionCut"));
    QAction *copy = window.findChild<QAction *>(QStringLiteral("actionCopy"));
    QAction *undo = window.findChild<QAction *>(QStringLiteral("actionUndo"));

    QVERIFY(cut);
    QVERIFY(copy);
    QVERIFY(undo);
    QVERIFY(!cut->isEnabled());
    QVERIFY(!copy->isEnabled());

    editor->insertPlainText(QStringLiteral("selected text"));
    editor->selectAll();
    QApplication::processEvents();
    QVERIFY(cut->isEnabled());
    QVERIFY(copy->isEnabled());
    QVERIFY(undo->isEnabled());
}

void TestUi::statusLabelsTrackTextAndCursor()
{
    MainWindow window;
    CodeEditor *editor = window.editor();
    QLabel *lengthLabel =
        window.findChild<QLabel *>(QStringLiteral("lengthAndLinesLabel"));
    QLabel *cursorLabel =
        window.findChild<QLabel *>(QStringLiteral("cursorPositionLabel"));

    QVERIFY(lengthLabel);
    QVERIFY(cursorLabel);
    editor->setPlainText(QStringLiteral("first\nsecond\nthird"));
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    editor->setTextCursor(cursor);
    QApplication::processEvents();

    QVERIFY(lengthLabel->text().contains(QStringLiteral("lines: 3")));
    QVERIFY(cursorLabel->text().contains(QStringLiteral("Ln: 3")));
    QVERIFY(cursorLabel->text().contains(QStringLiteral("Col: 6")));
}

void TestUi::lineNumbersAndWordWrapToggle()
{
    MainWindow window;
    CodeEditor *editor = window.editor();
    QAction *lineNumbers =
        window.findChild<QAction *>(QStringLiteral("actionShowLineNumbers"));
    QAction *wordWrap =
        window.findChild<QAction *>(QStringLiteral("actionWordWrap"));

    QVERIFY(lineNumbers);
    QVERIFY(wordWrap);
    QVERIFY(editor->lineNumbersVisible());
    QCOMPARE(editor->lineWrapMode(), QPlainTextEdit::WidgetWidth);

    lineNumbers->trigger();
    wordWrap->trigger();
    QVERIFY(!editor->lineNumbersVisible());
    QCOMPARE(editor->lineWrapMode(), QPlainTextEdit::NoWrap);
}

void TestUi::savesAndReopensUtf8Text()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString path = temporaryDirectory.filePath(QStringLiteral("lab2.txt"));

    MainWindow window;
    const QString expected = QStringLiteral("第一行\nsecond line\n第三行");
    window.editor()->setPlainText(expected);
    QVERIFY(window.saveToPath(path));
    QCOMPARE(window.currentFilePath(), path);

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString onDisk = QString::fromUtf8(file.readAll());
    QCOMPARE(onDisk, expected);

    window.editor()->document()->setModified(false);
    window.editor()->clear();
    window.editor()->document()->setModified(false);
    QVERIFY(window.openFile(path));
    QCOMPARE(window.editor()->toPlainText(), expected);
}

void TestUi::searchAndReplaceFromDialogs()
{
    MainWindow window;
    CodeEditor *editor = window.editor();
    editor->setPlainText(QStringLiteral("alpha beta alpha"));

    FindDialog *findDialog = window.findChild<FindDialog *>();
    ReplaceDialog *replaceDialog = window.findChild<ReplaceDialog *>();
    QVERIFY(findDialog);
    QVERIFY(replaceDialog);

    findDialog->setSearchText(QStringLiteral("alpha"));
    QVERIFY(QMetaObject::invokeMethod(
        findDialog, "requestFind", Qt::DirectConnection));
    QCOMPARE(editor->textCursor().selectedText(), QStringLiteral("alpha"));

    QLineEdit *findEdit =
        replaceDialog->findChild<QLineEdit *>(QStringLiteral("findLineEdit"));
    QLineEdit *replaceEdit =
        replaceDialog->findChild<QLineEdit *>(QStringLiteral("replaceLineEdit"));
    QVERIFY(findEdit);
    QVERIFY(replaceEdit);
    findEdit->setText(QStringLiteral("alpha"));
    replaceEdit->setText(QStringLiteral("gamma"));
    QVERIFY(QMetaObject::invokeMethod(
        replaceDialog, "requestReplaceAll", Qt::DirectConnection));
    QCOMPARE(editor->toPlainText(), QStringLiteral("gamma beta gamma"));
}

void TestUi::autoSaveWritesChangedDocument()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString path =
        temporaryDirectory.filePath(QStringLiteral("autosave.txt"));

    MainWindow window;
    window.editor()->setPlainText(QStringLiteral("before"));
    QVERIFY(window.saveToPath(path));
    window.editor()->moveCursor(QTextCursor::End);
    window.editor()->insertPlainText(QStringLiteral(" after"));
    QVERIFY(window.editor()->document()->isModified());

    QVERIFY(QMetaObject::invokeMethod(
        &window, "autoSaveDocument", Qt::DirectConnection));
    QVERIFY(!window.editor()->document()->isModified());

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(
        QString::fromUtf8(file.readAll()),
        QStringLiteral("before after"));
}

void TestUi::rendersMainWindowAndDialogs()
{
    MainWindow window;
    window.resize(960, 680);
    window.editor()->setPlainText(
        QStringLiteral("Simple text editor\n第二行\nThird line"));
    window.show();
    QApplication::processEvents();
    QVERIFY(!window.grab().isNull());
    QVERIFY(window.grab().save(
        QDir::current().filePath(QStringLiteral("lab2-ui-main.png"))));

    AboutDialog about(&window);
    about.show();
    FindDialog find(&window);
    find.setSearchText(QStringLiteral("Second"));
    find.show();
    ReplaceDialog replace(&window);
    replace.setSearchText(QStringLiteral("Second"));
    replace.show();
    QApplication::processEvents();

    QVERIFY(about.grab().save(
        QDir::current().filePath(QStringLiteral("lab2-ui-about.png"))));
    QVERIFY(find.grab().save(
        QDir::current().filePath(QStringLiteral("lab2-ui-find.png"))));
    QVERIFY(replace.grab().save(
        QDir::current().filePath(QStringLiteral("lab2-ui-replace.png"))));
}

QTEST_MAIN(TestUi)

#include "test_ui.moc"
