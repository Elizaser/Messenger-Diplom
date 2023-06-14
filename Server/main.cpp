#include <QCoreApplication>
#include "workdatabase.h"
#include "worksocket.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WorkSocket sock;
    sock.start();

    return a.exec();
}
