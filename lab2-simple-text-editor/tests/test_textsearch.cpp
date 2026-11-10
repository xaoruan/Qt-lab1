#include "fileservice.h"
#include "textsearch.h"

#include <QTemporaryDir>
#include <QTextDocument>
#include <QtTest>

class TestTextSearch : public QObject
{
    Q_OBJECT

private slots:
    void findsForwardAndWraps();
    void findsBackwardAndWraps();
    void honorsCaseSensitivity();
    void replacesAllWithoutCaseSensitivity();
    void ignoresEmptyQuery();
    void rejectsInvalidUtf8File();
};

void TestTextSearch::findsForwardAndWraps()
{
    const QString text = QStringLiteral("alpha beta alpha");
    TextSearchOptions options;

    const TextMatch middle = TextSearch::find(text, 1, QStringLiteral("alpha"), options);
    QVERIFY(middle.found);
    QCOMPARE(middle.start, 11);
    QVERIFY(!middle.wrapped);

    const TextMatch wrapped = TextSearch::find(text, 12, QStringLiteral("alpha"), options);
    QVERIFY(wrapped.found);
    QCOMPARE(wrapped.start, 0);
    QVERIFY(wrapped.wrapped);
}

void TestTextSearch::findsBackwardAndWraps()
{
    const QString text = QStringLiteral("alpha beta alpha");
    TextSearchOptions options;
    options.backward = true;

    const TextMatch first = TextSearch::find(text, 11, QStringLiteral("alpha"), options);
    QVERIFY(first.found);
    QCOMPARE(first.start, 0);

    const TextMatch wrapped = TextSearch::find(text, 0, QStringLiteral("alpha"), options);
    QVERIFY(wrapped.found);
    QCOMPARE(wrapped.start, 11);
    QVERIFY(wrapped.wrapped);
}

void TestTextSearch::honorsCaseSensitivity()
{
    const QString text = QStringLiteral("Find find FIND");
    TextSearchOptions options;
    options.caseSensitive = true;
    options.wrapAround = false;

    const TextMatch upper = TextSearch::find(text, 0, QStringLiteral("FIND"), options);
    QVERIFY(upper.found);
    QCOMPARE(upper.start, 10);

    options.caseSensitive = false;
    const TextMatch insensitive =
        TextSearch::find(text, 0, QStringLiteral("find"), options);
    QVERIFY(insensitive.found);
    QCOMPARE(insensitive.start, 0);
}

void TestTextSearch::replacesAllWithoutCaseSensitivity()
{
    QTextDocument document;
    document.setPlainText(QStringLiteral("one ONE one"));

    const int count = TextSearch::replaceAll(
        &document, QStringLiteral("one"), QStringLiteral("two"), false);

    QCOMPARE(count, 3);
    QCOMPARE(document.toPlainText(), QStringLiteral("two two two"));
}

void TestTextSearch::ignoresEmptyQuery()
{
    QTextDocument document;
    document.setPlainText(QStringLiteral("content"));
    const int count = TextSearch::replaceAll(
        &document, QString(), QStringLiteral("x"), false);
    QCOMPARE(count, 0);
    QCOMPARE(document.toPlainText(), QStringLiteral("content"));
}

void TestTextSearch::rejectsInvalidUtf8File()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString path =
        temporaryDirectory.filePath(QStringLiteral("invalid.txt"));
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(QByteArray::fromHex("ff"));
    file.close();

    QString text;
    QString error;
    QVERIFY(!FileService::readTextFile(path, &text, &error));
    QVERIFY(!error.isEmpty());
}

QTEST_MAIN(TestTextSearch)

#include "test_textsearch.moc"
