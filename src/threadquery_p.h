#ifndef THREADQUERY_P_H
#define THREADQUERY_P_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QMutex>

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

    //! Устанавливает ссылку на флаг остановки получения значений
    void setStopFetchFlag(bool *flag, QMutex *mutex);

public slots:

// Подготовка и выполнение запроса
    //! Установка заполнителя
    void bindValue(const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Установка политики точности числа
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);

    //! Установка просмотра записей только вперёд
    void setForwardOnly(bool forward);

    //! Подготовка запроса
    bool prepare(const QString &query);

    //! Выполнение подготовленного запроса
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

    //! Перейти к предыдущей записи
    bool previous();

    //! Перейти к последней записи
    bool last();

// Получение значений
    //! Вызывает получение всех значений в потоке
    void fetchAll();

    //! Вызывает получение значения из потока
    void fetchOne();

// Окончание выполнения запроса
    //! Окончание выполнения запроса
    void finish();

    //! Очищает запрос
    void clear();

// Работа с транзакциями
    //! Начало транзакции
    bool transaction();

    //! Завершение транзакции
    bool commit();

    //! Откат транзакции
    bool rollback();

signals:
    //! Сигнал об окончании выполнения операции
    void executeDone(bool success);

    //! Возвращает номер позиции
    void changePosition(int pos);

    //! Возвращает ошибку
    void error(QSqlError err);

    //! Возвращает все значения из потока
    void values(const QList<QSqlRecord> &records);

    //! Возвращает значение из потока
    void value(const QSqlRecord &record);

private:
    //! SQL запрос к БД
    QSqlQuery *m_query;

    //! Соединение с БД
    QString m_connectionName;

    //! Флаг остановки получения значений
    bool *m_stopFetch;

    //! Мьютекс остановки получения значений
    QMutex *m_stopFetchMutex;
};

}}
#endif // THREADQUERY_P_H
