#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QtTest>

class KeyboardTests : public QObject
{
    Q_OBJECT

private slots:
    void keyboardCalculation()
    {
        MainWindow window;
        window.show();
        window.activateWindow();
        window.setFocus();

        QTest::keyClicks(&window, QStringLiteral("8*7"));
        QTest::keyClick(&window, Qt::Key_Return);

        auto *display = window.findChild<QLineEdit *>(QStringLiteral("display"));
        QVERIFY(display != nullptr);
        QCOMPARE(display->text(), QStringLiteral("56"));
    }

    void keyboardBackspaceAndClear()
    {
        MainWindow window;
        window.show();
        window.setFocus();

        QTest::keyClicks(&window, QStringLiteral("123"));
        QTest::keyClick(&window, Qt::Key_Backspace);

        auto *display = window.findChild<QLineEdit *>(QStringLiteral("display"));
        QVERIFY(display != nullptr);
        QCOMPARE(display->text(), QStringLiteral("12"));

        QTest::keyClick(&window, Qt::Key_Escape);
        QCOMPARE(display->text(), QStringLiteral("0"));
    }

    void mouseAndKeyboardUseSameState()
    {
        MainWindow window;
        window.show();
        window.setFocus();

        auto *button = window.findChild<QPushButton *>(QStringLiteral("btn7"));
        auto *display = window.findChild<QLineEdit *>(QStringLiteral("display"));
        QVERIFY(button != nullptr);
        QVERIFY(display != nullptr);

        QTest::mouseClick(button, Qt::LeftButton);
        QTest::keyClick(&window, Qt::Key_Plus);
        QTest::keyClick(&window, Qt::Key_3);
        QTest::keyClick(&window, Qt::Key_Equal);
        QCOMPARE(display->text(), QStringLiteral("10"));
    }
};

QTEST_MAIN(KeyboardTests)

#include "test_keyboard.moc"
