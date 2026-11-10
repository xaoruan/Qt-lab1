#pragma once

#include <QString>

class QTextCursor;
class QTextDocument;

struct TextSearchOptions
{
    bool caseSensitive = false;
    bool backward = false;
    bool wrapAround = true;
};

struct TextMatch
{
    bool found = false;
    bool wrapped = false;
    int start = -1;
    int length = 0;
};

class TextSearch
{
public:
    static TextMatch find(
        const QString &text,
        int fromPosition,
        const QString &query,
        const TextSearchOptions &options);
    static TextMatch find(
        QTextDocument *document,
        int fromPosition,
        const QString &query,
        const TextSearchOptions &options);
    static int replaceAll(
        QTextDocument *document,
        const QString &query,
        const QString &replacement,
        bool caseSensitive);
};
