#include "threadqueryitem.h"

#include <QVariant>

namespace RTPTechGroup {
namespace SqlExtension {

ThreadQueryItem::ThreadQueryItem(ThreadQuery *query, ThreadQueryPool *pool)
    : QObject(query)
{
    m_query = query;
    m_pool  = pool;

    connect(m_query,  &ThreadQuery::executeDone,
            this, &ThreadQueryItem::executeDone);
    connect(m_query,  &ThreadQuery::changePosition,
            this, &ThreadQueryItem::changePosition);
    connect(m_query,  &ThreadQuery::error,
            this, &ThreadQueryItem::error);
    connect(m_query,  &ThreadQuery::values,
            this, &ThreadQueryItem::values);
    connect(m_query,  &ThreadQuery::value,
            this, &ThreadQueryItem::value);
}

ThreadQueryItem::~ThreadQueryItem()
{
    m_pool->freeThreadQuery(m_query);
}

void ThreadQueryItem::setNumericalPrecisionPolicy(
        QSql::NumericalPrecisionPolicy precisionPolicy)
{
    m_query->setNumericalPrecisionPolicy(precisionPolicy);
}

QSql::NumericalPrecisionPolicy ThreadQueryItem::numericalPrecisionPolicy()
{
    return m_query->numericalPrecisionPolicy();
}

void ThreadQueryItem::setForwardOnly(bool forward)
{
    m_query->setForwardOnly(forward);
}

bool ThreadQueryItem::isForwardOnly()
{
    return m_query->isForwardOnly();
}

void ThreadQueryItem::bindValue(const QString &placeholder, const QVariant &val,
                                QSql::ParamType paramType)
{
    m_query->bindValue(placeholder, val, paramType);
}

QVariant ThreadQueryItem::boundValue(const QString &placeholder)
{
    return m_query->boundValue(placeholder);
}

QMap<QString, QVariant> ThreadQueryItem::boundValues()
{
    return m_query->boundValues();
}

void ThreadQueryItem::prepare(const QString &query)
{
    m_query->prepare(query);
}

void ThreadQueryItem::execute(const QString &query)
{
    m_query->execute(query);
}

void ThreadQueryItem::execute()
{
    m_query->execute();
}

void ThreadQueryItem::executeBatch(QSqlQuery::BatchExecutionMode mode)
{
    m_query->executeBatch(mode);
}

QString ThreadQueryItem::lastQuery()
{
    return m_query->lastQuery();
}

QSqlError ThreadQueryItem::lastError() const
{
    return m_query->lastError();
}

void ThreadQueryItem::first()
{
    m_query->first();
}

void ThreadQueryItem::next()
{
    m_query->next();
}

void ThreadQueryItem::seek(int index, bool relative)
{
    m_query->seek(index, relative);
}

void ThreadQueryItem::previous()
{
    m_query->previous();
}

void ThreadQueryItem::last()
{
    m_query->last();
}

void ThreadQueryItem::fetchAll()
{
    m_query->fetchAll();
}

void ThreadQueryItem::stopFetchAll()
{
    m_query->stopFetchAll();
}

void ThreadQueryItem::fetchOne()
{
    m_query->fetchOne();
}

void ThreadQueryItem::finish()
{
    m_query->finish();
}

void ThreadQueryItem::clear()
{
    m_query->clear();
}

void ThreadQueryItem::transaction()
{
    m_query->transaction();
}

void ThreadQueryItem::commit()
{
    m_query->commit();
}

void ThreadQueryItem::rollback()
{
    m_query->rollback();
}

}}
