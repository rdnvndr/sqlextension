#include "threadquery.h"

#include <QtCore/QMutexLocker>
#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <QMetaObject>

#include "threadquery_p.h"
#include "clogging.h"

namespace RTPTechGroup {
namespace SqlExtension {

ThreadQuery::ThreadQuery(const QString &query, const QSqlDatabase &db): QThread()
{
    m_driverName = db.driverName();
    m_databaseName = db.databaseName();
    m_hostName = db.hostName();
    m_port = db.port();
    m_userName = db.userName();
    m_password = db.password();
    m_precisionPolicy = db.numericalPrecisionPolicy();
    m_forwardOnly = false;
    m_queryText = query;
    m_stopFetch = true;
    m_blockThread = nullptr;
    m_mutex.lock();

    this->start();
}

ThreadQuery::ThreadQuery(const QSqlDatabase &db): QThread()
{
    m_driverName = db.driverName();
    m_databaseName = db.databaseName();
    m_hostName = db.hostName();
    m_port = db.port();
    m_userName = db.userName();
    m_password = db.password();
    m_precisionPolicy = db.numericalPrecisionPolicy();
    m_forwardOnly = false;
    m_queryText = "";
    m_stopFetch = true;
    m_blockThread = nullptr;
    m_mutex.lock();

    this->start();
}

ThreadQuery::~ThreadQuery()
{
    m_stopFetch = true;
    QCoreApplication::removePostedEvents(m_queryPrivate);
    QMetaObject::invokeMethod(m_queryPrivate, "deleteLater",
                              Qt::BlockingQueuedConnection);
    this->quit();
    this->wait();
}

void ThreadQuery::setNumericalPrecisionPolicy(
        QSql::NumericalPrecisionPolicy precisionPolicy)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_precisionPolicy = precisionPolicy;
    QMetaObject::invokeMethod(
                m_queryPrivate, "setNumericalPrecisionPolicy", Qt::QueuedConnection,
                Q_ARG( QSql::NumericalPrecisionPolicy, precisionPolicy));
}

QSql::NumericalPrecisionPolicy ThreadQuery::numericalPrecisionPolicy()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    return m_precisionPolicy;
}

void ThreadQuery::setForwardOnly(bool forward)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_forwardOnly = forward;
    QMetaObject::invokeMethod(
                m_queryPrivate, "setForwardOnly", Qt::QueuedConnection,
                Q_ARG(bool, m_forwardOnly));
}

bool ThreadQuery::isForwardOnly()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    return m_forwardOnly;
}

void ThreadQuery::bindValue(const QString &placeholder, const QVariant &val,
                            QSql::ParamType paramType)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_boundValues.insert(placeholder, val);
    m_boundTypes.insert(placeholder, paramType);

    QMetaObject::invokeMethod(
                m_queryPrivate, "bindValue", Qt::QueuedConnection,
                Q_ARG(QString, placeholder),
                Q_ARG(QVariant, val),
                Q_ARG(QSql::ParamType, paramType));
}

QVariant ThreadQuery::boundValue(const QString &placeholder)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    return m_boundValues.value(placeholder);
}

QMap<QString, QVariant> ThreadQuery::boundValues()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    return m_boundValues;
}

void ThreadQuery::prepare(const QString &query)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_boundTypes.clear();
    m_boundValues.clear();
    m_queryText = query;
    QMetaObject::invokeMethod(m_queryPrivate, "prepare", Qt::QueuedConnection,
                              Q_ARG(QString, m_queryText));
}

QUuid ThreadQuery::execute(const QString &query)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_queryUuid = QUuid::createUuid();
    m_queryText = query;
    QMetaObject::invokeMethod(m_queryPrivate, "execute", Qt::QueuedConnection,
                              Q_ARG(QUuid, m_queryText),
                              Q_ARG(QString, m_queryText));

    return m_queryUuid;
}

QUuid ThreadQuery::execute()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_queryUuid = QUuid::createUuid();
    QMetaObject::invokeMethod(m_queryPrivate, "execute", Qt::QueuedConnection,
                              Q_ARG(QUuid, m_queryText));

    return m_queryUuid;
}

QUuid ThreadQuery::executeBatch(QSqlQuery::BatchExecutionMode mode)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_queryUuid = QUuid::createUuid();
    QMetaObject::invokeMethod(m_queryPrivate, "executeBatch", Qt::QueuedConnection,
                              Q_ARG(QUuid, m_queryText),
                              Q_ARG(QSqlQuery::BatchExecutionMode, mode));

    return m_queryUuid;
}

QString ThreadQuery::lastQuery()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    return m_queryText;
}

QSqlError ThreadQuery::lastError() const
{
    return m_lastError;
}

void ThreadQuery::begin()
{
    m_mutex.lock();
    m_blockThread = QThread::currentThread();
}

void ThreadQuery::end()
{
    m_mutex.unlock();
    m_blockThread = nullptr;
}

void ThreadQuery::first(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "first", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::next(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "next", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::seek(const QUuid &queryUuid, int index, bool relative)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "seek", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid),
                              Q_ARG(int, index), Q_ARG(bool, relative));
}

void ThreadQuery::seek(int index, bool relative)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "seek", Qt::QueuedConnection,
                              Q_ARG(QUuid, QUuid()),
                              Q_ARG(int, index), Q_ARG(bool, relative));
}

void ThreadQuery::previous(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "previous", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::last(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "last", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::fetchAll(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "fetchAll", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::stopFetch()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_stopFetch = true;

    emit changePosition(m_queryUuid, ThreadQuery::StoppedFetch);
}

void ThreadQuery::fetchOne(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "fetchOne", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::finish(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "finish", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::clear(const QUuid &queryUuid)
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    m_boundTypes.clear();
    m_boundValues.clear();
    QMetaObject::invokeMethod(m_queryPrivate, "clear", Qt::QueuedConnection,
                              Q_ARG(QUuid, queryUuid));
}

void ThreadQuery::transaction()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "transaction", Qt::QueuedConnection);
}

void ThreadQuery::commit()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "commit", Qt::QueuedConnection);
}

void ThreadQuery::rollback()
{
    QMutexLocker locker((m_blockThread != QThread::currentThread()) ? &m_mutex : nullptr);
    QMetaObject::invokeMethod(m_queryPrivate, "rollback", Qt::QueuedConnection);
}

void ThreadQuery::run()
{
    m_queryPrivate =  new ThreadQueryPrivate();
    m_queryPrivate->setStopFetch(&m_stopFetch);

    qRegisterMetaType<QSql::ParamType>( "QSql::ParamType" );

    connect(m_queryPrivate, &ThreadQueryPrivate::prepareDone,
            this, &ThreadQuery::prepareDone, Qt::DirectConnection);

    connect(m_queryPrivate, &ThreadQueryPrivate::executeDone,
            this, &ThreadQuery::executeDone, Qt::DirectConnection);

    connect(m_queryPrivate, &ThreadQueryPrivate::changePosition,
            this, &ThreadQuery::pChangePosition, Qt::DirectConnection);

    qRegisterMetaType< QSqlError >( "QSqlError" );
    connect(m_queryPrivate, &ThreadQueryPrivate::error,
            this, &ThreadQuery::pError, Qt::DirectConnection);

    qRegisterMetaType< QList<QSqlRecord> >( "QList<QSqlRecord>" );
    connect(m_queryPrivate, &ThreadQueryPrivate::values,
            this, &ThreadQuery::pValues, Qt::DirectConnection);

    qRegisterMetaType< QSqlRecord >( "QSqlRecord" );
    connect(m_queryPrivate, &ThreadQueryPrivate::value,
            this, &ThreadQuery::pValue, Qt::DirectConnection);

    QMetaObject::invokeMethod(
                m_queryPrivate, "databaseConnect", Qt::QueuedConnection,
                Q_ARG(QString, m_driverName), Q_ARG(QString, m_databaseName),
                Q_ARG(QString, m_hostName),   Q_ARG(int, m_port),
                Q_ARG(QString, m_userName),   Q_ARG(QString, m_password),
                Q_ARG(QString, m_queryText));

    m_mutex.unlock();

    exec();
}

void ThreadQuery::pChangePosition(const QUuid &queryUuid, int pos)
{
    if (m_stopFetch)
        return;

    emit changePosition(queryUuid, pos);
}

void ThreadQuery::pError(const QUuid &queryUuid, QSqlError err)
{
    m_lastError = err;
    emit error(queryUuid, err);

    qCWarning(lcSqlExtension) << err.text();
}

void ThreadQuery::pValues(const QUuid &queryUuid, const QList<QSqlRecord> &records)
{
    if (m_stopFetch)
        return;

    emit values(queryUuid, records);
}

void ThreadQuery::pValue(const QUuid &queryUuid, const QSqlRecord &record)
{
    if (m_stopFetch)
        return;

    emit value(queryUuid, record);
}

}}
