#include <QObject>
#include <QtTest/QtTest>

#include <sqlextension/dsvstream.h>

class tst_DsvStream : public QObject
{
    Q_OBJECT
          
public slots:
    void initTestCase();

private slots:
    void readDelimiter();
    void readQuote();
    void readLine();
    void writeDelimiter();
    void writeQuotes();
    void createLine();
    void writeLine();
    void alwaysQuotes();
};

void tst_DsvStream::initTestCase()
{
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    // Подключение перевода для Qt
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    QTranslator *ts = new QTranslator(this);
    if (ts->load(translatorFileName,
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        QApplication::installTranslator(ts);
    }
}

void tst_DsvStream::readDelimiter()
{
    QByteArray data("1;2.2;\"3\"");
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    QStringList list;
    while (!dsv.atEnd()) {
        list.append(dsv.read());
    }
    buffer.close();
    QCOMPARE(list, QStringList({"1", "2.2", "3"}));
}

void tst_DsvStream::readQuote()
{
    QByteArray data("1;2.2;\"3;54\"");
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    QStringList list;
    while (!dsv.atEnd()) {
        list.append(dsv.read());
    }
    buffer.close();
    QCOMPARE(list, QStringList({"1", "2.2", "3;54"}));
}

void tst_DsvStream::readLine()
{
    QByteArray data("1;2.2;\"3\"\n4;5;6");
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    QCOMPARE(dsv.readLine(), QStringList({"1", "2.2", "3"}));
    QCOMPARE(dsv.readLine(), QStringList({"4", "5", "6"}));

    buffer.close();
}

void tst_DsvStream::writeDelimiter()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    dsv.write(1);
    dsv.write(2);
    dsv.write(3);

    buffer.close();
    QCOMPARE(data, QByteArray("1;2;3"));
}

void tst_DsvStream::writeQuotes()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    dsv.write(1);
    dsv.write(2);
    dsv.write("3;54");

    buffer.close();
    QCOMPARE(data, QByteArray("1;2;\"3;54\""));
}

void tst_DsvStream::createLine()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    dsv.write(1);
    dsv.write(2);
    dsv.writeLine();
    dsv.write(3);

    buffer.close();
    QCOMPARE(data, QByteArray("1;2\n3"));
}

void tst_DsvStream::writeLine()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    dsv.writeLine({1,2,"3"});

    buffer.close();
    QCOMPARE(data, QByteArray("1;2;3\n"));
}

void tst_DsvStream::alwaysQuotes()
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);

    DsvStream dsv(&buffer);
    dsv.setAlwaysQuotes(true);
    dsv.write(1);
    dsv.write(2);
    dsv.write(3);

    buffer.close();
    QCOMPARE(data, QByteArray("\"1\";\"2\";\"3\""));
}

QTEST_MAIN(tst_DsvStream)
#include "tst_dsvstream.moc"

