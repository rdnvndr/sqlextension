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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

public slots:
    void onActionExec();
    void onRunAction();
    void onActionConnect();
    void onShowPool();
    void onResult(QString r);
    void addToCache();

signals:
    void stoped();

private:
    Ui::MainWindow *ui;
    QStringListModel *m_model;
    QStringList m_list;
    QMutex m_modelMutex;

    QDateTime m_start;
    QDateTime m_finish;

    QCache<QString, QStringList> m_cache;

    ThreadQueryPool<QueryManagerThread> *m_threadManagerPool;
    ThreadQueryPool<ThreadQuery>        *m_threadPool;
    ThreadQueryItem<QueryManagerThread> *m_threadQuery;
};

#endif // MAINWINDOW_H
