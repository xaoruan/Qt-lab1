#include "textsearch.h"

#include <QTextCursor>
#include <QTextDocument>
#include <Qt>

#include <algorithm>

TextMatch TextSearch::find(
    const QString &text,
    int fromPosition,
    const QString &query,
    const TextSearchOptions &options)
{
    TextMatch match;
    if (query.isEmpty() || text.isEmpty()) {
        return match;
    }

    const Qt::CaseSensitivity sensitivity =
        options.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    fromPosition = std::clamp(fromPosition, 0, static_cast<int>(text.size()));

    if (!options.backward) {
        int position = text.indexOf(query, fromPosition, sensitivity);
        if (position >= 0) {
            match.found = true;
            match.start = position;
            match.length = query.size();
            return match;
        }
        if (options.wrapAround && fromPosition > 0) {
            position = text.indexOf(query, 0, sensitivity);
            if (position >= 0) {
                match.found = true;
                match.wrapped = true;
                match.start = position;
                match.length = query.size();
            }
        }
        return match;
    }

    int position = fromPosition > 0
        ? text.lastIndexOf(query, fromPosition - 1, sensitivity)
        : -1;
    if (position >= 0) {
        match.found = true;
        match.start = position;
        match.length = query.size();
        return match;
    }
    if (options.wrapAround && fromPosition < text.size()) {
        position = text.lastIndexOf(query, text.size() - 1, sensitivity);
        if (position >= 0) {
            match.found = true;
            match.wrapped = true;
            match.start = position;
            match.length = query.size();
        }
    }
    return match;
}

TextMatch TextSearch::find(
    QTextDocument *document,
    int fromPosition,
    const QString &query,
    const TextSearchOptions &options)
{
    if (!document) {
        return {};
    }
    return find(document->toPlainText(), fromPosition, query, options);
}

int TextSearch::replaceAll(
    QTextDocument *document,
    const QString &query,
    const QString &replacement,
    bool caseSensitive)
{
    if (!document || query.isEmpty()) {
        return 0;
    }

    const QTextDocument::FindFlags searchFlags =
        caseSensitive ? QTextDocument::FindCaseSensitively
                      : QTextDocument::FindFlags();

    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::Start);
    QTextCursor match = document->find(query, cursor, searchFlags);
    if (match.isNull()) {
        return 0;
    }

    int count = 0;
    cursor.beginEditBlock();
    while (!match.isNull()) {
        match.insertText(replacement);
        ++count;
        match = document->find(query, match, searchFlags);
    }
    cursor.endEditBlock();
    return count;
}
