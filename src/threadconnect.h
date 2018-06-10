#ifndef THREADCONNECT_H
#define THREADCONNECT_H

#include <QThread>
#include <QMutex>

namespace RTPTechGroup {
namespace SqlExtension {

//! Класс для запуска соединения в отдельном потоке
class ThreadConnect: public QThread {
    Q_OBJECT

public:
    //! Конструктор класса
    explicit ThreadConnect(QObject *parent = NULL);

    //! Запуск соединения в потоке
    void run() Q_DECL_OVERRIDE;

    //! Возвращает имя драйвера
    QString driverName() const;

    //! Устанавливает имя драйвера
    void setDriverName(const QString &driverName);

    //! Возвращает имя базы данных
    QString databaseName() const;

    //! Устанавливает имя базы данных
    void setDatabaseName(const QString &databaseName);

    //! Возвращает имя хоста
    QString hostName() const;

    //! Устанавливает имя хоста
    void setHostName(const QString &hostName);

    //! Возвращает номер порта
    int port() const;

    //! Устанавливает номер порта
    void setPort(int port);

    //! Возвращает имя пользователя
    QString userName() const;

    //! Устанавливает имя пользователя
    void setUserName(const QString &userName);

    //! Возвращает пароль
    QString password() const;

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

    //! Мьютекс доступа к параметрам соединения
    QMutex  m_mutex;
};

}}

#endif // THREADCONNECT_H
