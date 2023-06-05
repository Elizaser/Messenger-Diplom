#include "registrationprocess.h"

RegistrationProcess::RegistrationProcess(QTcpSocket* socket, WorkDataBase* db):Process(db)
{
    this->socket = socket;
}

void RegistrationProcess::sendingData(DataParsing messageFromClient)
{
    ClientInfo newClientInfo = messageFromClient.getClientInfo();
    if (!db->isClientExist(newClientInfo.login)) {
        if(db->insertUser(newClientInfo.name, newClientInfo.login, newClientInfo.password)){
            printClientInfo("Зарегистрировался клиент: ", newClientInfo);
            sockWrite(socket, "registration", "welcome");
        } else {
            printClientInfo("Ошибка с базами данных. Не получилось добавить нового пользователя", newClientInfo);
            sockWrite(socket, "registration", "errorInsertBD");
        }
    } else {
        qDebug()<<"Попытка регистрации под уже существующим логином. Отказано";
        sockWrite(socket, "registration", "errorLogin");
    }
}
