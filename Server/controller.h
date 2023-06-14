#ifndef SERVER_H
#define SERVER_H

#include <QThread>
#include <QTcpServer>
#include <QSslSocket>
#include "workdatabase.h"
#include "dataparsing.h"
#include "entryprocess.h"
#include "registrationprocess.h"
#include "mainprocess.h"

class Controller:public QObject
{
Q_OBJECT
    QMap <qintptr, QSslSocket*>sockets;
    WorkDataBase* db;
    QObject* parent;
    EntryProcess* entry;
    RegistrationProcess* registration;
    MainProcess* main;
public:
    qintptr sockDescriptor;
    Controller(QMap <qintptr, QSslSocket*> sockets, qintptr sockDescriptor, QObject * parent);
    void updateListSockets(qintptr sockDescriptor, QSslSocket* sockets);
    void deleteInListSockets(qintptr sockDescriptor);
public slots:
    void start();
    void swriteSocketInProcess(QSslSocket* sockets, QByteArray data);
private slots:
    void dataAnalysis();
    void disconnected();
signals:
    void finished();
    void sockDisconnect(int sockDescriptor);
    void error(QString err);
    void writeSocket(QSslSocket* sockets, QByteArray data);


};

#endif // SERVER_H
