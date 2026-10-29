#include "calculator.h"

#include <QtTest>

class CalculatorTests : public QObject
{
    Q_OBJECT

private:
    static void enter(CalculatorEngine &engine, const QString &text)
    {
        for (const QChar ch : text) {
            if (ch.isDigit())
                engine.inputDigit(ch);
            else if (ch == QLatin1Char('.'))
                engine.inputDecimal();
            else if (ch == QLatin1Char('='))
                engine.calculate();
            else
                engine.inputOperator(ch);
        }
    }

private slots:
    void decimalAddition()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("12.5+7.5="));
        QCOMPARE(engine.displayText(), QStringLiteral("20"));
    }

    void duplicateDecimalIsIgnored()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("1..2"));
        QCOMPARE(engine.displayText(), QStringLiteral("1.2"));
    }

    void consecutiveOperatorReplacesPrevious()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("6+*2="));
        QCOMPARE(engine.displayText(), QStringLiteral("12"));
    }

    void divisionByZeroShowsError()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("8/0="));
        QVERIFY(engine.isError());
        QCOMPARE(engine.displayText(), QStringLiteral("错误：除数不能为0"));
    }

    void errorCanBeClearedByDigit()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("8/0="));
        enter(engine, QStringLiteral("1.5+2="));
        QCOMPARE(engine.displayText(), QStringLiteral("3.5"));
        QVERIFY(!engine.isError());
    }

    void continuedCalculation()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("2+3*4="));
        QCOMPARE(engine.displayText(), QStringLiteral("20"));
    }

    void resultThenDigitStartsNewCalculation()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("2+3=7"));
        QCOMPARE(engine.displayText(), QStringLiteral("7"));
        QVERIFY(engine.expressionText().isEmpty());
    }

    void backspaceAndClear()
    {
        CalculatorEngine engine;
        enter(engine, QStringLiteral("123"));
        engine.backspace();
        QCOMPARE(engine.displayText(), QStringLiteral("12"));
        engine.clear();
        QCOMPARE(engine.displayText(), QStringLiteral("0"));
    }
};

QTEST_APPLESS_MAIN(CalculatorTests)

#include "test_calculator.moc"
