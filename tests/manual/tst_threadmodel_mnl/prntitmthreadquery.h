#ifndef PRNTITMTHREADQUERY_H
#define PRNTITMTHREADQUERY_H

#include <sqlextension/threadquery.h>

#include "treeitem.h"
using namespace RTPTechGroup::SqlExtension;

//! Класс выполнения многопоточного Sql запроса для получения данных дочерних
//! элементов, содержащихся в указанном родительском элементе
class PrntItmThreadQuery : public ThreadQuery
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit PrntItmThreadQuery(const QString &query = QString(),
                     const QSqlDatabase &db = QSqlDatabase::database());

    //! Конструктор класса
    explicit PrntItmThreadQuery(const QSqlDatabase &db);

    //! Возвращает родительский элемент
    TreeItem *parentItem() const;

    //! Устанавливает родительский элемент
    void setParentItem(TreeItem *parentItem);

private:
    //! Родительский элемент
    TreeItem *m_parentItem;
};

#endif // PRNTITMTHREADQUERY_H
