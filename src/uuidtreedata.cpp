#include "uuidtreedata.h"

UuidTreeData::UuidTreeData(QObject *parent, int maxCost)
    : QObject(parent)
{
    m_cache.setMaxCost(maxCost);
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

}

UuidTreeNode* UuidTreeData::node(QString id) const
{
    UuidTreeNode* n = m_cache[id];
    if (n) return n;

    n = createNode(id);

    m_cache.insert(id, n);
    return n;
}

QVariant UuidTreeData::data(QString id, int column) const
{
    UuidTreeNode* n = node(id);
    if (!n)
        return QVariant();
    return n->data[column];
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

    n->data[column] = value;
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
    return n->data.count();
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

    if (!id.isEmpty())
        n->children.append(id);
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
    foreach(QString child, nc->children)
        removeChild(id, child);

    if (!removeData(parent, id))
        return false;

    np->children.removeOne(id);
    m_cache.remove(id);
    return true;
}



