#include "uuidtreemodel.h"
#include "uuidtreedata.h"

#include <QtCore/QVariant>

namespace RTPTechGroup {
namespace SqlExtension {

UuidTreeModel::UuidTreeModel(UuidTreeData *data, QObject *parent)
    :QAbstractItemModel(parent), m_data(data)
{

}

UuidTreeModel::~UuidTreeModel()
{

} 

int UuidTreeModel::columnCount(const QModelIndex &parent) const
{
    QString id;
    if (parent.isValid())
        id =*static_cast<QString*>(parent.internalPointer());
    else
        id.clear();
    return m_data->dataCount(id);
}


QVariant UuidTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    return m_data->data(*static_cast<QString*>(
                            index.internalPointer()), index.column());
}

bool UuidTreeModel::setData(const QModelIndex &index, const QVariant &value,
                               int role)
{
    if (role != Qt::EditRole)
        return false;
    return m_data->setData(*static_cast<QString*>(
                               index.internalPointer()), index.column(), value);
}


Qt::ItemFlags UuidTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags fl = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (m_data->isEditable(*static_cast<QString*>(
                               index.internalPointer()), index.column()))
    {
        fl |= Qt::ItemIsEditable;
    }
    return fl;
}

QVariant UuidTreeModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_data->data(QString(), section );
    }
    return QVariant();
}

bool UuidTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                     const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;
    return m_data->setData(QString(), section, value);
}

QModelIndex UuidTreeModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    UuidTreeNode* parentId;
    if (!parent.isValid())
        parentId = nullptr;
    else
        parentId = static_cast<UuidTreeNode*>(parent.internalPointer());

    if (!parentId) {
        parentId = m_data->node("");
    }

    UuidTreeNode* childId = m_data->node(parentId->children.value(row));

    if (!childId)
        QModelIndex();

    return createIndex(row, column, childId);

}

QModelIndex UuidTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    UuidTreeNode* childId = static_cast<UuidTreeNode* >(index.internalPointer());
    UuidTreeNode* parentId = m_data->node(childId->parent);
    if (!parentId)
        return QModelIndex();

    int row = m_data->childIndex(parentId->parent, parentId->id);
    return createIndex(row, 0, parentId);
}

int UuidTreeModel::rowCount(const QModelIndex &parent) const
{
    QString parentId;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentId.clear();
    else
        parentId = *static_cast<QString*>(parent.internalPointer());

    return m_data->childCount(parentId);
}

bool UuidTreeModel::removeRows(int position, int rows,
                                  const QModelIndex &parent)
{
    UuidTreeNode*  parentId = static_cast<UuidTreeNode*>(parent.internalPointer());

    bool success = true;
    beginRemoveRows(parent, position, position + rows - 1);
    for ( int i = 0; i < rows; i++ ) {
        UuidTreeNode* id = m_data->child(parentId->id, i + position);
        success = m_data->removeChild(parentId->id, id->id);
    }
    endRemoveRows();

    return success;
}

bool UuidTreeModel::insertRows(int position, int rows,
                                  const QModelIndex &parent)
{
    Q_UNUSED(position)

    QString parentId;
    if (parent.isValid())
        parentId = *static_cast<QString*>(parent.internalPointer());

    bool success = true;
    beginInsertRows(parent, 0, rows - 1);
    for (int i = 0; i < rows; i++) {
        QString id = m_data->insertChild(parentId);
        success = (id.isEmpty()) ? false : true;
    }
    endInsertRows();

    return success;
}

}}
