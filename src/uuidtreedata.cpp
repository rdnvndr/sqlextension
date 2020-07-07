#include "uuidtreedata.h"

namespace RTPTechGroup {
namespace SqlExtension {

UuidTreeData::UuidTreeData(QObject *parent, int maxCost)
    : QObject(parent)
{
    m_dataCache.setMaxCost(maxCost);
}

void UuidTreeData::setFields(const QStringList &dataFields)
{
    m_dataNames = dataFields;
}

QStringList UuidTreeData::fields() const
{
    return m_dataNames;
}

UuidTreeData::~UuidTreeData()
{
    qDeleteAll(m_nodes);
}

UuidTreeNode* UuidTreeData::node(QString id) const
{
    UuidTreeNode* n = m_nodes[id];
    if (n) return n;

    n = createNode(id);

    m_nodes.insert(id, n);
    return n;
}

QVariant UuidTreeData::data(QString id, int column) const
{
    UuidTreeNode* n = node(id);
    if (!n)
        return QVariant();

    if (!m_dataCache.contains(n))
        if (!refreshData(n))
            return QVariant();

    return (*m_dataCache[n])[column];
}

QVariant UuidTreeData::data(QString id, const QString &name) const
{
    int column = m_dataNames.indexOf(name);
    if (column == -1)
        return QVariant();
    return data(id, column);
}

bool UuidTreeData::setData(QString id, int column, const QVariant &value)
{
    UuidTreeNode* n = node(id);
    if (!n)
        return false;

    if (!writeData(id, column, value))
        return false;

    if (!m_dataCache.contains(n))
        if (!refreshData(n))
            return false;

    (*m_dataCache[n])[column] = value;

    return true;
}

bool UuidTreeData::setData(QString id, const QString &name,
                           const QVariant &value)
{
    int column = m_dataNames.indexOf(name);
    if (column == -1)
        return false;
    return setData(id, column, value);
}

bool UuidTreeData::isEditable(QString id, int column) const
{
    Q_UNUSED(column)

    return !id.isEmpty();
}

UuidTreeNode *UuidTreeData::parent(QString id) const
{
    UuidTreeNode* n = node(id);
    if (!n)
        return nullptr;
    return node(n->parent);
}

int UuidTreeData::childCount(QString parent) const
{
    UuidTreeNode* n = node(parent);
    if (!n)
        return 0;
    return n->children.count();
}

int UuidTreeData::dataCount(QString id) const
{
    UuidTreeNode* n = node(id);
    if (!n)
        return 0;

    if (!m_dataCache.contains(n))
        if (!refreshData(n))
            return 0;
    return m_dataCache[n]->count();
}

UuidTreeNode *UuidTreeData::child(QString parent, int index) const
{
    UuidTreeNode* n = node(parent);
    if (!n){
        return nullptr;
    }
    return node(n->children.value(index));
}

int UuidTreeData::childIndex(QString parent, QString child) const
{
    UuidTreeNode* n = node(parent);
    if (!n)
        return -1;
    return n->children.indexOf(child);
}

QString UuidTreeData::insertChild(QString parent,
                                  const QMap<QString, QVariant> &data)
{
    UuidTreeNode* n = node(parent);
    if (!n)
        return QString("");

    QString id = insertData(parent, data);

    if (!id.isEmpty()) {
        n->children.append(id);
    }
    return id;
}

bool UuidTreeData::removeChild(QString parent, QString id)
{
    UuidTreeNode* np = node(parent);
    if (!np)
        return false;
    UuidTreeNode* nc = node(id);
    if (!nc)
        return false;
    for (const QString &child : qAsConst(nc->children))
        removeChild(id, child);

    if (!removeData(parent, id))
        return false;

    m_dataCache.remove(nc);
    np->children.removeOne(id);
    m_nodes.remove(id);
    delete nc;

    return true;
}

}}
