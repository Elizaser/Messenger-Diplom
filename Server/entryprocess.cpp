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
        qDebug()<<"EntryProcess socket = " << socket;
        sockWrite(socket, "entry", "welcome");
        printClientInfo("Вошел клиент ", curClientInfo);

    } else {
        sockWrite(socket, "entry", "error");
        printClientInfo("Неверный логин или пароль ", curClientInfo);
    }
}

void EntryProcess:: sendingUserInfo(ClientInfo curClientInfo)
{
    sockWrite(socket, "main", "setUserInfo",
              "\"userID\":\"" + curClientInfo.userID +
              "\", \"name\":\"" +  curClientInfo.name +
              "\", \"login\":\"" + curClientInfo.login +
              "\", \"password\":\"" + curClientInfo.password + "\"");
}

