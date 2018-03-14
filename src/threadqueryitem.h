#ifndef THREADQUERYITEM_H
#define THREADQUERYITEM_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "threadquery.h"
#include "threadquerypool.h"

namespace RTPTechGroup {
namespace SqlExtension {

template<class T>
class ThreadQueryPool;

//! Класс предназначен для обёртки многопоточного SQL запроса
template<class T>
class ThreadQueryItem : public T
{

public:
    //! Возвращает в пул многопоточных SQL запросов
    void release() {
        if (m_pool) {
            m_new = false;
            m_pool->release(this);
        }
    }

    //! Проверяет новый ли многопоточный SQL запрос
    bool isNew() {
        return m_new;
    }

    //! Дружественный класс
    friend class ThreadQueryPool<T>;

private:
    //! Конструктор класса
    explicit ThreadQueryItem(ThreadQueryPool<T> *pool, QSqlDatabase db) : T(db)
    {
         m_pool  = pool;
         m_new = true;
    }

    //! Запрещает конструктор копирования
    ThreadQueryItem( const ThreadQueryItem& ) = delete;

    //! Запрещает оператор присваивания
    void operator=( const ThreadQueryItem& ) = delete;

private:
    //! Многопоточный SQL запрос
    ThreadQuery *m_query;

    //! Пул многопоточных SQL запросов
    QPointer< ThreadQueryPool<T> > m_pool;

    //! Флаг нового многопоточного SQL запроса
    bool m_new;
};

}}

#endif // THREADQUERYITEM_H
