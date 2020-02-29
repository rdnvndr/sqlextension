#include "querymanagerthread.h"

#include <QDebug>
#include <QCoreApplication>

QueryManagerThread::QueryManagerThread(QSqlDatabase db)
    : ThreadQuery(db)
{
    connect(this, &QueryManagerThread::executeDone,
            this, &QueryManagerThread::directExecuteDone,
            Qt::DirectConnection);

    connect(this, &QueryManagerThread::changePosition,
            this, &QueryManagerThread::directChangePosition,
            Qt::DirectConnection);

    connect(this, &QueryManagerThread::value,
            this, &QueryManagerThread::directValue,
            Qt::DirectConnection);

    m_sql = "SELECT TOP " + QString("%1").arg(MAX_COUNT)
            + " GUID, %1 AS NAME FROM %2 WHERE %3\n";

    m_busyCount = 0;
}

QueryManagerThread::~QueryManagerThread()
{

}

void QueryManagerThread::execQuery(const QString &strQuery)
{
    ThreadQueryItem<Query> *query = m_threadPool->acquire();
    if (query == nullptr)
        return;

    ++m_busyCount;
    query->setQueryManager(this);
    query->execute(strQuery);
}

void QueryManagerThread::setThreadPool(ThreadQueryPool<Query> *threadPool)
{
    m_threadPool = threadPool;
}

void QueryManagerThread::directExecuteDone(const QUuid &queryUuid, const QSqlError &err)
{
    if (err.isValid()) {
        return;
    }

    m_fields = "";
    m_expr   = "";
    m_table  = "";
    m_valueMutex.lock();
    m_count  = 0;
    ++m_busyCount;
    m_valueMutex.unlock();

    this->first(queryUuid);
}

void QueryManagerThread::directValue(const QUuid &queryUuid, const QSqlRecord &record)
{

    QString findString = "'%"
            + record.value(0).toString().simplified().replace(' ', '%') + "%'";

    QString nameTable = record.value("NAMETABLE").toString();
    QString nameField = record.value("NAMEFIELD").toString();

    if (nameField != "SCREEN_NAME" && !nameField.isEmpty()) {
        if (m_table != nameTable) {
            if (m_table != "")
                execQuery(m_sql.arg(m_fields).arg(m_table).arg(m_expr));
            m_fields = "CAST(" + nameField + " AS varchar(250))";
            m_expr = nameField + " like " + findString;
            m_table = nameTable;
        } else {
            m_fields += "+ ' ' + CAST(" + nameField + " AS varchar(250))";
            m_expr += " or " + nameField + " like " + findString;
        }
    }

    this->next(queryUuid);
}

void QueryManagerThread::directChangePosition(QUuid queryUuid, int pos)
{
    if (pos >= 0) {
        this->fetchOne(queryUuid);
    } else if (pos == ThreadQuery::StoppedFetch) {
        releaseQuery();
    } else {
        if (m_table != "" && pos == ThreadQuery::AfterLastRow) {
            execQuery(m_sql.arg(m_fields).arg(m_table).arg(m_expr));
        }
        this->finish();
    }
}

void QueryManagerThread::queryValue(const QUuid &queryUuid, const QSqlRecord &record)
{
    Q_UNUSED(queryUuid);
    Q_ASSERT(this->m_count <= MAX_COUNT+1);

    m_valueMutex.lock();
    if (this->m_count < MAX_COUNT) {
        this->m_count++;
        m_valueMutex.unlock();
        emit result(record.value(1).toString());
    } else if (this->m_count == MAX_COUNT) {
        this->m_count++;
        this->finish();
        m_valueMutex.unlock();
        emit stoppedFetch();
    } else {
        m_valueMutex.unlock();
    }
}

void QueryManagerThread::releaseQuery()
{
    Q_ASSERT(m_busyCount > 0);

    auto query = dynamic_cast<Query *>(QObject::sender());
    if (query)
        query->releaseQuery();

    --m_busyCount;
    if (m_busyCount == 0) {
        dynamic_cast<ThreadQueryItem<QueryManagerThread> *>(this)->release();
    }

}


