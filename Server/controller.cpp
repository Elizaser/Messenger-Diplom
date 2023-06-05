#include "controller.h"

Controller::Controller(QMap <qintptr, QTcpSocket*> sockets, qintptr sockDeskriptor, QObject * parent)
{
    this->sockDescriptor = sockDeskriptor;
    this->parent = parent;
    this->sockets = sockets;
    db = new WorkDataBase();
    db->start(sockDeskriptor);
}

void Controller::start()
{
    entry = new EntryProcess(sockets[sockDescriptor], db);
    registration = new RegistrationProcess(sockets[sockDescriptor], db);
    main = new MainProcess(sockets, sockets[sockDescriptor], db);
//    qDebug() << "Starting thread:"<< QThread::currentThreadId() << "Socket:" << sockets[sockDescriptor];
    connect(sockets[sockDescriptor], SIGNAL(readyRead()), this, SLOT(dataAnalysis()));
    connect(sockets[sockDescriptor], SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(entry, SIGNAL(writeSocketInProcess(QTcpSocket*, QByteArray)), this, SLOT(swriteSocketInProcess(QTcpSocket*, QByteArray)));
    connect(registration, SIGNAL(writeSocketInProcess(QTcpSocket*, QByteArray)), this, SLOT(swriteSocketInProcess(QTcpSocket*, QByteArray)));
    connect(main, SIGNAL(writeSocketInProcess(QTcpSocket*, QByteArray)), this, SLOT(swriteSocketInProcess(QTcpSocket*, QByteArray)));
}

void Controller::dataAnalysis()
{
    try {
        QByteArray strsocket = sockets[sockDescriptor]->readAll();
//        qDebug() << strsocket;
        DataParsing messageFromClient(strsocket);
        QString  process = messageFromClient.getProccess();
        if(process == "entry") {
            entry->sendingData(messageFromClient);
        } else if(process == "registration"){
            registration->sendingData(messageFromClient);
        } else if(process == "main"){
            main->sendingData(messageFromClient);
        }
    } catch (QString error_message){
        qDebug() <<  "Ошибка. Непонятно имя процесса " + error_message;
    }

}
void Controller::disconnected()
{
    qDebug() << sockDescriptor << " disconnected. Thread:" << QThread::currentThreadId();
    db->deleteOnlineUser(QString::number(sockDescriptor));
    sockets[sockDescriptor]->deleteLater();
    emit sockDisconnect(sockDescriptor);
    emit finished();
}
void Controller::updateListSockets(qintptr sockDescriptor, QTcpSocket* socket)
{
    this->sockets.insert(sockDescriptor, socket);
    main->updateListSockets(sockDescriptor, socket);
}
void Controller::deleteInListSockets(qintptr sockDescriptor)
{
    this->sockets.remove(sockDescriptor);
    main->deleteInListSockets(sockDescriptor);
}
void Controller:: swriteSocketInProcess(QTcpSocket* socket, QByteArray data){
    emit writeSocket(socket, data);
}
