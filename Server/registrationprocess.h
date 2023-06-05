#ifndef REGISTRATIONPROCESS_H
#define REGISTRATIONPROCESS_H

#include "process.h"

class RegistrationProcess: public Process
{
    Q_OBJECT
    QTcpSocket* socket;
public:
    RegistrationProcess(QTcpSocket* socket, WorkDataBase* db);
    void sendingData(DataParsing messageFromClient);
};

#endif // REGISTRATIONPROCESS_H
