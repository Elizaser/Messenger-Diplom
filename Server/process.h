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
    QByteArray generateData(QString process, QString signal);
    QByteArray generateData(QString process, QString signal, QString message);
    QByteArray generateData(QString process, QString signal, QList<ClientChat> chats);
    QByteArray generateData(QString process, QString signal, ClientChat chat);
    QByteArray generateData(QString process, QString signal, QList<ClientMessage> conntents);
    QByteArray generateData(QString process, QString signal, ClientMessage conntent);
    QByteArray generateData(QString process, QString signal, QList<ClientInfo> clientInfos);
    QByteArray generateData(QString process, QString signal, ClientInfo clientInfo);
    QByteArray generateData(QString process, QString signal, QList<ClientMessage> conntents, QString dopMessage);
    void sockWrite(QTcpSocket* socket, QByteArray data);
    void printClientInfo(QString header, ClientInfo clientInfo);
public:
    Process(WorkDataBase* db);
    virtual void sendingData(){}
};

#endif // PROCESS_H
