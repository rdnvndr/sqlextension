#ifndef SQLTREEDATA_H
#define SQLTREEDATA_H

#include <QtSql/QSqlDatabase>

#include "sqlextensionglobal.h"
#include "uuidtreedata.h"

namespace RTPTechGroup {
namespace SqlExtension {

//! Класс узла дерева
class SQLEXTENSIONLIB SqlTreeData : public UuidTreeData
{
	Q_OBJECT

public:
    //! Конструктор узла дерева
    explicit SqlTreeData( QObject *parent = 0, int maxCost = 10000);

    //! Деструктор узла дерева
    virtual ~SqlTreeData();


    //! Установка текущей БД
    void setDatabase(QSqlDatabase database);

    //! Установка текущей таблицы
    void setTable(const QString &table);

    //! Установка списка полей для выборки из таблицы
    void setFields( const QString &idField, const QString &parentField,
                    const QStringList &dataFields );


    //! Проверка является ли запись редактируемой
    bool isEditable(QString id, int column) const;


protected:
    //! Создание узла дерева
    UuidTreeNode *createNode(QString id) const;

    //! Обновление данных
    bool refreshData(UuidTreeNode *node) const;

    //! Запись данных
    bool writeData(QString id, int column, const QVariant &value);

    //! Удаление данных
    bool removeData(QString parent, QString id);

    //! Вставка данных
    QString insertData( QString parent, const QMap<QString, QVariant> &data
                                = QMap<QString, QVariant>());

private:
    //! Текущая БД
    QSqlDatabase m_db;

    //! Имя таблицы
    QString m_tableName;

    //! Имя поля UUID записи  таблицы
    QString m_idName;

    //! Имя поля UUID родителя записи  таблицы
    QString m_parentName;
};

}}
#endif // SQLTREEDATA_H
