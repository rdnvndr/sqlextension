#include "threadconnect.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QMutexLocker>
#include <QSqlDriver>
#include <QApplication>

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

QString ThreadConnect::driverName() const
{
    return m_driverName;
}

void ThreadConnect::setDriverName(const QString &driverName)
{
    QMutexLocker locker(&m_mutex);
    m_driverName = driverName;
}

QString ThreadConnect::databaseName() const
{
    return m_databaseName;
}

void ThreadConnect::setDatabaseName(const QString &databaseName)
{
    QMutexLocker locker(&m_mutex);
    m_databaseName = databaseName;
}

QString ThreadConnect::hostName() const
{
    return m_hostName;
}

void ThreadConnect::setHostName(const QString &hostName)
{
    QMutexLocker locker(&m_mutex);
    m_hostName = hostName;
}

int ThreadConnect::port() const
{
    return m_port;
}

void ThreadConnect::setPort(int port)
{
    QMutexLocker locker(&m_mutex);
    m_port = port;
}

QString ThreadConnect::userName() const
{
    return m_userName;
}

void ThreadConnect::setUserName(const QString &userName)
{
    QMutexLocker locker(&m_mutex);
    m_userName = userName;
}

QString ThreadConnect::password() const
{
    return m_password;
}

void ThreadConnect::setPassword(const QString &password)
{
    QMutexLocker locker(&m_mutex);
    m_password = password;
}

}}
