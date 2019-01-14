#ifndef THREADMODEL_H
#define THREADMODEL_H

#include <QAbstractItemModel>
#include <sqlextension/threadquerypool.h>

#include "treeitem.h"
#include "prntitmthreadquery.h"

using namespace RTPTechGroup::SqlExtension;

//! Модель примера базы данных chinook.db
class ThreadModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //! Конструктор класса
    explicit ThreadModel(QObject *parent = nullptr);

    //! Деструктор класса
    virtual ~ThreadModel() override;

    //! Возвращает количество строк
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    //! Возвращает количество колонок
    int columnCount(const QModelIndex &parent) const override;

    //! Возвращает индекс в родителе
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    //! Возвращает индекс родителя
    QModelIndex parent(const QModelIndex &index) const override;

    //! Возвращает данные по индексу
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    //! Возвращает заголовок
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    //! Возвращает флаг для индекса
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //! Получение элемента по индексу
    TreeItem *toItem(const QModelIndex &index) const;

    //! Получение индекса по элементу
    QModelIndex fromItem(TreeItem *item, int column = 0) const;

    //! Возвращает наличие дочерних элементов
    bool hasChildren(const QModelIndex &parent) const override;

    //! Возвращает возможность ленивого чтения данных
    bool canFetchMore(const QModelIndex &parent) const override;

    //! Ленивое чтение данных
    void fetchMore(const QModelIndex &parent) override;

private:
    //! Корневой элемент данных
    TreeItem *rootItem;

    //! Пул многопоточных Sql запросов для получения данных дочерних элементов
    ThreadQueryPool<PrntItmThreadQuery> *m_threadPool;
};

#endif // THREADMODEL_H
