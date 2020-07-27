#ifndef THREADQUERY_H
#define THREADQUERY_H

#include <QtCore/QThread>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QUuid>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "sqlextensionglobal.h"
#include <utils/spinlock.h>

using namespace RTPTechGroup::Utils;

namespace RTPTechGroup {
namespace SqlExtension {

class SQLEXTENSIONLIB ThreadQueryPrivate;

//! Класс предназначенный для выполнения SQL запросов в отдельном потоке
/*! Пример:
\code
    ThreadQuery *threadQuery = new ThreadQuery();

    // Обработка окончания выполнение запроса
    connect(threadQuery, &ThreadQuery::executeDone,
            [=] (const QUuid &queryUuid, const QSqlError &err)
    {
        if (err.isValid())
            return;
        threadQuery->first(queryUuid);
    });

    //! Обработка выбора записи
    connect(threadQuery, &ThreadQuery::changePosition,
            [=] (const QUuid &queryUuid, int pos)
    {
        if (pos >= 0)
            threadQuery->fetchOne(queryUuid);
    });

    // Обработка получения значений
    connect(threadQuery, &ThreadQuery::value,
            [=] (const QUuid &queryUuid, const QSqlRecord &value)
    {
       qDebug() << QString("Value: %1 \n").arg(value.value(0).toString()));
       threadQuery->next(queryUuid);
    });

    // Начало выполнения запроса
    threadQuery->execute("SELECT * FROM TEST");
\endcode
*/
class SQLEXTENSIONLIB ThreadQuery : public QThread
{
    Q_OBJECT

public:
    //! Конструктор класса
    explicit ThreadQuery(const QString &query = QString(),
                         const QSqlDatabase &db = QSqlDatabase::database());

    //! Конструктор класса
    explicit ThreadQuery(const QSqlDatabase &db);

    //! Деструктор класса
    virtual ~ThreadQuery() override;

    enum Location
    {
        BeforeFirstRow = -1, //!< Позиция курсора перед первой строкой
        AfterLastRow   = -2, //!< Позиция курсора после последней строки
        StoppedFetch   = -3  //!< Получение значений прервано
    };

// Общие настройки
     //! Установка политики точности числа
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);

    //! Получение политики точности числа
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy();

    //! Установка просмотра записей только вперёд
    void setForwardOnly(bool forward);

    //! Получение просмотра записей только вперёд
    bool isForwardOnly();

    //! Установка заполнителя
    void bindValue(const QUuid &queryUuid, const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Установка заполнителя
    void bindValue(const QString  &placeholder, const QVariant &val,
                   QSql::ParamType paramType = QSql::In);

    //! Получение заполнителя
    QVariant boundValue(const QString & placeholder);

    //! Установка заполнителей
    QMap<QString, QVariant> boundValues();

// Подготовка и выполнение запроса
    //! Подготовка запроса
    void prepare(const QString &query, const QUuid &queryUuid = QUuid::createUuid());

    //! Выполнение указанного запроса
    void execute(const QString &query, const QUuid &queryUuid = QUuid::createUuid());

    //! Выполнение указанного запроса
    void execute(const char *query, const QUuid &queryUuid = QUuid::createUuid());

    //! Выполнение подготовленного запроса
    void execute(const QUuid &queryUuid = QUuid::createUuid());

    //! Выполнение ранее подготовленного запроса в пакете
    void executeBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows,
                      const QUuid &queryUuid = QUuid::createUuid());

    //! Возвращает текущий запрос
    QString   lastQuery();

    //! Возвращает последнюю ошибку
    QSqlError lastError() const;

    //! Начинает непрерывный блок запросов одного потока
    void begin();

    //! Заканчивает непрерывный блок запросов одного потока
    void end();

// Позиционирование на записи
     //! Перейти к первой записи
    void first(const QUuid &queryUuid = QUuid());

    //! Перейти к следующей записи
    void next(const QUuid &queryUuid  = QUuid());

    //! Перейти к указанной записи
    void seek(const QUuid &queryUuid, int index, bool relative = false);

    //! Перейти к указанной записи
    void seek(int index, bool relative = false);

    //! Перейти к предыдущей записи
    void previous(const QUuid &queryUuid = QUuid());

    //! Перейти к последней записи
    void last(const QUuid &queryUuid = QUuid());

// Получение значений
    //! Вызывает получение всех значений в потоке
    void fetchAll(const QUuid &queryUuid = QUuid());

    //! Вызывает получение значения из потока
    void fetchOne(const QUuid &queryUuid = QUuid());

// Окончание выполнения запроса
    //! Окончание выполнения запроса
    void finish();

    //! Очищает запрос
    void clear();

// Работа с транзакциями
    //! Начало транзакции
    void transaction();

    //! Завершение транзакции
    void commit();

    //! Откат транзакции
    void rollback();

signals:
    //! Сигнал об окончании подготовки запроса
    void prepareDone(const QUuid &queryUuid, QSqlError err);

    //! Сигнал об окончании выполнения операции в потоке
    void executeDone(const QUuid &queryUuid, QSqlError err);

    //! Сигнал изменения номера позиции в потоке
    void changePosition(const QUuid &queryUuid, int pos);

    //! Сигнал ошибки БД в потоке
    void error(const QUuid &queryUuid, QSqlError err);

    //! Сигнал получения значений из потока
    void values(const QUuid &queryUuid, const QList<QSqlRecord> &records);

    //! Сигнал получения значения из потока
    void value(const QUuid &queryUuid, const QSqlRecord &record);

protected:
    //! Выполнение потока
    void run() Q_DECL_OVERRIDE;

private slots:
    //! Изменение номера позиции в потоке
    void pChangePosition(const QUuid &queryUuid, int pos);

    //! Обработка ошибки
    void pError(const QUuid &queryUuid, QSqlError err);

    //! Получение значений из потока
    void pValues(const QUuid &queryUuid,  const QList<QSqlRecord> &records);

    //! Получение значения из потока
    void pValue(const QUuid &queryUuid, const QSqlRecord &record);

private:
    //! Spinlock подготовки запроса
    SpinLock m_spinlock;

    //! Последняя ошибка работы с БД
    QSqlError m_lastError;

    //! Текст SQL запроса к БД
    QString m_queryText;

    //! Имя драйвера БД
    QString m_driverName;

    //! Имя БД
    QString m_databaseName;

    //! Имя хоста БД
    QString m_hostName;

    //! Порт БД
    int m_port;

    //! Логин пользователя БД
    QString m_userName;

    //! Пароль пользователя БД
    QString m_password;

    //! Политика работы с вещественным числом
    QSql::NumericalPrecisionPolicy m_precisionPolicy;

    //!  Просмотра записей только вперёд
    bool m_forwardOnly;

    //! Тип параметров
    QMap<QString, QSql::ParamType> m_boundTypes;

    //! Значение параметров
    QMap<QString, QVariant> m_boundValues;

    //! Обёртка над QSqlQuery
    ThreadQueryPrivate *m_queryPrivate;

    //! Хранит поток с блоком запросов
    QPointer<QThread> m_blockThread;

    //! Идентификатор запроса
    QUuid m_queryUuid;
};

}}

#endif // THREADQUERY_H
