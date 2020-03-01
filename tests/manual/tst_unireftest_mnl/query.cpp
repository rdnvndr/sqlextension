#include "query.h"

#include "querymanagerthread.h"

Query::Query(QSqlDatabase db): ThreadQuery(db)
{
    connect(this, &ThreadQuery::changePosition, this, &Query::directChangePosition,
            Qt::DirectConnection);
    connect(this, &ThreadQuery::executeDone,
            this, [=] (const QUuid &queryUuid, const QSqlError &err) {
                if (err.isValid())
                    this->finish();
                else
                    this->first(queryUuid);
            },
            Qt::DirectConnection
    );
}

void Query::setQueryManager(QueryManagerThread *manager)
{
    Q_ASSERT(manager != nullptr);

    m_valueConn = connect(this, &ThreadQuery::value,
                          manager, &QueryManagerThread::queryValue);
    m_errorConn = connect(this, &ThreadQuery::error,
                          manager, &QueryManagerThread::error);
    m_stopConn  = connect(manager, &QueryManagerThread::stoppedFetch,
                          this, &ThreadQuery::finish, Qt::DirectConnection);
    m_releaseConn = connect(this, &Query::releasingQuery,
                            manager, &QueryManagerThread::releaseQuery);
}

void Query::releaseQuery()
{
    QObject::disconnect(m_stopConn);
    QObject::disconnect(m_valueConn);
    QObject::disconnect(m_errorConn);
    QObject::disconnect(m_releaseConn);

    auto threadQuery = dynamic_cast<ThreadQueryItem<Query> *>(this);
    if (threadQuery)
        threadQuery->release();
}

void Query::directChangePosition(const QUuid &queryUuid, int pos)
{
    if (pos >= 0) {
        this->fetchOne(queryUuid);
        this->next(queryUuid);
    } else if (pos == ThreadQuery::StoppedFetch) {
        emit releasingQuery();
    } else {
        this->finish();
    }
}
