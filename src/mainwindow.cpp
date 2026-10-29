#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFont>
#include <QKeyEvent>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

    ui->display->setReadOnly(true);
    ui->display->setFocusPolicy(Qt::NoFocus);
    for (QPushButton *button : findChildren<QPushButton *>())
        button->setFocusPolicy(Qt::NoFocus);

    bindButtons();
    updateView();
    setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bindButtons()
{
    connect(ui->btn0, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("0")); });
    connect(ui->btn1, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("1")); });
    connect(ui->btn2, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("2")); });
    connect(ui->btn3, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("3")); });
    connect(ui->btn4, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("4")); });
    connect(ui->btn5, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("5")); });
    connect(ui->btn6, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("6")); });
    connect(ui->btn7, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("7")); });
    connect(ui->btn8, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("8")); });
    connect(ui->btn9, &QPushButton::clicked, this, [this] { handleDigit(QStringLiteral("9")); });
    connect(ui->btnDecimal, &QPushButton::clicked, this, &MainWindow::handleDecimal);
    connect(ui->btnAdd, &QPushButton::clicked, this, [this] { handleOperator(QLatin1Char('+')); });
    connect(ui->btnSubtract, &QPushButton::clicked, this, [this] { handleOperator(QLatin1Char('-')); });
    connect(ui->btnMultiply, &QPushButton::clicked, this, [this] { handleOperator(QLatin1Char('*')); });
    connect(ui->btnDivide, &QPushButton::clicked, this, [this] { handleOperator(QLatin1Char('/')); });
    connect(ui->btnEquals, &QPushButton::clicked, this, &MainWindow::handleCalculate);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &MainWindow::handleBackspace);
    connect(ui->btnClearEntry, &QPushButton::clicked, this, &MainWindow::handleClearEntry);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::handleClear);
    connect(ui->btnPercent, &QPushButton::clicked, this, &MainWindow::handlePercent);
    connect(ui->btnReciprocal, &QPushButton::clicked, this, &MainWindow::handleReciprocal);
    connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::handleSquare);
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::handleSquareRoot);
    connect(ui->btnSign, &QPushButton::clicked, this, &MainWindow::handleToggleSign);
}

void MainWindow::handleDigit(const QString &digit)
{
    m_engine.inputDigit(digit.front());
    updateView();
}

void MainWindow::handleDecimal()
{
    m_engine.inputDecimal();
    updateView();
}

void MainWindow::handleOperator(const QChar &op)
{
    m_engine.inputOperator(op);
    updateView();
}

void MainWindow::handleCalculate()
{
    m_engine.calculate();
    updateView();
}

void MainWindow::handleBackspace()
{
    m_engine.backspace();
    updateView();
}

void MainWindow::handleClearEntry()
{
    m_engine.clearEntry();
    updateView();
}

void MainWindow::handleClear()
{
    m_engine.clear();
    updateView();
}

void MainWindow::handlePercent()
{
    m_engine.percent();
    updateView();
}

void MainWindow::handleReciprocal()
{
    m_engine.reciprocal();
    updateView();
}

void MainWindow::handleSquare()
{
    m_engine.square();
    updateView();
}

void MainWindow::handleSquareRoot()
{
    m_engine.squareRoot();
    updateView();
}

void MainWindow::handleToggleSign()
{
    m_engine.toggleSign();
    updateView();
}

void MainWindow::updateView()
{
    ui->display->setText(m_engine.displayText());
    ui->lblExpression->setText(m_engine.expressionText());
    ui->display->setProperty("error", m_engine.isError());
    ui->display->style()->unpolish(ui->display);
    ui->display->style()->polish(ui->display);

    QFont font = ui->display->font();
    if (m_engine.isError())
        font.setPixelSize(22);
    else if (m_engine.displayText().size() > 14)
        font.setPixelSize(32);
    else
        font.setPixelSize(44);
    ui->display->setFont(font);
}

void MainWindow::processKeyText(const QString &text)
{
    if (text.size() != 1)
        return;

    const QChar key = text.front();
    if (key.isDigit())
        handleDigit(QString(key));
    else if (key == QLatin1Char('.'))
        handleDecimal();
    else if (key == QLatin1Char('+') || key == QLatin1Char('-')
             || key == QLatin1Char('*') || key == QLatin1Char('/'))
        handleOperator(key);
    else if (key == QLatin1Char('%'))
        handlePercent();
    else if (key == QLatin1Char('='))
        handleCalculate();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        handleCalculate();
        event->accept();
        return;
    case Qt::Key_Backspace:
        handleBackspace();
        event->accept();
        return;
    case Qt::Key_Escape:
    case Qt::Key_Delete:
        handleClear();
        event->accept();
        return;
    case Qt::Key_Percent:
        handlePercent();
        event->accept();
        return;
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        handleDigit(event->text());
        event->accept();
        return;
    case Qt::Key_Period:
        handleDecimal();
        event->accept();
        return;
    case Qt::Key_Plus:
        handleOperator(QLatin1Char('+'));
        event->accept();
        return;
    case Qt::Key_Minus:
        handleOperator(QLatin1Char('-'));
        event->accept();
        return;
    case Qt::Key_Asterisk:
        handleOperator(QLatin1Char('*'));
        event->accept();
        return;
    case Qt::Key_Slash:
        handleOperator(QLatin1Char('/'));
        event->accept();
        return;
    case Qt::Key_Equal:
        handleCalculate();
        event->accept();
        return;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::applyCaptureScenario(const QString &scenario)
{
    m_engine.clear();
    if (scenario == QStringLiteral("decimal")) {
        for (const QChar ch : QStringLiteral("12.5+7.5")) {
            if (ch.isDigit())
                m_engine.inputDigit(ch);
            else if (ch == QLatin1Char('.'))
                m_engine.inputDecimal();
            else
                m_engine.inputOperator(ch);
        }
        m_engine.calculate();
    } else if (scenario == QStringLiteral("error")) {
        for (const QChar ch : QStringLiteral("8/0")) {
            if (ch.isDigit())
                m_engine.inputDigit(ch);
            else
                m_engine.inputOperator(ch);
        }
        m_engine.calculate();
    } else if (scenario == QStringLiteral("continued")) {
        for (const QChar ch : QStringLiteral("2+3*4")) {
            if (ch.isDigit())
                m_engine.inputDigit(ch);
            else
                m_engine.inputOperator(ch);
        }
        m_engine.calculate();
    }
    updateView();
}
