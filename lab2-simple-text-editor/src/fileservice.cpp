#include "fileservice.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTextStream>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringDecoder>
#else
#include <QTextCodec>
#endif

bool FileService::readTextFile(
    const QString &filePath, QString *text, QString *errorMessage)
{
    const QFileInfo info(filePath);
    if (!info.exists()) {
        if (errorMessage) {
            *errorMessage = QObject::tr("文件不存在：%1").arg(filePath);
        }
        return false;
    }
    if (!info.isFile()) {
        if (errorMessage) {
            *errorMessage = QObject::tr("所选路径不是普通文件：%1").arg(filePath);
        }
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    const QByteArray bytes = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QByteArray payload = bytes;
    QString content;
    if (payload.startsWith("\xEF\xBB\xBF")) {
        payload.remove(0, 3);
    }
    if (payload.startsWith("\xFF\xFE") || payload.startsWith("\xFE\xFF")) {
        const bool littleEndian = payload.startsWith("\xFF\xFE");
        const int length = (payload.size() - 2) / 2;
        const auto *data =
            reinterpret_cast<const uchar *>(payload.constData() + 2);
        content.reserve(length);
        for (int index = 0; index < length; ++index) {
            const uchar first = data[index * 2];
            const uchar second = data[index * 2 + 1];
            const ushort codeUnit = littleEndian
                ? static_cast<ushort>(first | (second << 8))
                : static_cast<ushort>((first << 8) | second);
            content.append(QChar(codeUnit));
        }
    } else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QStringDecoder decoder(QStringDecoder::Utf8);
        content = decoder.decode(payload);
        if (decoder.hasError()) {
            if (errorMessage) {
                *errorMessage = QObject::tr("文件不是有效的 UTF-8 或 UTF-16 文本。");
            }
            return false;
        }
#else
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::ConverterState state;
        content = codec->toUnicode(payload.constData(), payload.size(), &state);
        if (state.invalidChars > 0) {
            if (errorMessage) {
                *errorMessage = QObject::tr("文件不是有效的 UTF-8 或 UTF-16 文本。");
            }
            return false;
        }
#endif
    }

    if (text) {
        *text = content;
    }
    return true;
}

bool FileService::writeTextFile(
    const QString &filePath, const QString &text, QString *errorMessage)
{
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 3)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    stream << text;
    stream.flush();
    if (stream.status() != QTextStream::Ok || !file.commit()) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }
    return true;
}

QString FileService::displayName(const QString &filePath)
{
    return filePath.isEmpty() ? QObject::tr("新建文本文件")
                              : QFileInfo(filePath).fileName();
}
