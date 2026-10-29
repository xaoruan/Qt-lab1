#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("带键盘事件的计算器"));

    QFile styleFile(QStringLiteral(":/styles.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    MainWindow window;
    window.show();

    const QString scenario = qEnvironmentVariable("CALC_CAPTURE_SCENARIO");
    const QString capturePath = qEnvironmentVariable("CALC_CAPTURE_PATH");
    if (!scenario.isEmpty() && !capturePath.isEmpty()) {
        QTimer::singleShot(250, &window, [&window, scenario, capturePath] {
            window.applyCaptureScenario(scenario);
            window.repaint();
            QTimer::singleShot(100, &window, [&window, capturePath] {
                window.grab().save(capturePath);
                qApp->quit();
            });
        });
    }

    return app.exec();
}
