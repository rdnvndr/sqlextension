#ifndef THREADCONNECT_H
#define THREADCONNECT_H

#include <QtCore/QThread>

namespace RTPTechGroup {
namespace SqlExtension {

//! Класс для запуска соединения в отдельном потоке
class ThreadConnect: public QThread {
    Q_OBJECT

public:
    //! Конструктор класса
    explicit ThreadConnect(QObject *parent = nullptr);

    //! Запуск соединения в потоке
    void run() override;

    //! Устанавливает имя драйвера
    void setDriverName(const QString &driverName);

    //! Устанавливает имя базы данных
    void setDatabaseName(const QString &databaseName);

    //! Устанавливает имя хоста
    void setHostName(const QString &hostName);

    //! Устанавливает номер порта
    void setPort(int port);

    //! Устанавливает имя пользователя
    void setUserName(const QString &userName);

    //! Устанавливает пароль
    void setPassword(const QString &password);

signals:
    //! Сигнал об ошибке соединения
    void finishConnect(QString text);

private:
    //! Имя драйвера
    QString m_driverName;

    //! Имя базы данных
    QString m_databaseName;

    //! Имя хоста
    QString m_hostName;

    //! Порт
    int     m_port;

    //! Имя пользователя
    QString m_userName;

    //! Пароль
    QString m_password;
};

}}

#endif // THREADCONNECT_H
