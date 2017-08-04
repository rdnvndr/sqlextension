#include <QApplication>
#include <sqlextension/dialogconnect.h>

using namespace RTPTechGroup::SqlExtension;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DialogConnect windowConnect;
    windowConnect.setWindowTitle("Connecting");
    windowConnect.show();

    return a.exec();
}
