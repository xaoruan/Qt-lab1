#include "mainwindow.h"
#include "uifont.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("简单文本编辑器"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));
    QApplication::setOrganizationName(QStringLiteral("Qt 应用程序开发实验二"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/app.svg")));
    applyApplicationFont();

    QFile styleFile(QStringLiteral(":/styles/styles.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        application.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    MainWindow window;
    window.show();
    return application.exec();
}
