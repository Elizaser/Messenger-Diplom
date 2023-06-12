#include "registrationprocess.h"

RegistrationProcess::RegistrationProcess(QTcpSocket* socket, WorkDataBase* db):Process(db)
{
    this->socket = socket;
}

void RegistrationProcess::sendingData(DataParsing messageFromClient)
{
    ClientInfo newClientInfo = messageFromClient.getClient();
    if (!db->isClientExist(newClientInfo.login)) {
        if(db->insertUser(newClientInfo.name, newClientInfo.login, newClientInfo.password)){
            printClientInfo("Зарегистрировался клиент: ", newClientInfo);
            sockWrite(socket, generateData("registration", "welcome"));
        } else {
            printClientInfo("Ошибка с базами данных. Не получилось добавить нового пользователя", newClientInfo);
            sockWrite(socket, generateData("registration", "errorInsertBD"));
        }
    } else {
        qDebug()<<"Попытка регистрации под уже существующим логином. Отказано";
        sockWrite(socket, generateData("registration", "errorLogin"));
    }
}
