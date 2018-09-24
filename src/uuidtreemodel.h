#ifndef UUIDTREEMODEL_H
#define UUIDTREEMODEL_H

#include "sqlextensionglobal.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class UuidTreeData;

//! Класс модель дерева с UUID идентификатором
/*! Данный класс предназначен для создания дерева на основе, UUID идентификаторов.
    Узлы дерева связаны между собой полями идентификатор и ссылки на
    идентификатор родителя.\n
    Пример использования:
    \code
        UuidTreeData* data = new UuidTreeData(this);
        data->setDatabase(db);
        data->setTable("tags");
        QStringList columns;
        columns << "name";
        data->setFields(columns);

        SqlUuidTreeModel* model = new SqlUuidTreeModel(data, this);
        QTreeView* treeView = new QTreeView( this );
        treeView->setAlternatingRowColors(true);
        treeView->setModel(model);
    \endcode

*/
class SQLEXTENSIONLIB UuidTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //! Конструктор модели
    UuidTreeModel(UuidTreeData *data, QObject *parent = nullptr);

    //! Деструктор модели
    ~UuidTreeModel();

    //! Возвращает хранимые данные
    QVariant data(const QModelIndex &index, int role) const;

    //! Устанавливает значение для указанной записи
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    //! Возвращает флаг записи
    Qt::ItemFlags flags(const QModelIndex &index) const;

    //! Возвращает название заголовка
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    //! Устанавливает название заголовка
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role);

    //! Возвращает индекс модели для строки и колонки
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;

    //! Возвращает индекс родителя
    QModelIndex parent(const QModelIndex &index) const;

    //! Возвращает количество строк в индексе родителя
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //! Возвращает количество столбцов в индексе родителя
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //! Удаление строки
    bool removeRows(int position, int rows, const QModelIndex &parent);

    //! Вставка строки
    bool insertRows(int position, int rows, const QModelIndex &parent);

private:
    //! Данные узла дерева
    UuidTreeData* m_data;
};

#endif // UUIDTREEMODEL_H
