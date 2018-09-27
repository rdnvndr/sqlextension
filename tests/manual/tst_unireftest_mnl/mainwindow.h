#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QSemaphore>
#include <QDateTime>
#include <QMutex>
#include <QQueue>
#include <QCache>
#include <QSqlDatabase>

#include <sqlextension/dialogconnect.h>
#include <sqlextension/threadquerypool.h>
#include <sqlextension/threadquery.h>
#include <math.h>

#include "querymanagerthread.h"

using namespace RTPTechGroup::SqlExtension;


namespace Ui {
class MainWindow;
}

//! Основное окно программы
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! Конструктор класса
    explicit MainWindow(QWidget *parent = nullptr);

    //! Деструктор класса
    ~MainWindow();

    //! Обрабатывает закрытие окна
    void closeEvent(QCloseEvent *);

public slots:
    //! Выполняет поиск данных
    void onActionExec();

    //! Выполняет поиск данных без учёта кэша
    void onRunAction();

    //! Производит соединение с БД
    void onActionConnect();

    //! Отображает количесво созданных и свободных элементов пула
    void onShowPool();

    //! Отображает результаты поиска
    void onResult(QString r);

    //! Добавляет результаты поиска в кэш
    void addToCache();

private:
    //! Основное окно
    Ui::MainWindow *ui;

    //! Модель результатов запроса
    QStringListModel *m_model;

    //! Список результатов запроса
    QStringList m_list;

    //! Защищает модель результатов запроса
    QMutex m_modelMutex;

    //! Время начала поиска
    QDateTime m_start;

    //! Время окончания поиска
    QDateTime m_finish;

    //! Кэш результата запроса
    QCache<QString, QStringList> m_cache;

    //! Пул многопоточных запроосов таблиц УТС
    ThreadQueryPool<QueryManagerThread> *m_threadManagerPool;

    //! Пул запросов значений из таблиц
    ThreadQueryPool<Query>              *m_threadPool;

    //! Текущий пул многопоточных запроосов данных
    ThreadQueryItem<QueryManagerThread> *m_threadQuery;
};

#endif // MAINWINDOW_H
