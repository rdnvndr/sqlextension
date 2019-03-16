#include "threadmodel.h"

#include <QApplication>
#include <QDir>
#include <QThread>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent>
#include <QDebug>

ThreadModel::ThreadModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "LastName" << "FirstName" <<"Title" << "BirthDate" << "HireDate"
             << "Address" << "City" << "State" << "Country" << "PostalCode"
             << "Phone" << "Fax" << "Email" << "Id";
    rootItem = new TreeItem(rootData);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/chinook.db");
    m_threadPool = new ThreadQueryPool<PrntItmThreadQuery>(db);
    if (!db.isOpen()) {
        QSqlError err = db.lastError();
        if (err.type() != QSqlError::NoError){
            qDebug() << err.text();
            QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        }
    }
}

ThreadModel::~ThreadModel()
{
    delete m_threadPool;
    delete rootItem;
}


int ThreadModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem *parentItem = this->toItem(parent);
    return parentItem->childCount();
}

int ThreadModel::columnCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = this->toItem(parent);

    return  parentItem->columnCount();
}

QVariant ThreadModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = this->toItem(index);
    return item->data(index.column());
}

QModelIndex ThreadModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = this->toItem(parent);
    TreeItem *childItem = parentItem->child(row);

    return this->fromItem(childItem, column);
}

QModelIndex ThreadModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = this->toItem(index);
    TreeItem *parentItem = childItem->parentItem();

    return this->fromItem(parentItem, index.column());
}

bool ThreadModel::canFetchMore(const QModelIndex &index) const
{
    TreeItem *parentItem = this->toItem(index);

    return !parentItem->isFetched() && !parentItem->isLocked();
}

void ThreadModel::fetchMore(const QModelIndex &index)
{
    TreeItem *parentItem = this->toItem(index);
    parentItem->setLocked(true);

    ThreadQueryItem<PrntItmThreadQuery> *threadQuery = m_threadPool->acquire();
    threadQuery->setParentItem(parentItem);
    if (threadQuery->isNew()) {
        // Обработка окончания выполнение запроса
        connect(threadQuery, &ThreadQuery::executeDone, [=]
                (const QUuid &queryUuid, const QSqlError &err)
        {
            if (err.isValid())
                return;

            // Имитация долгого получения данных
            QThread::msleep(1000);
            threadQuery->fetchAll(queryUuid);
        });

        // Обработка получения значений
        QObject::connect(threadQuery, &ThreadQuery::values, this,
                [=] (const QUuid &queryUuid, const QList<QSqlRecord> &records)
        {
            Q_UNUSED(queryUuid)

            TreeItem *item = threadQuery->parentItem();
            threadQuery->release();

            QModelIndex parent =  this->fromItem(item);
            if (!records.isEmpty()) {
                for (const QSqlRecord &record : records) {
                    QList<QVariant> itemData;
                    int recCount = record.count();
                    for (int column = 0; column < recCount; ++column)
                        itemData << record.value(column);
                    TreeItem *childItem = new TreeItem(itemData, item);
                    item->appendChild(childItem);
                }

                beginInsertRows(parent, 0, item->childCount() - 1);
                endInsertRows();
            }

            item->setLocked(false);
            item->setFetched(true);

            // Обновление содержимого узла для представлений
            QList<QPersistentModelIndex> indexList {QPersistentModelIndex(parent)};
            emit layoutChanged(indexList);

        }, Qt::QueuedConnection);
    }

    QString sqlStr = QString(
                "SELECT LastName, FirstName, Title, BirthDate, HireDate, Address,"
                "City, State, Country, PostalCode, Phone, Fax, Email, Employeeid "
                "FROM employees WHERE ReportsTo %1").arg(
                    (parentItem == rootItem) ? " IS NULL" :  "= "
                    + parentItem->data(parentItem->columnCount() - 1).toString()
                 );

    threadQuery->execute(sqlStr);
}

Qt::ItemFlags ThreadModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

TreeItem *ThreadModel::toItem(const QModelIndex &index) const
{
    if (!index.isValid())
        return  rootItem;
    else
        return  static_cast<TreeItem*>(index.internalPointer());
}

QModelIndex ThreadModel::fromItem(TreeItem *item, int column) const
{
    if (item && item != rootItem)
        return createIndex(item->row(), column, item);
    else
        return QModelIndex();
}

QVariant ThreadModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

bool ThreadModel::hasChildren(const QModelIndex &parent) const
{
    TreeItem *parentItem = this->toItem(parent);

    return (parentItem->isFetched()) ? !parentItem->isEmpty() : true;
}
