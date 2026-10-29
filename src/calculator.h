#pragma once

#include <QChar>
#include <QString>

class CalculatorEngine
{
public:
    QString displayText() const;
    QString expressionText() const;
    bool isError() const;

    void inputDigit(const QChar &digit);
    void inputDecimal();
    void inputOperator(const QChar &op);
    void calculate();
    void backspace();
    void clear();

private:
    static QString formatNumber(double value);
    bool applyPendingOperator();
    void reset();
    double currentValue() const;

    QString m_display = QStringLiteral("0");
    QString m_expression;
    double m_storedValue = 0.0;
    QChar m_pendingOperator;
    bool m_waitingForOperand = true;
    bool m_justCalculated = false;
    bool m_error = false;
};
