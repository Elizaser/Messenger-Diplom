#include "worksocket.h"

WorkSocket::WorkSocket(){

}

WorkSocket::~WorkSocket(){}

void WorkSocket::start()
{
    if (this->listen(QHostAddress::Any,5555)) {
        qDebug()<<"Listening users";
    } else {
        qDebug()<<"Not listening users";
    }
}

void WorkSocket::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<socketDescriptor<<" new connection";
    sockets.insert(socketDescriptor, new QSslSocket(this));
    if(!sockets[socketDescriptor]->setSocketDescriptor(socketDescriptor)){
        return;
    }

    foreach (qintptr key, controllers.keys()){
        controllers.value(key)->updateListSockets(socketDescriptor, sockets[socketDescriptor]);
    }
    Controller* controller = new Controller(sockets, socketDescriptor, this);
    controllers.insert(socketDescriptor, controller);

    QThread * thread = new QThread(this);
    controller->moveToThread(thread);

    connect(controller, &Controller::error, this, &WorkSocket::error);
    connect(thread, &QThread::started, controller, &Controller::start);

    connect(controller,SIGNAL(finished()),thread,SLOT(quit()));
    connect(controller,SIGNAL(sockDisconnect(int)),this,SLOT(deleteSocketInSockets(int)));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(controller, SIGNAL(writeSocket(QSslSocket*, QByteArray)),this, SLOT(swriteSocket(QSslSocket*, QByteArray)));
    connect(controller,SIGNAL(finished()),controller,SLOT(deleteLater()));


    thread->start();

}

void WorkSocket::error(QString err)
{

}

void WorkSocket::swriteSocket(QSslSocket* socket, QByteArray data)
{
    qDebug() << "out - "<< data;
    socket->write(data);
    socket->waitForBytesWritten();
}
void WorkSocket:: deleteSocketInSockets(int socketDescriptor)
{
    sockets.remove(socketDescriptor);
    controllers.remove(socketDescriptor);
    foreach (qintptr key, controllers.keys()){
        controllers.value(key)->updateListSockets(socketDescriptor, sockets[socketDescriptor]);
    }
}
