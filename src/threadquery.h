#ifndef THREADQUERY_H
#define THREADQUERY_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QMap>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "sqlextensionglobal.h"

namespace RTPTechGroup {
namespace SqlExtension {

class ThreadQueryPrivate;
typedef void ( *ThreadQueryFunction )(QSqlQuery *query);

//! Класс предназначенный для выполнения SQL запросов в отдельном потоке
/*! Пример:
\code
    ThreadQuery *threadQuery = new ThreadQuery();

    //! Обработка окончания выполнение запроса
    connect(threadQuery, &ThreadQuery::executeDone, [=] (bool success) {
        if (success) {
            threadQuery->first();
        }
    });

    //! Обработка выбора записи
    connect(threadQuery, &ThreadQuery::changePosition, [=] (int pos) {
        if (pos >= 0)
            threadQuery->fetchOne();
    });

    //! Обработка получения значений
    connect(threadQuery, &ThreadQuery::value, [=] (const QSqlRecord &value)
    {
       qDebug() << QString("Value: %1 \n").arg(value.value(0).toString()));
       threadQuery->next();
    });

    //! Начало выполнения запроса
    threadQuery->prepare("SELECT * FROM TEST");
    threadQuery->execute();
\endcode
*/
class SQLEXTENSIONLIB ThreadQuery : public QThread
{
    Q_OBJECT

public:
    //! Конструктор класса
    explicit ThreadQuery(const QString & query = QString(),
                         QSqlDatabase db = QSqlDatabase::database(),
                         ThreadQueryFunction func = NULL);

    //! Конструктор класса
    explicit ThreadQuery(QSqlDatabase db, ThreadQueryFunction func = NULL);

    //! Конструктор класса
    explicit ThreadQuery(ThreadQueryFunction func);

    //! Деструктор класса
    virtual ~ThreadQuery();

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
    void execute(const QString &query);

    //! Выполнение подготовленного запроса
    void execute();

    //! Выполнение ранее подготовленного запроса в пакете
    void executeBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);

    //! Возвращает текущий запрос
    QString   lastQuery();

    //! Возвращает последнюю ошибку
    QSqlError lastError() const;

// Позиционирование на записи
     //! Перейти к первой записи
    void first();

    //! Перейти к следующей записи
    void next();

    //! Перейти к указанной записи
    void seek(int index, bool relative = false);

    //! Перейти к предыдущей записи
    void previous();

    //! Перейти к последней записи
    void last();

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
    void transaction();

    //! Завершение транзакции
    void commit();

    //! Откат транзакции
    void rollback();

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

protected:
    //! Выполнение потока
    void run() Q_DECL_OVERRIDE;

private slots:
    //! Обработка окончания выполнения операции
    void pExecuteDone(bool success);

    //! Обработка изменения позиции
    void pChangePosition(int pos);

    //! Обработка ошибки
    void pError(QSqlError err);

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

    //! Функция обработки окончания выполнения запроса
    ThreadQueryFunction m_executeDoneFunc;
};

}}

#endif // THREADQUERY_H
