#include "threadconnect.h"

#include <QSqlDatabase>
#include <QSqlError>

namespace RTPTechGroup {
namespace SqlExtension {

ThreadConnect::ThreadConnect(QObject *parent): QThread(parent)
{

}

void ThreadConnect::run()
{
    {
        QSqlDatabase db = QSqlDatabase::database();
        if (db.open()) {
            emit finishConnect("");
            return;
        }

        QSqlError err = db.lastError();
        emit finishConnect(err.text());
    }
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection))
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

}}
