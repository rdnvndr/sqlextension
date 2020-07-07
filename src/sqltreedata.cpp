#include "sqltreedata.h"

#include <QtCore/QUuid>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace RTPTechGroup {
namespace SqlExtension {

SqlTreeData::SqlTreeData(QObject *parent, int maxCost)
    : UuidTreeData(parent, maxCost)
{

}

void SqlTreeData::setDatabase(QSqlDatabase database)
{
    m_db = database;
}

void SqlTreeData::setTable(const QString &table)
{
    m_tableName = table;
}

void SqlTreeData::setFields(const QString &idField, const QString &parentField,
                             const QStringList &dataFields)
{
    m_idName = idField;
    m_parentName = parentField;
    m_dataNames = dataFields;
}

SqlTreeData::~SqlTreeData()
{

}

UuidTreeNode *SqlTreeData::createNode(QString id) const
{
    UuidTreeNode* n = nullptr;

    QSqlQuery query(m_db);
    if (!id.isEmpty()) {
        query.exec(QString("SELECT %1, %2, %3 FROM %4 WHERE %5 = '%6'")
                   .arg(m_idName)
                   .arg(m_parentName)
                   .arg(m_dataNames.join(", "))
                   .arg(m_tableName)
                   .arg(m_idName)
                   .arg(id)
                   );

        if (!query.next())
            return nullptr;

        n = new UuidTreeNode;
        n->id = query.value(0).toString();
        n->parent = query.value(1).toString();
        QList<QVariant> *nodeValue = new QList<QVariant>();
        for (int i=0; i < m_dataNames.count(); i++)
           nodeValue->append(query.value(i+2));
        m_dataCache.insert(n, nodeValue);
    } else {
        n = new UuidTreeNode;
        n->id.clear();
        n->parent = QString("-");
        QList<QVariant> *nodeValue = new QList<QVariant>();
        for (int i=0; i < m_dataNames.count(); i++)
            nodeValue->append(m_dataNames[i]);
        m_dataCache.insert(n, nodeValue);
    }

    query.exec(QString("SELECT %1 FROM %2 WHERE %3 %4 ORDER BY %5")
               .arg(m_idName)
               .arg(m_tableName)
               .arg(m_parentName)
               .arg(id.isEmpty() ? "is NULL" : "= '" + id + "'")
               .arg(m_idName)
               );

    while (query.next()) {
        n->children.append(query.value(0).toString());
    }

    return n;
}

bool SqlTreeData::refreshData(UuidTreeNode *node) const
{
    QSqlQuery query(m_db);
    QString id = node->id;
    if (!id.isEmpty()) {
        query.exec(QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                   .arg(m_dataNames.join(", "))
                   .arg(m_tableName)
                   .arg(m_idName)
                   .arg(id)
                   );

        if (!query.next())
            return false;

        QList<QVariant> *nodeValue = new QList<QVariant>();
        for (int i=0; i < m_dataNames.count(); i++)
           nodeValue->append(query.value(i));
        m_dataCache.insert(node, nodeValue);
    } else {
        QList<QVariant> *nodeValue = new QList<QVariant>();
        for (int i=0; i < m_dataNames.count(); i++)
            nodeValue->append(m_dataNames[i]);
        m_dataCache.insert(node, nodeValue);
    }

    return true;
}

bool SqlTreeData::writeData(QString id, int column, const QVariant &value)
{
    QSqlQuery query(m_db);
    QString queryStr = QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
            .arg(m_tableName)
            .arg(m_dataNames.value(column))
            .arg(value.toString())
            .arg(m_idName)
            .arg(id);
    if (!query.exec(queryStr)) {
        return false;
    }

    return true;
}

bool SqlTreeData::removeData(QString parent, QString id)
{
    Q_UNUSED(parent)

    QSqlQuery query(m_db);
    if (!query.exec(QString("DELETE FROM %1 WHERE %2 = '%3'")
                    .arg(m_tableName).arg(m_idName).arg(id)))
        return false;

    return true;
}

QString SqlTreeData::insertData(QString parent, const QMap<QString, QVariant> &data)
{
    QSqlQuery query(m_db);
    QStringList names = data.keys();
    QStringList values;

    for (const QVariant &value : data.values()) {
        values.append(QString("'%1'").arg(value.toString()));
    }

    names.prepend(m_parentName);
    values.prepend( parent.isEmpty() ? "NULL" : "'" + parent + "'");

    names.prepend(m_idName);
    QString id = QUuid::createUuid().toString();
    values.prepend("'" + id + "'");

    QString queryStr = QString("INSERT INTO %1(%2) VALUES(%3)")
            .arg(m_tableName)
            .arg(names.join(", "))
            .arg(values.join(", "));

    if (!query.exec(queryStr)) {
        QString err = query.lastError().text();
        return QString("");
    }

    return id;
}

bool SqlTreeData::isEditable(QString id, int column) const
{
    QString field = m_dataNames[column];
    return (UuidTreeData::isEditable(id, column)
            && field != m_idName
            && field != m_parentName);
}

}}
