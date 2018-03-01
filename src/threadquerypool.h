#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QObject>
#include <QSqlDatabase>
#include <QQueue>

#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {

class ThreadQueryItem;

class IThreadQueryPool
{
    //! Дружественный класс
    friend class ThreadQueryItem;
protected:
    virtual void freeThreadQuery(ThreadQuery *query) = 0;
};

//template<class T = ThreadQuery> class ThreadQueryPool;

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

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem *threadQuery()
    {
        ThreadQuery *query;
        if (!m_freeQueue.isEmpty()) {
            query = m_freeQueue.dequeue();
        } else {
            query = dynamic_cast<ThreadQuery *>(new T(m_db));
        }

        return new ThreadQueryItem(query, this);
    }

private:
    //! Освобождение многопоточного SQL запроса
    void freeThreadQuery(ThreadQuery *query)
    {
        query->clear();
        m_freeQueue.enqueue(query);
    }

    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QQueue<ThreadQuery *> m_freeQueue;
};

}}

#endif // THREADQUERYPOOL_H
