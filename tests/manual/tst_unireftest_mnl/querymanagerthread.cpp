#include "querymanagerthread.h"

#include <QDebug>
#include <QCoreApplication>
const int MAX_COUNT = 10;

QueryManagerThread::QueryManagerThread(QSqlDatabase db)
    : ThreadQuery(db)
{
    connect(this, &QueryManagerThread::executeDone,
            this, &QueryManagerThread::directExecuteDone, Qt::DirectConnection);

    connect(this, &QueryManagerThread::changePosition,
            this, &QueryManagerThread::directChangePosition, Qt::DirectConnection);

    connect(this, &QueryManagerThread::value,
            this, &QueryManagerThread::directValue, Qt::DirectConnection);

    m_sql = "SELECT TOP " + QString("%1").arg(MAX_COUNT)
            + " GUID, %1 AS NAME FROM %2 WHERE %3\n";
}

QueryManagerThread::~QueryManagerThread()
{

}

void QueryManagerThread::execQuery(const QString &strQuery)
{
    ThreadQueryItem<ThreadQuery> *query = m_threadPool->acquire();
    if (query == NULL)
        return;
    if (query->isNew()) {

        connect(query, &ThreadQuery::executeDone, [query]()
        {
            query->first();
        });

        connect(query, &ThreadQuery::changePosition, [query](int pos)
        {
            if (pos >= 0) {
                query->fetchOne();
            } else {
                query->release();
            }

        });

        connect(query, &ThreadQuery::value, [query, this](const QSqlRecord &record)
        {
            if (this->m_count < MAX_COUNT) {
                emit result(record.value(1).toString());
                this->m_count++;
                query->next();
            }

            if (this->m_count == MAX_COUNT) {
                this->stopFetch();
                this->m_count++;
            }

        });

        connect(query, &ThreadQuery::error, this, &QueryManagerThread::error);

        connect(this, &QueryManagerThread::stoppedFetch,
                query, &ThreadQuery::stopFetch, Qt::QueuedConnection);
    }
    query->execute(strQuery);
}

void QueryManagerThread::setThreadPool(ThreadQueryPool<ThreadQuery> *threadPool)
{
    m_threadPool = threadPool;
}

void QueryManagerThread::directExecuteDone()
{
    m_fields = "";
    m_expr   = "";
    m_table  = "";
    m_count  = 0;
    this->first();
}

void QueryManagerThread::directValue(const QSqlRecord &record)
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

    this->next();
}

void QueryManagerThread::directChangePosition(int pos)
{
    if (pos >= 0) {
        this->fetchOne();
    } else {
        if (m_table != "" && pos == ThreadQuery::AfterLastRow)
            execQuery(m_sql.arg(m_fields).arg(m_table).arg(m_expr));
        emit stoppedFetch();
    }
}

