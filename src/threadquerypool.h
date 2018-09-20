#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSemaphore>
#include <QtSql/QSqlDatabase>

#include "threadquery.h"

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
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database(),
                             uint maxCount = 1000)
        : QObject()
    {
        m_db = db;
        m_stopFetch = false;
        m_semaphore.release(maxCount);
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool() {
        m_stopFetch = true;
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem<T> *acquire()
    {
        if (m_stopFetch)
            return NULL;

        m_semaphore.acquire();
        m_mutex.lock();
        ThreadQueryItem<T> *query = (!m_freeQueue.isEmpty())
                ? m_freeQueue.dequeue() : NULL;
        m_mutex.unlock();

        if (query == NULL) {
            query = new ThreadQueryItem<T>(this, m_db);
            connect(this, &QObject::destroyed, query, &QObject::deleteLater);
        } else {
            query->m_busy = true;
        }

        return query;
    }

    //! Помечает занятым многопоточный SQL запрос
    bool acquire(ThreadQueryItem<T> *item) {
        QMutexLocker locker(&m_mutex);
        if (m_freeQueue.removeOne(item)) {
            item->m_busy = true;
            m_semaphore.acquire();
            return true;
        }
        return false;
    }

    //! Удаляет многопоточный SQL запрос
    void remove(ThreadQueryItem<T> *item) {
        QMutexLocker locker(&m_mutex);
        if (!m_freeQueue.removeOne(item))
            m_semaphore.release();
    }

    //! Возвращает количество свободных SQL запросов
    int freeCount() {
        QMutexLocker locker(&m_mutex);
        return m_freeQueue.count();
    }

    //! Возвращает возможное количество SQL запросов
    int count() {
        return m_semaphore.available();
    }

    //! Дружественный класс
    friend class ThreadQueryItem<T>;

private:
    //! Освобождает многопоточный SQL запрос
    void release(ThreadQueryItem<T> *item)
    {
        m_mutex.lock();
        m_freeQueue.enqueue(item);
        m_semaphore.release();
        m_mutex.unlock();
    }

private:
    //! Количество многопоточных SQL запросов
    QSemaphore m_semaphore;

    //! Мьютекс для работы с очередью
    QMutex m_mutex;

    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QQueue<ThreadQueryItem<T> *> m_freeQueue;

    volatile bool m_stopFetch;
};

}}

#endif // THREADQUERYPOOL_H
