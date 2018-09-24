#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QSettings>

using namespace RTPTechGroup::SqlExtension;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectAction, &QAction::triggered, this, &MainWindow::onActionConnect);
    connect(ui->runAction, &QAction::triggered, this, &MainWindow::onRunAction);
    connect(ui->exitAction, &QAction::triggered, this, &MainWindow::close);
    m_model = new QStringListModel(this);
    ui->resultTableView->setModel(m_model);
    connect(ui->findLineEdit, &QLineEdit::textChanged, this, &MainWindow::onActionExec);
    connect(ui->poolAction, &QAction::triggered, this, &MainWindow::onShowPool);

    m_threadQuery = nullptr;
    m_threadManagerPool = nullptr;
    m_threadPool = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_model;
}

void MainWindow::onActionExec()
{
    ui->logPlainText->clear();
    if (!QSqlDatabase::database().isOpen()) {
        ui->logPlainText->appendPlainText(
                    "Отсутствует соединение с базой данных\n");
        return;
    }

    if (ui->findLineEdit->text().isEmpty()) {
        ui->logPlainText->appendPlainText("Введите строку поиска\n");
        return;
    }
    if (m_threadQuery) {
        m_threadQuery->stopFetch();
        m_modelMutex.lock();

        if (ui->cacheAction->isChecked()) {
            QStringList *list = m_cache.object(ui->findLineEdit->text());
            if (list) {
                m_model->setStringList(*list);
                m_modelMutex.unlock();
                return;
            }
        }

        m_list.clear();
        m_model->setStringList(m_list);
        m_modelMutex.unlock();
    }

    ThreadQueryItem<QueryManagerThread> *threadQuery = m_threadManagerPool->acquire();
    if (threadQuery->isNew())
    {
        threadQuery->setThreadPool(m_threadPool);
        connect(threadQuery, &QueryManagerThread::stoppedFetch,
                threadQuery, &ThreadQueryItem<QueryManagerThread>::release,
                Qt::DirectConnection);

        connect(threadQuery, &QueryManagerThread::result,
                this, &MainWindow::onResult, Qt::QueuedConnection);

        connect(threadQuery, &QueryManagerThread::changePosition,
                [this](const QUuid &queryUuid, int pos)
        {
           if (pos == ThreadQuery::AfterLastRow)
               addToCache();
        });

        connect(threadQuery, &QueryManagerThread::error,
                [this](const QUuid &queryUuid, const QSqlError &err)
        {
            if (err.type() != QSqlError::NoError)
                ui->logPlainText->appendPlainText(err.text());
        });
    }

    m_threadQuery = threadQuery;

    threadQuery->prepare(
                "SELECT '" + ui->findLineEdit->text() + "' AS SEARCHWORD,\n"
                "BO_CLASSES.NAMECLASS AS NAMECLASS,\n"
                "BO_CLASSES.NAMETABLE AS NAMETABLE,\n"
                "BO_CLASSES.FGUID AS CLS_FGUID,\n"
                "BO_ATTR_CLASSES.NAMEATTR AS NAMEATTR,\n"
                "BO_ATTR_CLASSES.NAMEFIELD AS NAMEFIELD,\n"
                "BO_ATTR_CLASSES.NAMESCREEN AS NAMESCREEN\n"
                "FROM BO_ATTR_CLASSES \n"
                "LEFT OUTER JOIN BO_CLASSES\n"
                "ON BO_CLASSES.GUID = BO_ATTR_CLASSES.FGUID\n"
                "INNER JOIN BO_ATTR_CLASSES AS t3\n"
                "ON t3.NAMEFIELD = 'GUID' AND t3.FGUID = BO_CLASSES.GUID\n"
                "AND not t3.NAMEFIELD is NULL\n"
                "WHERE substring(BO_ATTR_CLASSES.ARRAYMDATA, 3,1) = '1'\n"
                "ORDER BY CLS_FGUID, NAMETABLE");
    threadQuery->execute();
    ui->logPlainText->appendPlainText("Запуск запроса\n");
    m_start = QDateTime::currentDateTime();
}

void MainWindow::onRunAction()
{
    m_cache.remove(ui->findLineEdit->text());
    onActionExec();
}

void MainWindow::onActionConnect()
{
    DialogConnect *windowConnect = new DialogConnect(this);
    windowConnect->setWindowTitle("Соединение с БД");
    QSettings settings("NONAME", "TestUts");
    settings.beginGroup("DbConnect");
    windowConnect->setDriverName(settings.value("driver").toString());
    windowConnect->setDatabaseName(settings.value("database").toString());
    windowConnect->setHostName(settings.value("hostname").toString());
    windowConnect->setPort(settings.value("port",-1).toInt());
    windowConnect->setUserName(settings.value("username").toString());

    if (windowConnect->exec() == QDialog::Accepted) {
        settings.setValue("driver",   windowConnect->driverName());
        settings.setValue("database", windowConnect->databaseName());
        settings.setValue("hostname", windowConnect->hostName());
        settings.setValue("port",     windowConnect->port());
        settings.setValue("username", windowConnect->userName());
    }
    settings.endGroup();

    delete windowConnect;

    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        if (m_threadPool != nullptr)
            m_threadPool->deleteLater();
        if (m_threadManagerPool != nullptr)
            m_threadManagerPool->deleteLater();

        this->m_threadManagerPool = new ThreadQueryPool<QueryManagerThread>(db);
        this->m_threadPool        = new ThreadQueryPool<ThreadQuery>(db);
    } else {
        QSqlError err = db.lastError();
        if (err.type() != QSqlError::NoError){
            ui->logPlainText->appendPlainText(err.text());
            QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        }
    }

    return;
}

void MainWindow::onShowPool()
{
    ui->logPlainText->appendPlainText(
                QString("\nПул менеджеров: %1 из %2"
                        "\nПул запросов: %3 из %4\n")
                .arg(m_threadManagerPool->freeCount())
                .arg(m_threadManagerPool->count())
                .arg(m_threadPool->freeCount())
                .arg(m_threadPool->count())
    );

}

void MainWindow::onResult(QString r)
{
    QMutexLocker lock(&m_modelMutex);
    int count = m_model->rowCount();
    m_model->insertRow(count);
    m_model->setData(m_model->index(count,0), r);
    m_finish = QDateTime::currentDateTime();
    int msecs = m_finish.time().msecsTo(m_start.time());
    ui->logPlainText->appendPlainText(
    QString("Запрос %1 выполнен за %2 мсек").arg(count).arg(abs(msecs)));

    if (count == 9) {
        addToCache();
    }
}

void MainWindow::addToCache()
{
    QString text = ui->findLineEdit->text();
    int textCount = text.count();
    int pos = -1;
    int i;
    m_cache.insert(text, new QStringList(m_model->stringList()));

    for (i = textCount-1; i > 0; --i) {
        QStringList *cacheValue = m_cache.object(text.left(i));
        if (cacheValue && *(cacheValue) == m_model->stringList())
            pos = i;
    }

    if (pos != -1) {
        for (i = textCount-1; i >= pos; --i)
            m_cache.insert(text.left(i), new QStringList(m_model->stringList()));
    }

}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (m_threadPool != nullptr)
        m_threadPool->deleteLater();
    if (m_threadManagerPool != nullptr)
        m_threadManagerPool->deleteLater();
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}
