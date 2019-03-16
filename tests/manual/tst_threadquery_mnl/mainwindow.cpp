#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql/QSqlRecord>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/tst_sqlextension.db");
    if (db.open())
        ui->logPlainText->insertPlainText("START THREAD\n");

    m_threadTest = new ThreadQuery();

    connect(m_threadTest, &ThreadQuery::executeDone, this,
            [=] (const QUuid &queryUuid, const QSqlError &err)
    {
        if (err.isValid())
            return;

        ui->logPlainText->insertPlainText("Execute done.\n");
        m_threadTest->fetchAll(queryUuid);
        m_threadTest->first(queryUuid);
    });

    connect(m_threadTest, &ThreadQuery::values, this,
            [=] (const QUuid &queryUuid, const QList<QSqlRecord> &values)
    {
        Q_UNUSED(queryUuid)

        ui->logPlainText->insertPlainText("\n");
        for (const QSqlRecord &record : qAsConst(values))
            ui->logPlainText->insertPlainText(
                        QString("Values: %1 \n")
                        .arg(record.value("FULLNAME").toString()));
        ui->logPlainText->insertPlainText("End FetchAll\n");
        ui->logPlainText->insertPlainText("\n");
    });

    connect(m_threadTest, &ThreadQuery::changePosition, this,
            [=] (const QUuid &queryUuid, int pos)
    {
        if (pos >= 0)
            m_threadTest->fetchOne(queryUuid);
        else {
            ui->logPlainText->insertPlainText("End FetchAll\n");
            ui->logPlainText->insertPlainText("\n");
        }
    });

    connect(m_threadTest, &ThreadQuery::value, this,
            [=] (const QUuid &queryUuid, const QSqlRecord &value)
    {
        ui->logPlainText->insertPlainText(
                    QString("Value: %1 \n")
                    .arg(value.value("FULLNAME").toString()));
        m_threadTest->next(queryUuid);

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
    m_threadTest->finish();
}
