#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtSql/QSqlDatabase>

#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {


template<class T>
class ThreadQueryItem;

//! Класс пула многопоточных SQL запросов
template<class T>
class SQLEXTENSIONLIB ThreadQueryPool : public QObject
{

public:
    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database())
        : QObject()
    {
        m_db = db;
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool() {
        QMutexLocker locker(&m_mutex);
        qDeleteAll(m_freeQueue);
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem<T> *acquire()
    {
        m_mutex.lock();
        ThreadQueryItem<T> *query = (!m_freeQueue.isEmpty())
                ? m_freeQueue.dequeue() : NULL;
        m_mutex.unlock();

        if (query == NULL)
            query = new ThreadQueryItem<T>(this, m_db);

        return query;
    }

    //! Дружественный класс
    friend class ThreadQueryItem<T>;

private:
    //! Освобождает многопоточный SQL запрос
    void release(ThreadQueryItem<T> *item)
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
    QQueue<ThreadQueryItem<T> *> m_freeQueue;
};

}}

#endif // THREADQUERYPOOL_H
