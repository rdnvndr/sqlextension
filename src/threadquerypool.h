#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtSql/QSqlDatabase>

#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {

//! Интерфейс пула многопоточных SQL запросов
class IThreadQueryPool : public QObject
{
    Q_OBJECT
public:
    explicit IThreadQueryPool() : QObject() {

    }

    //! Дружественный класс
    friend class ThreadQueryItem;

protected:
    //! Освобождает многопоточный SQL запрос
    virtual void freeThreadQuery(ThreadQuery *item) = 0;
};

//! Класс пула многопоточных SQL запросов
template<class T>
class SQLEXTENSIONLIB ThreadQueryPool: public IThreadQueryPool
{

public:
    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database())
        : IThreadQueryPool()
    {
        m_db = db;
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool() {
        QMutexLocker locker(&m_mutex);
        qDeleteAll(m_freeQueue);
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem *threadQuery()
    {
        m_mutex.lock();
        ThreadQuery *query = (!m_freeQueue.isEmpty())
                ? m_freeQueue.dequeue() : NULL;
        m_mutex.unlock();

        if (query == NULL) {
            query = dynamic_cast<ThreadQuery *>(new T(m_db));
        }

        return new ThreadQueryItem(query, this);
    }

protected:
    //! Освобождает многопоточный SQL запрос
    void freeThreadQuery(ThreadQuery *item)
    {
        QMutexLocker locker(&m_mutex);
        m_freeQueue.enqueue(item);
    }

private:
    //! Мьютекс для работы с очередью
    QMutex m_mutex;

    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QQueue<ThreadQuery *> m_freeQueue;
};

}}

#endif // THREADQUERYPOOL_H
