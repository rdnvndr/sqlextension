#ifndef THREADQUERY_H
#define THREADQUERY_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QUuid>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "sqlextensionglobal.h"

namespace RTPTechGroup {
namespace SqlExtension {

class SQLEXTENSIONLIB ThreadQueryPrivate;

//! Класс предназначенный для выполнения SQL запросов в отдельном потоке
/*! Пример:
\code
    ThreadQuery *threadQuery = new ThreadQuery();

    // Обработка окончания выполнение запроса
    connect(threadQuery, &ThreadQuery::executeDone, [=] (bool success) {
        threadQuery->first();
    });

    //! Обработка выбора записи
    connect(threadQuery, &ThreadQuery::changePosition, [=] (int pos) {
        if (pos >= 0)
            threadQuery->fetchOne();
    });

    // Обработка получения значений
    connect(threadQuery, &ThreadQuery::value, [=] (const QSqlRecord &value)
    {
       qDebug() << QString("Value: %1 \n").arg(value.value(0).toString()));
       threadQuery->next();
    });

    // Начало выполнения запроса
    threadQuery->prepare("SELECT * FROM TEST");
    threadQuery->execute();
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
    virtual ~ThreadQuery();

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
    void bindValue(const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Получение заполнителя
    QVariant boundValue(const QString & placeholder);

    //! Установка заполнителей
    QMap<QString, QVariant> boundValues();

// Подготовка и выполнение запроса
    //! Подготовка запроса
    void prepare(const QString &query);

    //! Выполнение указанного запроса
    QUuid execute(const QString &query);

    //! Выполнение подготовленного запроса
    QUuid execute();

    //! Выполнение ранее подготовленного запроса в пакете
    QUuid executeBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);

    //! Возвращает текущий запрос
    QString   lastQuery();

    //! Возвращает последнюю ошибку
    QSqlError lastError() const;

    //! Начинает блок запросов
    void begin();

    //! Заканчивает блок запросов
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

    //! Останавливает получение значений в потоке
    void stopFetch();

// Окончание выполнения запроса
    //! Окончание выполнения запроса
    void finish(const QUuid &queryUuid = QUuid());

    //! Очищает запрос
    void clear(const QUuid &queryUuid = QUuid());

// Работа с транзакциями
    //! Начало транзакции
    void transaction();

    //! Завершение транзакции
    void commit();

    //! Откат транзакции
    void rollback();

signals:
    //! Сигнал об окончании подготовки запроса
    void prepareDone();

    //! Сигнал об окончании выполнения операции в потоке
    void executeDone(const QUuid &queryUuid);

    //! Сигнал изменения номера позиции в потоке
    void changePosition(const QUuid &queryUuid, int pos);

    //! Сигнал ошибки в потоке
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
    //! Мьютекс подготовки запроса
    QMutex m_mutex;

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

    //! Флаг остановки получения значений
    volatile bool m_stopFetch;

    //! Хранит поток с блоком запросов
    QPointer<QThread> m_blockThread;

    //! Идентификатор запроса
    QUuid m_queryUuid;
};

}}

#endif // THREADQUERY_H
