#ifndef DSVSTREAM_H
#define DSVSTREAM_H

#include <QtCore/QIODevice>

//! Класс DsvStream для работы с данными в формате DSV (delimiter-separated values)
/*! Данный класс позволяет читать или записывать данные в формате DSV с
    возможностью указания разделителя полей и кавычек.\n
    Пример:\n
      \code
        file = new QFile("test.csv");
        file->open(QIODevice::ReadWrite);
        DsvStream *dsv = new DsvStream(file);
        QStringList list;
        while (!dsv->atEnd()) {
            list.append(dsv->read());
            if (dsv->atLineEnd())
                list.clear();
        }
        delete dsv;
        file->close();
        delete file;
      \endcode
*/
class DsvStream
{
    enum State {
        LineStart,
        FieldStart,
        FieldRegular,
        FieldQuoted,
        FieldEnd,
        LineEnd,
        End
    };

public:
    //! Конструктор класса
    explicit DsvStream(QIODevice *device);

    //! Конструктор класса
    explicit DsvStream(QByteArray *array,
                       QIODevice::OpenMode openMode = QIODevice::ReadWrite);

    //! Деструктор класса
    virtual ~DsvStream();


    //! Возвращает символ разделителя столбцов
    QChar delimiter() const;

    //! Устанавливает символ разделителя столбцов
    void setDelimiter(QChar delimiter);

    //! Возвращает символ кавычки
    QChar quote() const;

    //! Устанавливает символ кавычки
    void setQuote(QChar quote);


    //! Возвращает true, если данных больше нет
    bool atEnd() const;

    //! Возвращает true, если конец строки
    bool atLineEnd() const;

    //! Возвращает строку значений
    QStringList readLine();

    //! Возвращает значение
    QString read();

    //! Записывает строку значений
    bool writeLine(const QList<QVariant> &values);

    //! Вставляет новую строку
    bool writeLine();

    //! Записывает значение
    bool write(const QVariant &value);

    //! Возвращает использовать ли кавычки всегда или при необходимости
    bool alwaysQuotes() const;

    //! Устанавливает использование кавычек всегда или при необходимости
    void setAlwaysQuotes(bool alwaysQuotes);

private:
    //! Шаг чтения данных
    DsvStream::State step();

    //! Возвращает необходимо ли использовать кавычки
    bool needQuotes(const QString &field);

    //! Вставляет кавычки в строку
    QString quoteString(const QString &in);

    //! Символ разделителя столбцов
    QChar m_delimiter;

    //! Символ кавычки
    QChar m_quote;

    //! Флаг использования кавычек, всегда или при необходимости
    bool m_alwaysQuotes;

    //! Состояние
    State m_state;

    //! Источник данных
    QIODevice *m_device;

    //! Считанное поле
    QByteArray m_field;

};

#endif // DSVSTREAM_H
