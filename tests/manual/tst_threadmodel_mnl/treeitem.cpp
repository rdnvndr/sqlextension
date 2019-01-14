#include <QStringList>
#include "treeitem.h"

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
    m_locked  = false;
    m_fetched = false;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if (m_locked && column == 0)
        return m_itemData.value(column).toString() + " (Загрузка...)";

    return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

bool TreeItem::isLocked() const
{
    return m_locked;
}

void TreeItem::setLocked(bool lock)
{
    m_locked = lock;
}

bool TreeItem::isFetched() const
{
    return m_fetched;
}

void TreeItem::setFetched(bool fetch)
{
    m_fetched = fetch;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
