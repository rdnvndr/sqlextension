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
    pquery()->~QSqlQuery();
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
}

void ThreadQueryPrivate::databaseConnect(
        const QString &driverName, const QString &databaseName,
        const QString &hostName, int port, const QString &userName,
        const QString &password, const QString &sql)
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
        if (sql.isEmpty())
            new (pquery()) QSqlQuery(db);
        else
            new (pquery()) QSqlQuery(sql, db);
    }
}

void ThreadQueryPrivate::bindValue(const QUuid &queryUuid, const QString &placeholder,
                                   const QVariant &val, QSql::ParamType paramType)
{
    if (queryUuid != m_queryUuid)
        return;
    pquery()->bindValue(placeholder, val, paramType);
}

void ThreadQueryPrivate::setNumericalPrecisionPolicy(
        QSql::NumericalPrecisionPolicy precisionPolicy)
{
    pquery()->setNumericalPrecisionPolicy(precisionPolicy);
}

void ThreadQueryPrivate::setForwardOnly(bool forward)
{
    pquery()->setForwardOnly(forward);
}

bool ThreadQueryPrivate::prepare(const QUuid &queryUuid, const QString &sql)
{
    m_queryUuid = queryUuid;
    bool ret = pquery()->prepare(sql);

    emit prepareDone(queryUuid,
                     (!ret) ? pquery()->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::execute(const QUuid &queryUuid)
{
    m_queryUuid = queryUuid;
    bool ret = pquery()->exec();

    emit executeDone(queryUuid, (!ret) ? pquery()->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::execute(const QUuid &queryUuid, const QString &sql)
{
    m_queryUuid = queryUuid;
    bool ret = pquery()->exec(sql);

    emit executeDone(queryUuid, (!ret) ? pquery()->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::executeBatch(const QUuid &queryUuid,
                                      QSqlQuery::BatchExecutionMode mode)
{
    m_queryUuid = queryUuid;
    bool ret = pquery()->execBatch(mode);

    emit executeDone(queryUuid, (!ret) ? pquery()->lastError() : QSqlError());

    return ret;
}

bool ThreadQueryPrivate::first(const QUuid &queryUuid)
{
    if (queryUuid != m_queryUuid)
        return false;

    bool ret = pquery()->first();
    emit changePosition(queryUuid, pquery()->at());
    return ret;
}

bool ThreadQueryPrivate::next(const QUuid &queryUuid)
{
    if (queryUuid != m_queryUuid)
        return false;

    bool ret = pquery()->next();
    emit changePosition(queryUuid, pquery()->at());
    return ret;
}

bool ThreadQueryPrivate::seek(const QUuid &queryUuid, int index, bool relative)
{
    if (queryUuid != m_queryUuid)
        return false;

    bool ret = pquery()->seek(index, relative);
    emit changePosition(queryUuid, pquery()->at());
    return ret;
}

bool ThreadQueryPrivate::previous(const QUuid &queryUuid)
{
    if (queryUuid != m_queryUuid)
        return false;

    bool ret = pquery()->previous();
    emit changePosition(queryUuid, pquery()->at());
    return ret;
}

bool ThreadQueryPrivate::last(const QUuid &queryUuid)
{
    if (queryUuid != m_queryUuid)
        return false;

    bool ret = pquery()->last();
    emit changePosition(queryUuid, pquery()->at());
    return ret;
}

void ThreadQueryPrivate::fetchAll(const QUuid &queryUuid)
{
    QList<QSqlRecord> records;
    while (pquery()->next())
    {
        if (queryUuid != m_queryUuid)
            return;

        records.append(pquery()->record());
    }
    emit values(queryUuid, records);
}

void ThreadQueryPrivate::fetchOne(const QUuid &queryUuid)
{
    if (queryUuid != m_queryUuid)
        return;

    emit value(queryUuid, pquery()->record());
}

void ThreadQueryPrivate::finish(const QUuid &queryUuid)
{
    pquery()->finish();
    emit changePosition(queryUuid, -3);
}

void ThreadQueryPrivate::clear(const QUuid &queryUuid)
{
    pquery()->clear();

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
    m_queryUuid = queryUuid;
}

}}
