#include <QCoreApplication>
#include "workdatabase.h"
#include "worksocket.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WorkSocket sock;
    sock.start();
//    QThread *sockThread = new QThread(this);
//    sock->moveToThread(sockThread);
    return a.exec();
}
