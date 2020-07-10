#include "threadquery_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVariant>
#include <QtCore/QThread>

#include <QtSql/QSqlRecord>

namespace RTPTechGroup {
namespace SqlExtension {

const QUuid ThreadQueryPrivate::FINISH_UUID("{64418950-771a-4e53-b6cb-154b5200565d}");

ThreadQueryPrivate::ThreadQueryPrivate()
{

}

ThreadQueryPrivate::~ThreadQueryPrivate()
{
    delete m_query;
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
}

void ThreadQueryPrivate::databaseConnect(
        const QString &driverName, const QString &databaseName,
        const QString &hostName, int port, const QString &userName,
        const QString &password, const QString &query)
{
    QThread* curThread = QThread::currentThread();
    m_connectionName = QString("RTP0x%1").arg(
                reinterpret_cast<qlonglong>(curThread), 0, 16);
    QSqlDatabase db = QSqlDatabase::addDatabase(driverName, m_connectionName);
    db.setDatabaseName(databaseName);
    db.setHostName(hostName);
    db.setPort(port);
    db.setUserName(userName);
    db.setPassword(password);
    if (!db.open()) {
        emit error(QUuid(), db.lastError());
    } else {
        m_query = (query.isEmpty())
                ? new QSqlQuery(db) : new QSqlQuery(query, db);
    }
}

void ThreadQueryPrivate::bindValue(const QUuid &queryUuid, const QString &placeholder,
                                   const QVariant &val, QSql::ParamType paramType)
{
    if (m_queryUuid != queryUuid)
        return;
    m_query->bindValue(placeholder, val, paramType);
}

void ThreadQueryPrivate::setNumericalPrecisionPolicy(
        QSql::NumericalPrecisionPolicy precisionPolicy)
{
    m_query->setNumericalPrecisionPolicy(precisionPolicy);
}

void ThreadQueryPrivate::setForwardOnly(bool forward)
{
    m_query->setForwardOnly(forward);
}

bool ThreadQueryPrivate::prepare(const QUuid &queryUuid, const QString &query)
{
    m_uuidMutex.lock();
    m_queryUuid = queryUuid;
    m_uuidMutex.unlock();
    bool ret = m_query->prepare(query);

    emit prepareDone(queryUuid,
                     (!ret) ? m_query->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::execute(const QUuid &queryUuid)
{
    m_uuidMutex.lock();
    m_queryUuid = queryUuid;
    m_uuidMutex.unlock();
    bool ret = m_query->exec();

    emit executeDone(queryUuid, (!ret) ? m_query->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::execute(const QUuid &queryUuid, const QString &query)
{
    m_uuidMutex.lock();
    m_queryUuid = queryUuid;
    m_uuidMutex.unlock();
    bool ret = m_query->exec(query);

    emit executeDone(queryUuid, (!ret) ? m_query->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::executeBatch(const QUuid &queryUuid,
                                      QSqlQuery::BatchExecutionMode mode)
{
    m_uuidMutex.lock();
    m_queryUuid = queryUuid;
    m_uuidMutex.unlock();
    bool ret = m_query->execBatch(mode);

    emit executeDone(queryUuid, (!ret) ? m_query->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::first(const QUuid &queryUuid)
{
    if (m_queryUuid != queryUuid)
        return false;

    bool ret = m_query->first();
    emit changePosition(queryUuid, m_query->at());
    return ret;
}

bool ThreadQueryPrivate::next(const QUuid &queryUuid)
{
    if (m_queryUuid != queryUuid)
        return false;

    bool ret = m_query->next();
    emit changePosition(queryUuid, m_query->at());
    return ret;
}

bool ThreadQueryPrivate::seek(const QUuid &queryUuid, int index, bool relative)
{
    if (m_queryUuid != queryUuid)
        return false;

    bool ret = m_query->seek(index, relative);
    emit changePosition(queryUuid, m_query->at());
    return ret;
}

bool ThreadQueryPrivate::previous(const QUuid &queryUuid)
{
    if (m_queryUuid != queryUuid)
        return false;

    bool ret = m_query->previous();
    emit changePosition(queryUuid, m_query->at());
    return ret;
}

bool ThreadQueryPrivate::last(const QUuid &queryUuid)
{
    if (m_queryUuid != queryUuid)
        return false;

    bool ret = m_query->last();
    emit changePosition(queryUuid, m_query->at());
    return ret;
}

void ThreadQueryPrivate::fetchAll(const QUuid &queryUuid)
{
    QList<QSqlRecord> records;
    while (m_query->next())
    {
        if (m_queryUuid != queryUuid)
            return;

        records.append(m_query->record());
    }
    emit values(queryUuid, records);
}

void ThreadQueryPrivate::fetchOne(const QUuid &queryUuid)
{
    if (m_queryUuid != queryUuid)
        return;

    emit value(queryUuid, m_query->record());
}

void ThreadQueryPrivate::finish(const QUuid &queryUuid)
{
    m_query->finish();
    emit changePosition(queryUuid, -3);
}

void ThreadQueryPrivate::clear(const QUuid &queryUuid)
{
    m_query->clear();

    if (queryUuid != FINISH_UUID)
        emit changePosition(queryUuid, -3);
}

bool ThreadQueryPrivate::transaction()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.transaction();
    if (!ret) emit error(QUuid(), db.lastError());

    return ret;
}

bool ThreadQueryPrivate::commit()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.commit();
    if (!ret) emit error(QUuid(), db.lastError());

    return ret;
}

bool ThreadQueryPrivate::rollback()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.rollback();
    if (!ret) emit error(QUuid(), db.lastError());

    return ret;
}

void ThreadQueryPrivate::setQueryUuid(const QUuid &queryUuid)
{
    m_uuidMutex.lock();
    m_queryUuid = queryUuid;
    m_uuidMutex.unlock();
}

}}
