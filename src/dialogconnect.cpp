#include "dialogconnect.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QCloseEvent>

#include "ui_dialogconnect.h"
#include "threadconnect.h"
#include "clogging.h"

namespace RTPTechGroup {
namespace SqlExtension {

DialogConnect::DialogConnect(QWidget *pwgt)
    : QDialog(pwgt), ui(new Ui::DialogConnect)
{
    ui->setupUi(this);

    ui->textLabelHostname->hide();
    ui->editHostname->hide();
    ui->textLabelBD->hide();
    ui->editDatabase->hide();
    ui->textLabelPort->hide();
    ui->portSpinBox->hide();
    ui->textLabelDrv->hide();
    ui->comboDriver->hide();
    ui->line->hide();

    m_movie = new QMovie(":connecting");
    m_movie->setScaledSize(QSize(65,50));
    m_movie->start();
    m_movie->stop();
    ui->labelImage->setAttribute(Qt::WA_NoSystemBackground);
    ui->labelImage->setMovie(m_movie);

    QStringList drivers = QSqlDatabase::drivers();

    // remove names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");
    drivers.removeAll("QSQLITE");
    //drivers.removeAll("QOCI");
    //drivers.removeAll("QTDS");
    if (drivers.indexOf("QODBC")!=-1)
        drivers.insert(0,"MSSQL");
    drivers.removeAll("QODBC3");
    //drivers.removeAll("QODBC");
    //drivers.removeAll("QPSQL");
    //drivers.removeAll("QIBASE");

    ui->comboDriver->addItems(drivers);

    this->adjustSize();
    connect(ui->pushButtonProp, &QPushButton::clicked,
            this, &DialogConnect::onClickButtonProp);

    m_threadConnect = new ThreadConnect(this);
    connect(m_threadConnect, &ThreadConnect::finishConnect,
            this, &DialogConnect::finishConnect);
}

DialogConnect::~DialogConnect()
{
    delete ui;
    delete m_threadConnect;
    delete m_movie;
}

void DialogConnect::accept()
{
    startConnect();
}

void DialogConnect::setDriverName(const QString &name)
{
    ui->comboDriver->setCurrentIndex(ui->comboDriver->findText(name));
}

void DialogConnect::setDatabaseName(const QString &name)
{
    ui->editDatabase->setText(name);
}

void DialogConnect::setHostName(const QString &host)
{
    ui->editHostname->setText(host);
}

void DialogConnect::setUserName(const QString &name)
{
    ui->editUsername->setText(name);
}

void DialogConnect::setPassword(const QString &password)
{
    ui->editPassword->setText(password);
}

void DialogConnect::setPort(int port)
{
    ui->portSpinBox->setValue(port);
}

QString DialogConnect::driverName() const
{
    return ui->comboDriver->currentText();
}

QString DialogConnect::databaseName() const
{
    return ui->editDatabase->text();
}

QString DialogConnect::hostName() const
{
    return ui->editHostname->text();
}

QString DialogConnect::userName() const
{
    return ui->editUsername->text();
}

QString DialogConnect::password() const
{
    return ui->editPassword->text();
}

int DialogConnect::port() const
{
    return ui->portSpinBox->value();
}

void DialogConnect::closeEvent(QCloseEvent *event)
{
    if (!ui->comboDriver->isEnabled())
        event->ignore();
}

void DialogConnect::setLockDialog(bool locked)
{
    ui->comboDriver->setDisabled(locked);
    ui->editDatabase->setDisabled(locked);
    ui->editHostname->setDisabled(locked);
    ui->editUsername->setDisabled(locked);
    ui->editPassword->setDisabled(locked);
    ui->portSpinBox->setDisabled(locked);
    ui->pushButtonOk->setDisabled(locked);
    ui->pushButtonCancel->setDisabled(locked);
    if (locked)
        m_movie->start();
    else
        m_movie->stop();
}

void DialogConnect::startConnect()
{
    setLockDialog(true);

    if (driverName() == "MSSQL"){
        m_threadConnect->setDriverName("QODBC");

        m_threadConnect->setDatabaseName(
                    QString("%1;%2;%3").arg("DRIVER={SQL Server}")
                    .arg("DATABASE=" + databaseName())
                    .arg("SERVER=" + hostName()));
    } else if (driverName() == "QOCI") {
        m_threadConnect->setDriverName("QOCI");

        m_threadConnect->setDatabaseName(
                    QString("(DESCRIPTION = "
                            "(ADDRESS_LIST = "
                            "(ADDRESS = "
                            "(PROTOCOL = TCP)"
                            "(HOST = %1)"
                            "(PORT = %2)"
                            ")"
                            ")"
                            "(CONNECT_DATA ="
                            "(SERVER = DEDICATED)"
                            "(SID =%3)"
                            ")"
                            ")")
                    .arg(hostName())
                    .arg(port())
                    .arg(databaseName()));
    } else {
        m_threadConnect->setDriverName(driverName());
        m_threadConnect->setDatabaseName(databaseName());
        m_threadConnect->setHostName(hostName());
    }

    m_threadConnect->setPort(port());
    m_threadConnect->setUserName(userName());
    m_threadConnect->setPassword(password());

    m_threadConnect->start();

}

void DialogConnect::finishConnect(QString result)
{
    setLockDialog(false);

    if (!result.isEmpty()) {
        qCWarning(lcSqlExtension) << result;
        QMessageBox::warning(this, tr("Ошибка создании соединения"), result);
    } else
        QDialog::accept();
}

void DialogConnect::onClickButtonProp(){
    if (ui->textLabelHostname->isHidden()){
        ui->pushButtonProp->setText(tr("Кратко"));
        ui->textLabelHostname->show();
        ui->editHostname->show();
        ui->textLabelBD->show();
        ui->editDatabase->show();
        ui->textLabelPort->show();
        ui->portSpinBox->show();
        ui->textLabelDrv->show();
        ui->comboDriver->show();
        ui->line->show();
    } else {
        ui->pushButtonProp->setText(tr("Подробно"));
        ui->textLabelHostname->hide();
        ui->editHostname->hide();
        ui->textLabelBD->hide();
        ui->editDatabase->hide();
        ui->textLabelPort->hide();
        ui->portSpinBox->hide();
        ui->textLabelDrv->hide();
        ui->comboDriver->hide();
        ui->line->hide();
    }
    this->adjustSize();
}

}}
