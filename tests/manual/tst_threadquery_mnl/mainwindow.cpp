#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql/QSqlRecord>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
/*
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("localhost");
    db.setDatabaseName(
            QString("DRIVER=%1;DATABASE=%2;SERVER=%3")
            .arg("{SQL Server}")
            .arg("VRTSERVER_V7")
            .arg("localhost"));
    db.setUserName("sa");
    db.setPassword("111");
    db.setPort(1433);
*/
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/tst_sqlextension.db");
    if (db.open())
        ui->logPlainText->insertPlainText("START THREAD\n");

    m_threadTest = new ThreadQuery();

    connect(m_threadTest, &ThreadQuery::executeDone,
            this, [=] (bool success) {
        if (success) {
            ui->logPlainText->insertPlainText("Execute done.\n");
            m_threadTest->fetchAll();
            m_threadTest->first();
        } else {
            ui->logPlainText->insertPlainText("Execute error.\n");
            ui->logPlainText->insertPlainText(m_threadTest->lastError().text());
        }
    });

    connect(m_threadTest, &ThreadQuery::values,
            this, [=] (const QList<QSqlRecord> &values)
    {
        ui->logPlainText->insertPlainText("\n");
        foreach (QSqlRecord record, values)
            ui->logPlainText->insertPlainText(
                        QString("Values: %1 \n")
                        .arg(record.value("FULLNAME").toString()));
        ui->logPlainText->insertPlainText("End FetchAll\n");
        ui->logPlainText->insertPlainText("\n");
    });

    connect(m_threadTest, &ThreadQuery::changePosition,
            this, [=] (int pos) {
        if (pos >= 0)
            m_threadTest->fetchOne();
        else {
            ui->logPlainText->insertPlainText("End FetchAll\n");
            ui->logPlainText->insertPlainText("\n");
        }
    });

    connect(m_threadTest, &ThreadQuery::value,
            this, [=] (const QSqlRecord &value)
    {
        ui->logPlainText->insertPlainText(
                    QString("Value: %1 \n")
                    .arg(value.value("FULLNAME").toString()));
        m_threadTest->next();

    });
}

MainWindow::~MainWindow()
{
    delete m_threadTest;
    delete ui;
}

void MainWindow::on_runPushButton_clicked()
{
    m_threadTest->prepare("SELECT * FROM COUNTRIES");
    m_threadTest->execute();

}

void MainWindow::on_stopPushButton_clicked()
{
    m_threadTest->stopFetch();
}
