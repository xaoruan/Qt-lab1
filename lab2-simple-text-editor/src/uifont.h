#pragma once

#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QStringList>

inline void applyApplicationFont()
{
#ifdef Q_OS_WIN
    const QStringList candidates = {
        QStringLiteral("C:/Windows/Fonts/simhei.ttf"),
        QStringLiteral("C:/Windows/Fonts/Deng.ttf"),
        QStringLiteral("C:/Windows/Fonts/msyh.ttc"),
        QStringLiteral("C:/Windows/Fonts/NotoSansSC-VF.ttf")
    };
    for (const QString &fontPath : candidates) {
        const int fontId = QFontDatabase::addApplicationFont(fontPath);
        if (fontId >= 0) {
            const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
            if (!families.isEmpty()) {
                QApplication::setFont(QFont(families.constFirst(), 9));
                return;
            }
        }
    }
    QApplication::setFont(QFont(QStringLiteral("Microsoft YaHei UI"), 9));
#endif
}
