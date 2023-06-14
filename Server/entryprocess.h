#ifndef ENTRYROCESS_H
#define ENTRYROCESS_H

#include "process.h"

class EntryProcess:public Process
{
    Q_OBJECT
    QSslSocket* socket;
    void sendingStatus(ClientInfo curClientInfo);
    void sendingUserInfo(ClientInfo curClientInfo);
public:
    EntryProcess(QSslSocket* socket, WorkDataBase* db);
    void sendingData(DataParsing messageFromClient);
};

#endif // ENTRYROCESS_H
