#pragma once

#include <QMainWindow>

#include "calculator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void applyCaptureScenario(const QString &scenario);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void bindButtons();
    void handleDigit(const QString &digit);
    void handleDecimal();
    void handleOperator(const QChar &op);
    void handleCalculate();
    void handleBackspace();
    void handleClearEntry();
    void handleClear();
    void handlePercent();
    void handleReciprocal();
    void handleSquare();
    void handleSquareRoot();
    void handleToggleSign();
    void updateView();
    void processKeyText(const QString &text);

    Ui::MainWindow *ui;
    CalculatorEngine m_engine;
};
