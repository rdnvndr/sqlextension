#include "query.h"

#include "querymanagerthread.h"

Query::Query(QSqlDatabase db): ThreadQuery(db)
{
    connect(this, &ThreadQuery::changePosition,
            this, &Query::directChangePosition);
    connect(this, &ThreadQuery::executeDone,
            this, &ThreadQuery::first);
}

void Query::setQueryManager(QueryManagerThread *manager)
{
    m_valueConn = connect(this, &ThreadQuery::value,
                          manager, &QueryManagerThread::queryValue);
    m_errorConn = connect(this, &ThreadQuery::error,
                           manager, &QueryManagerThread::error);
    m_stopConn = connect(manager, &QueryManagerThread::stoppedFetch,
                         this, &ThreadQuery::finish, Qt::QueuedConnection);
}

void Query::directChangePosition(const QUuid &queryUuid, int pos)
{
    if (pos >= 0) {
        this->fetchOne(queryUuid);
    } else  if (pos == ThreadQuery::StoppedFetch) {
        QObject::disconnect(m_stopConn);
        QObject::disconnect(m_valueConn);
        QObject::disconnect(m_errorConn);
        emit stoppedFetch();
    } else
        this->finish();
}
