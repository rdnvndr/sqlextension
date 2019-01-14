#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

//! Элемент данных
class TreeItem
{
public:
    //! Конструктор данных
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);

    //! Деструктор данных
    virtual ~TreeItem();

    //! Добавляет дочерний элемент
    void appendChild(TreeItem *child);

    //! Возвращает дочерний элемент
    TreeItem *child(int row);

    //! Возвращает количество дочерних элементов
    int childCount() const;

    //! Возвращает количество колонок
    int columnCount() const;

    //! Возвращает данные в указанной колонке
    QVariant data(int column) const;

    //! Возвращает строку
    int row() const;

    //! Возвращает родительский элемент
    TreeItem *parentItem();

    //! Возвращает заблокированна ли работа с дочерними элементами
    bool isLocked() const;

    //! Устанавливает блокировку работы с дочерними элементами
    void setLocked(bool lock);

    //! Возвращает получены ли дочерние элементы
    bool isFetched() const;

    //! Устанавливает флаг получения дочерних элементов
    void setFetched(bool fetch);

private:
    //! Родительские элементы
    TreeItem *m_parentItem;

    //! Дочерние элементы
    QList<TreeItem*> m_childItems;

    //! Данные
    QList<QVariant> m_itemData;

    //! Флаг блокировки работы с дочерними элементами
    bool m_locked;

    //! Флаг получения дочерних элементов
    bool m_fetched;
};

#endif // TREEITEM_H
