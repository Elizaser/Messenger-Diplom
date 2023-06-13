#include "entryprocess.h"

EntryProcess::EntryProcess(QTcpSocket* socket, WorkDataBase* db):Process(db)
{
    this->socket = socket;
}

void EntryProcess::sendingData(DataParsing messageFromClient)
{
    QString signal  = messageFromClient.getSignal();
    ClientInfo curClientInfo = messageFromClient.getClient();
    ClientInfo curClientInfo2 = db->getUserInfo(curClientInfo.login);
    curClientInfo.userID = curClientInfo2.userID;
    curClientInfo.name = curClientInfo2.name;
    curClientInfo.status = curClientInfo2.status;

    if(signal == "getStatus") {
        sendingStatus(curClientInfo);
    }
    else if(signal == "setUserInfo") {
        sendingUserInfo(curClientInfo);
        db->insertNewOnlineUser(QString::number(socket->socketDescriptor()), curClientInfo.userID);
    } else {
          qDebug()<<"Ошибка. Непонятно имя сигнала";
    }

}

void EntryProcess:: sendingStatus(ClientInfo curClientInfo)
{
    if (db->isCorrectInfoClient(curClientInfo.login, curClientInfo.password)) {
        qDebug() << "1 entry curClientInfo.userID = " << curClientInfo.userID;
        sockWrite(socket, generateData("entry", "welcome"));
        printClientInfo("Вошел клиент ", curClientInfo);

    } else {
        sockWrite(socket, generateData("entry", "error"));
        printClientInfo("Неверный логин или пароль ", curClientInfo);
    }
}

void EntryProcess:: sendingUserInfo(ClientInfo curClientInfo)
{
    qDebug() << "2 entry curClientInfo.userID = " << curClientInfo.userID;
    sockWrite(socket, generateData("main", "setUserInfo", curClientInfo));
}

