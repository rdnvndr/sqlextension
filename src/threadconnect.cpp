#include "threadconnect.h"

#include <QApplication>
#include <QtCore/QMutexLocker>
#include <QtCore/QMutex>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>


namespace RTPTechGroup {
namespace SqlExtension {

ThreadConnect::ThreadConnect(QObject *parent): QThread(parent)
{

}

void ThreadConnect::run()
{
    if (QSqlDatabase::database().isOpen()) {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    {
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase(m_driverName);
        db.setDatabaseName(m_databaseName);
        db.setHostName(m_hostName);
        db.setPort(m_port);
        db.setUserName(m_userName);
        db.setPassword(m_password);

        if (db.open()) {
            db.driver()->moveToThread(QApplication::instance()->thread());
            emit finishConnect("");
            return;
        }

        QSqlError err = db.lastError();
        emit finishConnect(err.text());
    }

    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection))
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void ThreadConnect::setDriverName(const QString &driverName)
{
    m_driverName = driverName;
}

void ThreadConnect::setDatabaseName(const QString &databaseName)
{   
    m_databaseName = databaseName;
}

void ThreadConnect::setHostName(const QString &hostName)
{   
    m_hostName = hostName;
}

void ThreadConnect::setPort(int port)
{
    m_port = port;
}

void ThreadConnect::setUserName(const QString &userName)
{   
    m_userName = userName;
}

void ThreadConnect::setPassword(const QString &password)
{   
    m_password = password;
}

}}
