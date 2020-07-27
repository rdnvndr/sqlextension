#ifndef THREADQUERYPOOL_H
#define THREADQUERYPOOL_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QSemaphore>
#include <QtCore/QTimer>
#include <QtCore/QMutex>

#include <QtSql/QSqlDatabase>

#include <functional>

#include <utils/recursivespinlock.h>
#include <utils/spinlocker.h>
#include "threadquery.h"
#include "threadqueryitem.h"

namespace RTPTechGroup {
namespace SqlExtension {

template<typename T>
class ThreadQueryItem;

//! Класс пула многопоточных SQL запросов
template<typename T>
class ThreadQueryPool : public QObject
{

public:
    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db = QSqlDatabase::database(),
                             uint maxCount = 200, uint minCount = 0)
        : QObject()
    {
        m_db = db;
        m_stopFetch = false;
        m_availableCount.release(maxCount);
        m_minCount = minCount;
        m_expiryCount = 0;
        connect(&m_timer, &QTimer::timeout,
                this, &ThreadQueryPool::clearExpiredQueries);
    }

    //! Деструктор класса
    virtual ~ThreadQueryPool()
    {
        m_stopFetch = true;
    }

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem<T> *acquire(
          const std::function<void(ThreadQueryItem<T> *)> &initialize = nullptr)
    {
        if (m_stopFetch)
            return nullptr;

        m_availableCount.acquire();
        ThreadQueryItem<T> *query = nullptr;
        m_spinlock.lock();
        if (!m_freeQueue.isEmpty()) {
            query = *m_freeQueue.begin();
            m_freeQueue.remove(query);
        }
        m_spinlock.unlock();

        if (query == nullptr) {
            query = new ThreadQueryItem<T>(this, m_db);
            connect(this, &QObject::destroyed, query, &QObject::deleteLater);
            if (initialize != nullptr)
                initialize(query);
        }

        return query;
    }

    //! Помечает занятым многопоточный SQL запрос
    bool acquire(ThreadQueryItem<T> *item)
    {
        SpinLocker locker(&m_spinlock);
        if (m_freeQueue.remove(item)) {
            m_availableCount.acquire();
            return true;
        }
        return false;
    }

    //! Удаляет многопоточный SQL запрос
    void remove(ThreadQueryItem<T> *item)
    {
        SpinLocker locker(&m_spinlock);
        if (!m_freeQueue.remove(item))
            m_availableCount.release();
    }

    //! Возвращает количество свободных SQL запросов
    int freeCount()
    {
        SpinLocker locker(&m_spinlock);
        return m_freeQueue.count();
    }

    //! Возвращает возможное количество SQL запросов
    int count() const
    {
        return m_availableCount.available();
    }

    //! Возвращает период удаления многопоточных SQL запрос
    int expiryTimeout()
    {
        SpinLocker locker(&m_spinlock);
        return m_timer.interval();
    }

    //! Устанавливает в мс период удаления многопоточных SQL запрос
    void setExpiryTimeout(int expiryTimeout)
    {
        SpinLocker locker(&m_spinlock);
        if (expiryTimeout > 0)
            m_timer.start(expiryTimeout);
        else
            m_timer.stop();
    }

    //! Удаляет запросы с истекшим временем
    void clearExpiredQueries()
    {
        SpinLocker locker(&m_spinlock);
        while (!m_freeQueue.isEmpty() && m_expiryCount > m_minCount) {
            --m_expiryCount;
            delete *m_freeQueue.begin();
        }
        m_expiryCount = m_freeQueue.count();
    }

    //! Дружественный класс
    friend class ThreadQueryItem<T>;

private:
    //! Освобождает многопоточный SQL запрос
    void release(ThreadQueryItem<T> *item)
    {
        SpinLocker locker(&m_spinlock);
        if (!m_freeQueue.contains(item)) {
            m_freeQueue.insert(item);
            m_availableCount.release();
        }
    }

private:
    //! Количество доступных многопоточных SQL запросов
    QSemaphore m_availableCount;

    //! Минимальное количество зарезервированных многопоточных SQL запросов
    uint m_minCount;

    //! Рекурсивный spinlock для работы с очередью
    RecursiveSpinLock m_spinlock;

    //! База данных
    QSqlDatabase m_db;

    //! Очередь свободных многопоточных SQL запросов
    QSet<ThreadQueryItem<T> *> m_freeQueue;

    //! Период удаления многопоточных SQL запрос
    int m_expiryTimeout;

    //! Количество удаляемых многопоточных SQL запрос
    int m_expiryCount;

    //! Таймер
    QTimer m_timer;

    //! Флаг остановки предоставления многопоточных SQL запрос
    std::atomic_bool m_stopFetch;
};

}}

#endif // THREADQUERYPOOL_H
