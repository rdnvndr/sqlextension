#include "dsvstream.h"

#include <QtCore/QBuffer>
#include <QtCore/QVariant>

DsvStream::DsvStream(QIODevice *device)
{
    m_device = device;
    m_quote = '"';
    m_delimiter = ';';
    m_state = LineStart;
    m_alwaysQuotes = false;
}


DsvStream::DsvStream(QByteArray *array, QIODevice::OpenMode openMode)
{
    QBuffer buffer(array);
    buffer.open(openMode);
    m_device = &buffer;

    m_quote = '"';
    m_delimiter = ';';
    m_state = LineStart;
    m_alwaysQuotes = false;
}

DsvStream::~DsvStream()
{

}

QChar DsvStream::delimiter() const
{
    return m_delimiter;
}

void DsvStream::setDelimiter(QChar delimiter)
{
    m_delimiter = delimiter;
}

QChar DsvStream::quote() const
{
    return m_quote;
}

void DsvStream::setQuote(QChar quote)
{
    m_quote = quote;
}

bool DsvStream::atEnd() const
{
    return m_state == End;
}

bool DsvStream::atLineEnd() const
{
    return m_state == LineEnd;
}

QStringList DsvStream::readLine()
{
    QStringList line;

    while(m_state != End) {
        m_state = step();
        if (m_state == FieldEnd) {
            m_state = step();
            line.append(QString(m_field));
        }
        if (m_state == LineEnd) {
            break;
        }
    }

    return line;
}

QString DsvStream::read()
{
    while(m_state != End) {
        m_state = step();
        if (m_state == FieldEnd) {
            m_state = step();
            return QString(m_field);
        }
    }

    return QString();
}

bool DsvStream::writeLine(const QList<QVariant> &values)
{
    foreach (const QVariant &value, values) {
        if (!write(value))
            return false;
    }

    if (!writeLine())
        return false;

    return true;
}

bool DsvStream::writeLine()
{
    char ch = '\n';
    if (m_state != End) {
        m_state = LineEnd;
    }
    return m_device->write(&ch,1);
}

bool DsvStream::write(const QVariant &value)
{
    QString field = value.toString();

    if (needQuotes(field)) {
        field = quoteString(field);
    }

    char ch = 0;
    if (m_device->seek(m_device->pos() - 1)) {
        m_device->getChar(&ch);
        if (ch != '\r' && ch != '\n')
            field = m_delimiter + field;
    }
    if (m_state == LineStart)
        m_state = LineEnd;

    return m_device->write(field.toUtf8());
}

DsvStream::State DsvStream::step()
{
    char ch = 0;
       switch (m_state) {
       case LineStart:
           return FieldStart;

       case FieldStart:
           m_field.clear();
           if(m_device->peek(&ch, 1) < 1) {
               return FieldEnd;
           }
           if(ch == m_quote) {
               m_device->getChar(&ch);
               return FieldQuoted;
           }
           return FieldRegular;

       case FieldRegular:
           if(m_device->peek(&ch, 1) < 1 ||
                   ch == '\r' || ch == '\n' || ch == m_delimiter) {
               return FieldEnd;
           }
           m_device->getChar(&ch);
           m_field.push_back(ch);
           return FieldRegular;

       case FieldQuoted:
           if(!m_device->getChar(&ch)) {
               return FieldEnd;
           }
           if(ch == m_quote) {
               ch = 0;
               if(m_device->peek(&ch, 1) < 1 ||
                       ch == '\r' || ch == '\n' || ch == m_delimiter ) {
                   return FieldEnd;
               }
               if(ch == m_quote) {
                   m_device->getChar(&ch);
                   m_field.push_back(ch);
                   return FieldQuoted;
               }
           }
           m_field.push_back(ch);
           return FieldQuoted;

       case FieldEnd:
           if (m_device->peek(&ch, 1) < 1) {
               return End;
           }
           if (ch == '\r' || ch == '\n') {
               return LineEnd;
           }
           m_device->getChar(&ch);
           return FieldStart;

       case LineEnd:
           if(m_device->getChar(&ch)) {
               if(ch == '\r') {
                   ch = 0;
                   if(m_device->peek(&ch, 1) < 1) {
                       return End;
                   }
               }
               return LineStart;
           }

       }

       return End;
}

bool DsvStream::needQuotes(const QString &field)
{
    if(m_alwaysQuotes)
        return true;
    foreach (const QChar &ch, field) {
        if(ch == m_delimiter || ch == m_quote || ch == '\r' || ch == '\n') {
            return true;
        }
    }
    return false;
}

QString DsvStream::quoteString(const QString &in)
{
    QString out;
    out.push_back(m_quote);
    foreach (QChar ch, in) {
        if(ch == m_quote) {
            out.push_back(m_quote);
        }
        out.push_back(ch);
    }
    out.push_back(m_quote);
    return out;
}

bool DsvStream::alwaysQuotes() const
{
    return m_alwaysQuotes;
}

void DsvStream::setAlwaysQuotes(bool alwaysQuotes)
{
    m_alwaysQuotes = alwaysQuotes;
}
