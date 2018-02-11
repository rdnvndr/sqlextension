#ifndef THREADQUERYITEM_H
#define THREADQUERYITEM_H

#include <QObject>

#include "threadquery.h"
#include "threadquerypool.h"

namespace RTPTechGroup {
namespace SqlExtension {

class ThreadQueryPool;

//! Класс предназначен для обёртки многопоточного SQL запроса
class ThreadQueryItem : public QObject
{
    Q_OBJECT

private:
    //! Конструктор класса
    explicit ThreadQueryItem(ThreadQuery *query, ThreadQueryPool *pool);

    //! Запрещает конструктор копирования
    ThreadQueryItem( const ThreadQueryItem& ) = delete;

    //! Запрещает оператор присваивания
    void operator=( const ThreadQueryItem& ) = delete;

public:
    //! Деструктор класса
    virtual ~ThreadQueryItem();

// Общие настройки
    //! Установка политики точности числа
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);

    //! Получение политики точности числа
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy();

    //! Установка просмотра записей только вперёд
    void setForwardOnly(bool forward);

    //! Получение просмотра записей только вперёд
    bool isForwardOnly();

    //! Установка заполнителя
    void bindValue(const QString  &placeholder,
                   const QVariant &val, QSql::ParamType paramType = QSql::In);

    //! Получение заполнителя
    QVariant boundValue(const QString & placeholder);

    //! Установка заполнителей
    QMap<QString, QVariant> boundValues();

// Подготовка и выполнение запроса
    //! Подготовка запроса
    void prepare(const QString &query);

    //! Выполнение указанного запроса
    void execute(const QString &query);

    //! Выполнение подготовленного запроса
    void execute();

    //! Выполнение ранее подготовленного запроса в пакете
    void executeBatch(QSqlQuery::BatchExecutionMode mode = QSqlQuery::ValuesAsRows);

    //! Возвращает текущий запрос
    QString   lastQuery();

    //! Возвращает последнюю ошибку
    QSqlError lastError() const;

// Позиционирование на записи
    //! Перейти к первой записи
    void first();

    //! Перейти к следующей записи
    void next();

    //! Перейти к указанной записи
    void seek(int index, bool relative = false);

    //! Перейти к предыдущей записи
    void previous();

    //! Перейти к последней записи
    void last();

// Получение значений
    //! Вызывает получение всех значений в потоке
    void fetchAll();

    //! Вызывает получение значения из потока
    void fetchOne();

// Окончание выполнения запроса
    //! Окончание выполнения запроса
    void finish();

    //! Очищает запрос
    void clear();

// Работа с транзакциями
    //! Начало транзакции
    void transaction();

    //! Завершение транзакции
    void commit();

    //! Откат транзакции
    void rollback();

    //! Дружественный класс
    friend ThreadQueryPool;

signals:
    //! Сигнал об окончании выполнения операции
    void executeDone(bool success);

    //! Возвращает номер позиции
    void changePosition(int pos);

    //! Возвращает ошибку
    void error(QSqlError err);

    //! Возвращает все значения из потока
    void values(const QList<QSqlRecord> &records);

    //! Возвращает значение из потока
    void value(const QSqlRecord &record);

private:
    //! Многопоточный SQL запрос
    ThreadQuery *m_query;

    //! Пул многопоточных SQL запросов
    ThreadQueryPool *m_pool;
};

}}

#endif // THREADQUERYITEM_H
