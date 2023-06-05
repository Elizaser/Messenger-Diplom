#ifndef ENTRYROCESS_H
#define ENTRYROCESS_H

#include "process.h"

class EntryProcess:public Process
{
    Q_OBJECT
    QTcpSocket* socket;
    void sendingStatus(ClientInfo curClientInfo);
    void sendingUserInfo(ClientInfo curClientInfo);
public:
    EntryProcess(QTcpSocket* socket, WorkDataBase* db);
    void sendingData(DataParsing messageFromClient);
};

#endif // ENTRYROCESS_H
