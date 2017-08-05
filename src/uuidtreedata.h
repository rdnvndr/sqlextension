#ifndef UUIDTREEDATA_H
#define UUIDTREEDATA_H

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
    QList<QVariant> data;
};

//! Абстракный класс узла дерева
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
class  UuidTreeData : public QObject
{
    Q_OBJECT

public:
    //! Конструктор узла дерева
    explicit UuidTreeData(QObject *parent = 0, int maxCost = 10000);

    //! Деструктор узла дерева
    virtual ~UuidTreeData();


    //! Возращает значение по UUID и номеру колонки
    QVariant data(QString id, int column) const;

    //! Возращает значение по UUID и названию колонки
    QVariant data(QString id, const QString &name) const;

    //! Устанавливает значение по UUID и номеру колонки
    bool setData(QString id, int column, const QVariant &value);

    //! Устанавливает значение по UUID и названию колонки
    bool setData(QString id, const QString& name, const QVariant &value);


    //! Возращает UUID родителя
    UuidTreeNode* parent(QString id) const;

    //! Возращает UUID потомка
    UuidTreeNode* child(QString parent, int index) const;

    //! Возращает индекс потомка
    int childIndex(QString parent, QString child) const;


    //! Возращает количество детей в родителе
    int childCount(QString parent)  const;

    //! Возращает количество записей
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

    //! Возращает узел дерева
    UuidTreeNode *node(QString id) const;

protected:
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

protected:
    //! Кэш узлов
    mutable QCache<QString, UuidTreeNode> m_cache;

    //! Список полей данных для выборки
    QStringList m_dataNames;
};

#endif // UUIDTREEDATA_H
