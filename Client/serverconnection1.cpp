#include "serverconnection.h"

ServerConnection::ServerConnection()
{

}
void ServerConnection:: start(QString hostName)
{
    socket = new QTcpSocket();
    socket->connectToHost(hostName, 5555);

    while(!socket->waitForConnected(1000)) {
        throw -1;
    }
    connect(socket, SIGNAL(readyRead()), SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), SLOT(sockDisc()));
}
void ServerConnection:: sockDisc()
{
    socket->deleteLater();
}
void ServerConnection:: sockReady(){}
