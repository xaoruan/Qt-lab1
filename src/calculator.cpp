#include "calculator.h"

#include <QRegularExpression>
#include <QtGlobal>

#include <cmath>

QString CalculatorEngine::displayText() const
{
    return m_display;
}

QString CalculatorEngine::expressionText() const
{
    return m_expression;
}

bool CalculatorEngine::isError() const
{
    return m_error;
}

QString CalculatorEngine::formatNumber(double value)
{
    if (std::abs(value) < 1.0e-12)
        value = 0.0;

    QString text = QString::number(value, 'g', 12);
    if (text.contains(QLatin1Char('e')) || text.contains(QLatin1Char('E')))
        text = QString::number(value, 'f', 8);
    if (text.contains(QLatin1Char('.'))) {
        while (text.endsWith(QLatin1Char('0')))
            text.chop(1);
        if (text.endsWith(QLatin1Char('.')))
            text.chop(1);
    }
    return text == QStringLiteral("-0") ? QStringLiteral("0") : text;
}

void CalculatorEngine::reset()
{
    m_display = QStringLiteral("0");
    m_expression.clear();
    m_storedValue = 0.0;
    m_pendingOperator = QChar();
    m_waitingForOperand = true;
    m_justCalculated = false;
    m_error = false;
}

void CalculatorEngine::showError(const QString &message)
{
    m_display = message;
    m_pendingOperator = QChar();
    m_waitingForOperand = true;
    m_justCalculated = false;
    m_error = true;
}

void CalculatorEngine::applyUnaryResult(double value)
{
    m_display = formatNumber(value);
    m_waitingForOperand = false;
    m_justCalculated = false;
    m_error = false;
}

void CalculatorEngine::clear()
{
    reset();
}

void CalculatorEngine::inputDigit(const QChar &digit)
{
    if (!digit.isDigit())
        return;

    if (m_error || m_justCalculated) {
        reset();
    }

    if (m_waitingForOperand) {
        m_display = QString(digit);
        m_waitingForOperand = false;
        m_justCalculated = false;
        return;
    }

    const int digitCount = m_display.count(QRegularExpression(QStringLiteral("[0-9]")));
    if (digitCount >= 14)
        return;

    if (m_display == QStringLiteral("0"))
        m_display = QString(digit);
    else if (m_display == QStringLiteral("-0"))
        m_display = QStringLiteral("-") + QString(digit);
    else
        m_display.append(digit);
}

void CalculatorEngine::inputDecimal()
{
    if (m_error || m_justCalculated)
        reset();

    if (m_waitingForOperand) {
        m_display = QStringLiteral("0.");
        m_waitingForOperand = false;
        m_justCalculated = false;
        return;
    }

    if (!m_display.contains(QLatin1Char('.')))
        m_display.append(QLatin1Char('.'));
}

double CalculatorEngine::currentValue() const
{
    bool ok = false;
    const double value = m_display.toDouble(&ok);
    return ok ? value : 0.0;
}

bool CalculatorEngine::applyPendingOperator()
{
    const double right = currentValue();
    double result = m_storedValue;

    switch (m_pendingOperator.unicode()) {
    case '+':
        result += right;
        break;
    case '-':
        result -= right;
        break;
    case '*':
        result *= right;
        break;
    case '/':
        if (qFuzzyIsNull(right)) {
            m_expression = formatNumber(m_storedValue) + QStringLiteral(" ÷ 0 =");
            showError(QStringLiteral("错误：除数不能为0"));
            return false;
        }
        result /= right;
        break;
    default:
        return false;
    }

    m_display = formatNumber(result);
    m_storedValue = result;
    return true;
}

void CalculatorEngine::inputOperator(const QChar &op)
{
    if (op != QLatin1Char('+') && op != QLatin1Char('-')
        && op != QLatin1Char('*') && op != QLatin1Char('/')) {
        return;
    }

    if (m_error)
        reset();

    if (!m_pendingOperator.isNull() && !m_waitingForOperand) {
        if (!applyPendingOperator())
            return;
        m_expression = m_display + QLatin1Char(' ') + op;
        m_pendingOperator = op;
        m_waitingForOperand = true;
        m_justCalculated = false;
        return;
    }

    if (!m_pendingOperator.isNull()) {
        m_pendingOperator = op;
        m_expression = formatNumber(m_storedValue) + QLatin1Char(' ') + op;
        return;
    }

    m_storedValue = currentValue();
    m_pendingOperator = op;
    m_expression = m_display + QLatin1Char(' ') + op;
    m_waitingForOperand = true;
    m_justCalculated = false;
}

void CalculatorEngine::calculate()
{
    if (m_error || m_pendingOperator.isNull())
        return;

    if (m_waitingForOperand)
        m_display = formatNumber(m_storedValue);

    const QString left = formatNumber(m_storedValue);
    const QString right = m_display;
    const QChar op = m_pendingOperator;
    if (!applyPendingOperator()) {
        return;
    }

    m_expression = left + QLatin1Char(' ') + op + QLatin1Char(' ')
        + right + QStringLiteral(" =");
    m_pendingOperator = QChar();
    m_waitingForOperand = true;
    m_justCalculated = true;
}

void CalculatorEngine::backspace()
{
    if (m_error) {
        reset();
        return;
    }
    if (m_waitingForOperand)
        return;

    m_display.chop(1);
    if (m_display.isEmpty() || m_display == QStringLiteral("-"))
        m_display = QStringLiteral("0");
}

void CalculatorEngine::clearEntry()
{
    if (m_error) {
        reset();
        return;
    }
    m_display = QStringLiteral("0");
    m_waitingForOperand = true;
    m_justCalculated = false;
}

void CalculatorEngine::toggleSign()
{
    if (m_error || m_display == QStringLiteral("0"))
        return;
    if (m_display.startsWith(QLatin1Char('-')))
        m_display.remove(0, 1);
    else
        m_display.prepend(QLatin1Char('-'));
}

void CalculatorEngine::percent()
{
    if (m_error) {
        reset();
        return;
    }
    applyUnaryResult(currentValue() / 100.0);
}

void CalculatorEngine::reciprocal()
{
    if (m_error) {
        reset();
        return;
    }
    const double value = currentValue();
    if (qFuzzyIsNull(value)) {
        showError(QStringLiteral("错误：除数不能为0"));
        return;
    }
    applyUnaryResult(1.0 / value);
}

void CalculatorEngine::square()
{
    if (m_error) {
        reset();
        return;
    }
    applyUnaryResult(currentValue() * currentValue());
}

void CalculatorEngine::squareRoot()
{
    if (m_error) {
        reset();
        return;
    }
    const double value = currentValue();
    if (value < 0.0) {
        showError(QStringLiteral("错误：负数不能开平方"));
        return;
    }
    applyUnaryResult(std::sqrt(value));
}
