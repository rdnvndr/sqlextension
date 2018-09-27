#ifndef QUERY_H
#define QUERY_H

#include <sqlextension/threadquery.h>

using namespace RTPTechGroup::SqlExtension;

class QueryManagerThread;

//! Класс поиска значений в таблицах УТС
class Query : public ThreadQuery
{
    Q_OBJECT

public:
    //! Конструктор класса
    Query(QSqlDatabase db);

    //! Устанавливает менеджера запросов
    void setQueryManager(QueryManagerThread *manager);

public slots:
    //! Обрабатывает изменение позиции
    void directChangePosition(const QUuid &queryUuid, int pos);

signals:
    //! Сигнал остановки выполнения запроса
    void stoppedFetch();

private:
    //! Соединение возврата значения в менеджер запроса
    QMetaObject::Connection m_valueConn;

    //! Соединение остановки выполнения
    QMetaObject::Connection m_stopConn;

    //! Соединение получения ошибки
    QMetaObject::Connection m_errorConn;
};

#endif // QUERY_H
