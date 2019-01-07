#ifndef UUIDTREEDATA_H
#define UUIDTREEDATA_H

#include "sqlextensionglobal.h"

#include <QObject>
#include <QCache>
#include <QStringList>
#include <QVariant>

//! Структура данных узла для кэша
struct UuidTreeNode
{
    QString id;
    QString parent;
    QList<QString> children;
};

//! Абстрактный класс узла дерева
/*! Данный абстрактный класс предназначен для создания узла дерева на основе,
    UUID идентификаторов. Узлы дерева связаны между собой полями идентификаторов
    и ссылками на идентификатор родителя. Для создания класса узла дерева
    необходимо реализовать методы:\n
    \code
        //! Создание узла дерева
        virtual UuidTreeNode *createNode(QString id) const = 0;

        //! Запись данных
        virtual bool writeData(QString id, int column, const QVariant &value) = 0;

        //! Удаление данных
        virtual bool removeData(QString parent, QString id) = 0;

        //! Вставка данных
        virtual QString insertData(QString parent,
                                   const QMap<QString, QVariant> &data
                                   = QMap<QString, QVariant>()) = 0;
    \endcode

*/
class SQLEXTENSIONLIB UuidTreeData : public QObject
{
    Q_OBJECT

public:
    //! Конструктор узла дерева
    explicit UuidTreeData(QObject *parent = 0, int maxCost = 10000);

    //! Деструктор узла дерева
    virtual ~UuidTreeData();


    //! Возвращает значение по UUID и номеру колонки
    QVariant data(QString id, int column) const;

    //! Возвращает значение по UUID и названию колонки
    QVariant data(QString id, const QString &name) const;

    //! Устанавливает значение по UUID и номеру колонки
    bool setData(QString id, int column, const QVariant &value);

    //! Устанавливает значение по UUID и названию колонки
    bool setData(QString id, const QString& name, const QVariant &value);


    //! Возвращает UUID родителя
    UuidTreeNode* parent(QString id) const;

    //! Возвращает UUID потомка
    UuidTreeNode* child(QString parent, int index) const;

    //! Возвращает индекс потомка
    int childIndex(QString parent, QString child) const;


    //! Возвращает количество детей в родителе
    int childCount(QString parent)  const;

    //! Возвращает количество записей
    int dataCount(QString id) const;


    //! Проверка является ли запись редактируемой
    bool isEditable(QString id, int column) const;

    //! Вставка узла
    QString insertChild(QString parent, const QMap<QString, QVariant> &data
                        = QMap<QString, QVariant>());

    //! Удаление узла
    bool removeChild(QString parent, QString id);


    //! Список полей с данными
    QStringList fields() const;

    //! Установка списка полей для выборки из таблицы
    void setFields(const QStringList &dataFields);

    //! Возвращает узел дерева
    UuidTreeNode *node(QString id) const;

protected:
    //! Создание узла дерева
    virtual UuidTreeNode *createNode(QString id) const = 0;

    //! Обновление данных
    virtual bool refreshData(UuidTreeNode *node) const = 0;

    //! Запись данных
    virtual bool writeData(QString id, int column, const QVariant &value) = 0;

     //! Удаление данных
    virtual bool removeData(QString parent, QString id) = 0;

    //! Вставка данных
    virtual QString insertData(QString parent,
                                const QMap<QString, QVariant> &data
                                = QMap<QString, QVariant>()) = 0;

protected:
    //! Узлы дерева
    mutable QMap<QString, UuidTreeNode *> m_nodes;

    //! Кэш данных
    mutable QCache<UuidTreeNode *, QList<QVariant> > m_dataCache;

    //! Список полей данных для выборки
    QStringList m_dataNames;
};

#endif // UUIDTREEDATA_H
