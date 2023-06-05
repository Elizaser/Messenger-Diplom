#ifndef WORKSOCKET_H
#define WORKSOCKET_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include "controller.h"

class WorkSocket:public QTcpServer
{
    Q_OBJECT
public:
    WorkSocket();
    ~WorkSocket();
    void start();
private slots:
    void incomingConnection(qintptr socketDescriptor);
    void error(QString err);
    void swriteSocket(QTcpSocket* sockets, QByteArray data);
    void deleteSocketInSockets(int sockDescriptor);
private:
    QMap <qintptr, QTcpSocket*>sockets;
    QMap <qintptr, Controller*> controllers;
};

#endif // WORKSOCKET_H
