#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QSemaphore>
#include <QtCore/QTimer>
#include <QtSql/QSqlDatabase>

#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {

template<class T>
class ThreadQueryItem;

//! Класс пула многопоточных SQL запросов
template<class T>
class ThreadQueryPool : public QObject
{

public:
    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database(),
                             uint maxCount = 1000)
        : QObject()
    {
        m_db = db;
        m_stopFetch = false;
        m_availableCount.release(maxCount);
        m_expiryCount = 0;
        connect(&m_timer, &QTimer::timeout,
                this, &ThreadQueryPool::clearExpiredQueries);
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool() {
        m_stopFetch = true;
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem<T> *acquire(bool *isNewInstance = nullptr)
    {
        if (m_stopFetch)
            return nullptr;

        m_mutex.lock();
        m_availableCount.acquire();
        ThreadQueryItem<T> *query = nullptr;
        if (!m_freeQueue.isEmpty()) {
            query = *m_freeQueue.begin();
            m_freeQueue.remove(query);
        }
        m_mutex.unlock();

        if (query == nullptr) {
            query = new ThreadQueryItem<T>(this, m_db);
            connect(this, &QObject::destroyed, query, &QObject::deleteLater);
            if (isNewInstance != nullptr)
                *isNewInstance = true;
        } else {
            if (isNewInstance != nullptr)
                *isNewInstance = false;
        }

        return query;
    }

    //! Помечает занятым многопоточный SQL запрос
    bool acquire(ThreadQueryItem<T> *item) {
        QMutexLocker locker(&m_mutex);
        if (m_freeQueue.remove(item)) {
            m_availableCount.acquire();
            return true;
        }
        return false;
    }

    //! Удаляет многопоточный SQL запрос
    void remove(ThreadQueryItem<T> *item) {
        QMutexLocker locker(&m_mutex);
        if (!m_freeQueue.remove(item))
            m_availableCount.release();
    }

    //! Возвращает количество свободных SQL запросов
    int freeCount() {
        QMutexLocker locker(&m_mutex);
        return m_freeQueue.count();
    }

    //! Возвращает возможное количество SQL запросов
    int count() {
        return m_availableCount.available();
    }

    //! Возвращает период удаления многопоточных SQL запрос
    int expiryTimeout() const {
        return m_timer.interval();
    }

    //! Устанавливает в мс период удаления многопоточных SQL запрос
    void setExpiryTimeout(int expiryTimeout) {
        if (expiryTimeout > 0)
            m_timer.start(expiryTimeout);
        else
            m_timer.stop();
    }

    //! Удаляет запросы с истекшим временем
    void clearExpiredQueries() {
        m_mutex.lock();
        while (!m_freeQueue.isEmpty() && m_expiryCount > 0) {
            m_availableCount.acquire();
            auto *query = *m_freeQueue.begin();
            m_freeQueue.remove(query);
            --m_expiryCount;

            m_mutex.unlock();
            delete query;
            m_mutex.lock();
        }
        m_expiryCount = m_freeQueue.count();
        m_mutex.unlock();
    }

    //! Дружественный класс
    friend class ThreadQueryItem<T>;

private:
    //! Освобождает многопоточный SQL запрос
    void release(ThreadQueryItem<T> *item)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_freeQueue.contains(item)) {
            m_freeQueue.insert(item);
            m_availableCount.release();
        }
    }

private:
    //! Количество доступных многопоточных SQL запросов
    QSemaphore m_availableCount;

    //! Мьютекс для работы с очередью
    QMutex m_mutex;

    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QSet<ThreadQueryItem<T> *> m_freeQueue;

    //! Период удаления многопоточных SQL запрос
    int m_expiryTimeout;

    //! Количество удаляемых многопоточных SQL запрос
    int m_expiryCount;

    //! Таймер
    QTimer  m_timer;

    std::atomic_bool m_stopFetch;
};

}}

#endif // THREADQUERYPOOL_H
