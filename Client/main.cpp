#include "controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller connection;
    connection.start("127.0.0.1");

    return a.exec();
}
