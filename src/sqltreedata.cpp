#include "sqltreedata.h"

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
    UuidTreeNode* n;

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
            return 0;

        n = new UuidTreeNode;
        n->id = query.value(0).toString();
        n->parent = query.value(1).toString();
        for (int i=0; i < m_dataNames.count(); i++)
            n->data.append(query.value(i+2));
    } else {
        n = new UuidTreeNode;
        n->id.clear();
        n->parent = QString("-");
        for (int i=0; i < m_dataNames.count(); i++)
            n->data.append(m_dataNames[i]);
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
        QString err = query.lastError().text();
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
    foreach (QVariant value, data.values()) {
        values.append(QString("'%1'").arg(value.toString()));
    }
    names.prepend(m_parentName);
    values.prepend(QString("%1").arg(parent));
    QString queryStr = QString("INSERT INTO %1(%2) VALUES(%3)")
            .arg(m_tableName)
            .arg(names.join(", "))
            .arg(values.join(", "));

    if (!query.exec(queryStr))
        return QString("");

    return query.lastInsertId().toString();
}

bool SqlTreeData::isEditable(QString id, int column) const
{
    QString field = m_dataNames[column];
    return (UuidTreeData::isEditable(id, column)
            && field != m_idName
            && field != m_parentName);
}

