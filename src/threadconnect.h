#ifndef THREADCONNECT_H
#define THREADCONNECT_H

#include <QThread>

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

signals:
    //! Сигнал об ошибке соединения
    void finishConnect(QString text);
};

}}

#endif // THREADCONNECT_H
