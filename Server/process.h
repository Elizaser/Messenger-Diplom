#ifndef PROCESS_H
#define PROCESS_H
#include <QTcpSocket>
#include "process.h"
#include "workdatabase.h"
#include "dataparsing.h"
#include "clientinfo.h"

class Process:public QObject
{
Q_OBJECT
signals:
    void writeSocketInProcess(QTcpSocket* socket, QByteArray data);
protected:
    WorkDataBase* db;
    void sockWrite(QTcpSocket* socket, QString process, QString signal);
    void sockWrite(QTcpSocket* socket, QString process, QString signal, QString message);
    void sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientChats> chats);
    void sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientMessage> chats);
    void sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientInfo> clientInfos);
    void printClientInfo(QString header, ClientInfo clientInfo);
public:
    Process(WorkDataBase* db);
    virtual void sendingData(){}
};

#endif // PROCESS_H
