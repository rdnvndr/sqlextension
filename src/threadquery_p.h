#ifndef THREADQUERY_P_H
#define THREADQUERY_P_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <atomic>

#include "sqlextensionglobal.h"

namespace RTPTechGroup {
namespace SqlExtension {

//! Класс обёртка выполнения SQL запросов в отдельном потоке
class SQLEXTENSIONLIB ThreadQueryPrivate: public QObject
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit ThreadQueryPrivate();

    //! Деструктор класса
    virtual ~ThreadQueryPrivate();

    //! Установка значения текущего идентификатора запроса
    void setQueryUuid(const QUuid &queryUuid);

public slots:
    //! Устанавливает соединение
    void databaseConnect(const QString &driverName, const QString &databaseName,
                 const QString &hostName, int port, const QString &userName,
                 const QString &password, const QString &sql);

// Подготовка и выполнение запроса
    //! Установка заполнителя
    void bindValue(const QUuid &queryUuid, const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Установка политики точности числа
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);

    //! Установка просмотра записей только вперёд
    void setForwardOnly(bool forward);

    //! Подготовка запроса
    bool prepare(const QUuid &queryUuid, const QString &sql);

    //! Выполнение подготовленного запроса
    bool execute(const QUuid &queryUuid);

    //! Выполнение указанного запроса
    bool execute(const QUuid &queryUuid, const QString &sql);

    //! Выполнение ранее подготовленного запроса в пакете
    bool executeBatch(const QUuid &queryUuid,
                      QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);

// Позиционирование на записи
    //! Перейти к первой записи
    bool first(const QUuid &queryUuid);

    //! Перейти к следующей записи
    bool next(const QUuid &queryUuid);

    //! Перейти к указанной записи
    bool seek(const QUuid &queryUuid, int index, bool relative = false);

    //! Перейти к предыдущей записи
    bool previous(const QUuid &queryUuid);

    //! Перейти к последней записи
    bool last(const QUuid &queryUuid);

// Получение значений
    //! Вызывает получение всех значений в потоке
    void fetchAll(const QUuid &queryUuid);

    //! Вызывает получение значения из потока
    void fetchOne(const QUuid &queryUuid);

// Окончание выполнения запроса
    //! Окончание выполнения запроса
    void finish(const QUuid &queryUuid);

    //! Очищает запрос
    void clear(const QUuid &queryUuid);

// Работа с транзакциями
    //! Начало транзакции
    bool transaction();

    //! Завершение транзакции
    bool commit();

    //! Откат транзакции
    bool rollback();

signals:
    //! Сигнал об окончании подготовки запроса
    void prepareDone(const QUuid &queryUuid, QSqlError err);

    //! Сигнал об окончании выполнения операции
    void executeDone(const QUuid &queryUuid, QSqlError err);

    //! Возвращает номер позиции
    void changePosition(const QUuid &queryUuid, int pos);

    //! Возвращает ошибку в БД
    void error(const QUuid &queryUuid, QSqlError err);

    //! Возвращает все значения из потока
    void values(const QUuid &queryUuid, const QList<QSqlRecord> &records);

    //! Возвращает значение из потока
    void value(const QUuid &queryUuid, const QSqlRecord &record);

public:
    //! Идентификатор окончания запроса
    static const QUuid FINISH_UUID;

private:
    //! Возвращает SQL запрос к БД
    QSqlQuery *pquery() noexcept {
        return reinterpret_cast<QSqlQuery*>(&m_query);
    }

    //! SQL запрос к БД
    std::aligned_storage_t<sizeof(QSqlQuery), alignof(QSqlQuery)> m_query;

    //! Соединение с БД
    QString m_connectionName;

    //! Идентификатор запроса
    std::atomic<QUuid> m_queryUuid;
};

}}
#endif // THREADQUERY_P_H
