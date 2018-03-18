#include "threadquery_p.h"

#include <QtCore/QThread>
#include <QtCore/QCoreApplication>
#include <QtSql/QSqlRecord>

namespace RTPTechGroup {
namespace SqlExtension {

ThreadQueryPrivate::ThreadQueryPrivate()
{
    m_stopFetch = NULL;
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
        emit error(db.lastError());
        emit executeDone(false);
    }

    if (query.isEmpty())
        m_query = new QSqlQuery(db);
    else
        m_query = new QSqlQuery(query, db);
}

void ThreadQueryPrivate::bindValue(const QString &placeholder,
                                   const QVariant &val,
                                   QSql::ParamType paramType)
{
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

bool ThreadQueryPrivate::prepare(const QString &query)
{
    bool ret = m_query->prepare(query);
    if (!ret) {
        emit error(m_query->lastError());
        emit executeDone(ret);
    }
    return ret;
}

bool ThreadQueryPrivate::execute()
{
    QCoreApplication::removePostedEvents(this);
    bool ret = m_query->exec();
    if (!ret) emit error(m_query->lastError());

    *m_stopFetch = false;

    emit executeDone(ret);
    return ret;
}

bool ThreadQueryPrivate::execute(const QString &query)
{
    QCoreApplication::removePostedEvents(this);
    bool ret = m_query->exec(query);
    if (!ret) emit error(m_query->lastError());

    *m_stopFetch = false;

    emit executeDone(ret);
    return ret;
}

bool ThreadQueryPrivate::executeBatch(QSqlQuery::BatchExecutionMode mode)
{
    QCoreApplication::removePostedEvents(this);
    bool ret = m_query->execBatch(mode);

    *m_stopFetch = false;

    emit executeDone(ret);
    return ret;
}

bool ThreadQueryPrivate::first()
{
    if (*m_stopFetch) {
        return false;
    }

    bool ret = m_query->first();
    emit changePosition(m_query->at());
    return ret;
}

bool ThreadQueryPrivate::next()
{
    if (*m_stopFetch)
        return false;

    bool ret = m_query->next();
    emit changePosition(m_query->at());
    return ret;
}

bool ThreadQueryPrivate::seek(int index, bool relative)
{
    if (*m_stopFetch)
        return false;

    bool ret = m_query->seek(index, relative);
    emit changePosition(m_query->at());
    return ret;
}

bool ThreadQueryPrivate::previous()
{
    if (*m_stopFetch)
        return false;

    bool ret = m_query->previous();
    emit changePosition(m_query->at());
    return ret;
}

bool ThreadQueryPrivate::last()
{
    if (*m_stopFetch)
        return false;

    bool ret = m_query->last();
    emit changePosition(m_query->at());
    return ret;
}

void ThreadQueryPrivate::fetchAll()
{
    QList<QSqlRecord> records;
    while (m_query->next())
    {
        if (*m_stopFetch)
            break;

        records.append(m_query->record());
    }
    emit values(records);
}

void ThreadQueryPrivate::fetchOne()
{
    if (*m_stopFetch)
        return;

    emit value(m_query->record());
}

void ThreadQueryPrivate::finish()
{
    m_query->finish();
}

void ThreadQueryPrivate::clear()
{
    m_query->clear();
}

bool ThreadQueryPrivate::transaction()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.transaction();
    if (!ret) emit error(db.lastError());

    return ret;
}

bool ThreadQueryPrivate::commit()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.commit();
    if (!ret) emit error(db.lastError());

    return ret;
}

bool ThreadQueryPrivate::rollback()
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    bool ret = db.rollback();
    if (!ret) emit error(db.lastError());

    return ret;
}

void ThreadQueryPrivate::setStopFetch(volatile bool *stopFetch)
{
    m_stopFetch = stopFetch;
}

}}
