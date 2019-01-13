#ifndef QUERYMANAGERTHREAD_H
#define QUERYMANAGERTHREAD_H

#include <QtCore/QReadWriteLock>

#include <sqlextension/threadquery.h>
#include <sqlextension/threadquerypool.h>

#include "query.h"

using namespace RTPTechGroup::SqlExtension;

//! Класс получения таблиц класса УТС и списка полей с экранными именами
class QueryManagerThread : public ThreadQuery
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit QueryManagerThread(QSqlDatabase db);

    //! Деструктор класса
    virtual ~QueryManagerThread();

    //! Выполнение подзапроса
    void execQuery(const QString &strQuery);

    //! Устанавливает пул запросов
    void setThreadPool(ThreadQueryPool<Query> *threadPool);

public slots:
    //! Обработка окончания выполнения операции внутри потока
    void directExecuteDone(const QUuid &queryUuid);

    //! Обработка получения значения внутри потока
    void directValue(const QUuid &queyUuid, const QSqlRecord &record);

    //! Обработка получения смены позиции внутри потока
    void directChangePosition(QUuid queryUuid, int pos);

    //! Обрабатывает получение значений
    void queryValue(const QUuid &queryUuid, const QSqlRecord &record);

signals:
    //! Сигнал о получении значения
    void result(const QString& r);

    //! Сигнал остановки
    void stoppedFetch();

private:
    //! Пул запросов
    ThreadQueryPool<Query> *m_threadPool;

    //! Sql запрос
    QString m_sql;

    //! Список полей Sql запроса
    QString m_fields;

    //! Условие Sql запроса
    QString m_expr;

    //! Таблица Sql запроса
    QString m_table;

    //! Количество результатов
    int m_count;

    //! Защищает количество результатов
    QMutex m_valueMutex;
};

#endif // QUERYMANAGERTHREAD_H
