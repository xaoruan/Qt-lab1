#pragma once

#include <QString>

class FileService
{
public:
    static bool readTextFile(const QString &filePath, QString *text, QString *errorMessage);
    static bool writeTextFile(const QString &filePath, const QString &text, QString *errorMessage);
    static QString displayName(const QString &filePath);
};
