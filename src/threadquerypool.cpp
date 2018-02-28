#include "threadquerypool.h"

#include <QSharedPointer>

namespace RTPTechGroup {
namespace SqlExtension {

ThreadQueryPool::ThreadQueryPool(QObject *parent, QSqlDatabase db)
    : QObject(parent)
{
    m_driverName = db.driverName();
    m_databaseName = db.databaseName();
    m_hostName = db.hostName();
    m_port = db.port();
    m_userName = db.userName();
    m_password = db.password();
}

ThreadQueryPool::ThreadQueryPool(QSqlDatabase db)
    : QObject()
{
    m_driverName = db.driverName();
    m_databaseName = db.databaseName();
    m_hostName = db.hostName();
    m_port = db.port();
    m_userName = db.userName();
    m_password = db.password();
}

ThreadQueryItem *ThreadQueryPool::threadQuery()
{
    ThreadQuery *query;
    if (!m_freeQueue.isEmpty()) {
        query = m_freeQueue.dequeue();
    } else {
        QSqlDatabase db = QSqlDatabase::addDatabase(m_driverName);
        db.setDatabaseName(m_databaseName);
        db.setHostName(m_hostName);
        db.setPort(m_port);
        db.setUserName(m_userName);
        db.setPassword(m_password);
        query = new ThreadQuery(db);
    }

    return new ThreadQueryItem(query, this);
}

void ThreadQueryPool::freeThreadQuery(ThreadQuery *query)
{
    query->clear();
    m_freeQueue.enqueue(query);
}

}}
