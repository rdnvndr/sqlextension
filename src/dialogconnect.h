#ifndef DIALOGCONNECT_H
#define	DIALOGCONNECT_H

#include <QMovie>
#include <QDialog>

#include "sqlextensionglobal.h"

namespace Ui {
class DialogConnect;
}

namespace RTPTechGroup {
namespace SqlExtension {

class ThreadConnect;

//! Диалоговое окно подключения к БД
/*! Данный класс предназначен для создания соединения с базой данных с
    использованием диалогового окна.\n
    Пример:\n
    \code

    DialogConnect *windowConnect = new DialogConnect();
    windowConnect->setWindowTitle(tr("Соединение"));

    QSettings settings("MySoft", "Star Runner");
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

    \endcode
*/
class SQLEXTENSIONLIB DialogConnect: public QDialog {

    Q_OBJECT
public:
    //! Конструктор диалогового окна подключения к БД
    explicit DialogConnect(QWidget *pwgt = 0);

    //! Деструктор диалогового окна подключения к БД
    virtual ~DialogConnect();

    //! Вызов успешного завершения
    void accept() override;

    //! Установка драйвера БД
    void setDriverName(const QString &name);

    //! Установка имени БД
    void setDatabaseName(const QString &name);

    //! Установка имени хоста
    void setHostName(const QString &host);

    //! Установка имени пользователя
    void setUserName(const QString &name);

    //! Установка пароля
    void setPassword(const QString &password);

    //! Установка порта
    void setPort(int port);


    //! Получение драйвера БД
    QString driverName() const;

    //! Получение имени БД
    QString databaseName() const;

    //! Получение имени хоста
    QString hostName() const;

    //! Получение имени пользователя
    QString userName() const;

    //! Получение пароля
    QString password() const;

    //! Получение порта
    int port() const;

protected:
    //! Слот обработки события закрытия окна
    void closeEvent(QCloseEvent *event) override;

private:
    //! Блокирует диалоговое окно
    void setLockDialog(bool locked);

private slots:
    //! Начало соединения
    void startConnect();

    //! Окончание открытия соединения
    void finishConnect(QString result);

public slots:
    //! Слот для отображения дополнительных параметров подключения к БД
    void onClickButtonProp();

private:
    //! Поток соединения
    ThreadConnect *m_threadConnect;

    //! Анимация соединения
    QMovie *m_movie;

    //! Пользовательский интерфейс
    Ui::DialogConnect *ui;
};

}}

#endif
