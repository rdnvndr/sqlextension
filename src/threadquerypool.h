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

//! Класс пула многопоточных SQL запросов
class SQLEXTENSIONLIB  ThreadQueryPool : public QObject
{
    Q_OBJECT
public:
    //! Конструктор класса
    explicit ThreadQueryPool(QObject *parent = nullptr,
                             QSqlDatabase db = QSqlDatabase::database());

    //! Конструктор класса
    explicit ThreadQueryPool(QSqlDatabase db);

    //! Выдает многопоточный SQL запрос
    ThreadQueryItem *threadQuery();

    //! Устанавливает функцию обработки окончания запроса
    void setExecuteDone(ThreadQueryFunction func);

    //! Дружественный класс
    friend ThreadQueryItem;

private:
    //! Освобождение многопоточного SQL запроса
    void freeThreadQuery(ThreadQuery *query);

    //! Имя драйвера БД
    QString m_driverName;

    //! Имя БД
    QString m_databaseName;

    //! Имя хоста БД
    QString m_hostName;

    //! Порт БД
    int m_port;

    //! Логин пользователя БД
    QString m_userName;

    //! Пароль пользователя БД
    QString m_password;

    //! Очередь свободных многопоточных SQL запросов
    QQueue<ThreadQuery *> m_freeQueue;

    //! Функция обработки окончания выполнения запроса
    ThreadQueryFunction m_executeDoneFunc;
};

}}

#endif // THREADQUERYPOOL_H
