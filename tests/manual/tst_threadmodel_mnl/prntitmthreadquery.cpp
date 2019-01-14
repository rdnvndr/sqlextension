#include "prntitmthreadquery.h"

PrntItmThreadQuery::PrntItmThreadQuery(const QString &query, const QSqlDatabase &db)
    : ThreadQuery(query, db)
{
    m_parentItem = nullptr;
}

PrntItmThreadQuery::PrntItmThreadQuery(const QSqlDatabase &db)
    :ThreadQuery (db)
{

}

TreeItem *PrntItmThreadQuery::parentItem() const
{
    return m_parentItem;
}

void PrntItmThreadQuery::setParentItem(TreeItem *parentItem)
{
    m_parentItem = parentItem;
}
