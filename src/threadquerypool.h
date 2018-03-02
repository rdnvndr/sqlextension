#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSet>
#include <QtSql/QSqlDatabase>

#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {

//! Интерфейс пула многопоточных SQL запросов
class IThreadQueryPool
{

public:
    //! Дружественный класс
    friend class ThreadQueryItem;

protected:
    //! Освобождает многопоточный SQL запрос
    virtual void freeThreadQuery(ThreadQueryItem *item) = 0;
};

//! Класс пула многопоточных SQL запросов
template<class T>
class SQLEXTENSIONLIB ThreadQueryPool: IThreadQueryPool
{

public:
    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database())
    {
        m_db = db;
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool() {
        qDeleteAll(m_freeQueue);
        foreach (ThreadQueryItem *item, m_itemList)
            item->m_pool = NULL;
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem *threadQuery()
    {
        ThreadQuery *query;
        if (!m_freeQueue.isEmpty()) {
            query = m_freeQueue.dequeue();
        } else {
            query = dynamic_cast<ThreadQuery *>(new T(m_db));
        }

        ThreadQueryItem *item = new ThreadQueryItem(query, this);
        m_itemList.insert(item);

        return item;
    }

protected:
    //! Освобождает многопоточный SQL запрос
    void freeThreadQuery(ThreadQueryItem *item)
    {
        item->clear();
        m_freeQueue.enqueue(item->m_query);
        m_itemList.remove(item);
    }

private:
    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QQueue<ThreadQuery *> m_freeQueue;

    //! Список занятых многопоточных SQL запросов
    QSet<ThreadQueryItem *> m_itemList;
};

}}

#endif // THREADQUERYPOOL_H
