#ifndef THREADQUERY_P_H
#define THREADQUERY_P_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace RTPTechGroup {
namespace SqlExtension {

//! Класс обёртка выполнения SQL запросов в отдельном потоке
class ThreadQueryPrivate: public QObject
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit ThreadQueryPrivate(const QString &driverName,
                                const QString &databaseName,
                                const QString &hostName,
                                int port,
                                const QString &userName,
                                const QString &password,
                                const QString &query);
    //! Деструктор класса
    virtual ~ThreadQueryPrivate();

public slots:

// Плдготовка и выполнение запроса
    //! Установка заполнителя
    void bindValue(const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Установка политики точности числа
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);

    //! Установка просмотра записей только вперёд
    void setForwardOnly(bool forward);

    //! Подготовка запроса
    bool prepare(const QString &query);

    //! Выполнение подготовленого запроса
    bool execute();

    //! Выполнение указанного запроса
    bool execute(const QString &query);

    //! Выполнение ранее подготовленного запроса в пакете
    bool executeBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);

// Позиционирование на записи
    //! Перейти к первой записи
    bool first();

    //! Перейти к следующей записи
    bool next();

    //! Перейти к указанной записи
    bool seek(int index, bool relative = false);

    //! Перейти к предыдущнй записи
    bool previous();

    //! Перейти к последней записи
    bool last();

// Получение значений
    //! Вызывает получение всех значений в потоке
    void fetchAll();

    //! Вызывает получение значения из потока
    void fetchOne();

// Окончание выполнения запроса
    //! Окончание выполнеия запроса
    void finish();

    //! Очищает запрос
    void clear();

signals:
    //! Сигнал об окончании выполнения операции
    void executeDone(bool success);

    //! Возращает номер позиции
    void changePosition(int pos);

    //! Возращает ошибку
    void error(QSqlError err);

    //! Возращает все значения из потока
    void values(const QList<QSqlRecord> &records);

    //! Возращает значение из потока
    void value(const QSqlRecord &record);

private:
    //! SQL запрос к БД
    QSqlQuery *m_query;

    //! Соединение с БД
    QString m_connectionName;
};

}}
#endif // THREADQUERY_P_H
