#include "process.h"

Process::Process(WorkDataBase* db)
{
    this->db = db;
}

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\"}";
    emit writeSocketInProcess(socket, data);
}

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QString message)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", " + message.toLocal8Bit() + "}";
    emit writeSocketInProcess(socket, data);
}

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientChats> chats)
{
    if(chats.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"chats\":[";
        for(auto & chat : chats) {
            data.append("{\"chatID\":\"" + chat.chatID +
                        "\", \"userCreator\":\"" + chat.userCreator
                        + "\", \"name\":\"" + chat.name
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages +
                        "\", \"participants\":[");
            qDebug() << "chat.countIsNotReadMessages = " << chat.countIsNotReadMessages;
            for(auto & participant : chat.participants){
                data.append("{\"participant\":\"" + participant + "\"},");
            }
            data.remove(data.length()-1,1);
            data.append("]}, ");
        }
        data.remove(data.length()-1,1);
        data.remove(data.length()-1,1);
        data.append("]}");
        emit writeSocketInProcess(socket, data);
    } else {
        sockWrite(socket, process, signal,
                  "\"chat\":\"-1\"");
    }
}

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientMessage> conntents)
{
    if(conntents.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"conntents\":[";

        for(auto & conntent : conntents) {
            qDebug() << "conntent.messageID" << conntent.messageID;
            data.append("{\"messageID\":\"" + conntent.messageID +
                        "\", \"chatID\":\"" + conntent.chatID +
                        "\", \"senderID\":\"" + conntent.senderID +
                        "\", \"senderName\":\"" + conntent.senderName +
                        "\", \"isRead\":\"" + conntent.isRead +
                        "\", \"message\":\"" + conntent.message +"\"},");

        }
        data.remove(data.length()-1,1);
        data.append("]}");
        emit writeSocketInProcess(socket, data);
    } else {
        sockWrite(socket, process, signal,
                  "\"conntent\":\"-1\"");
    }
}

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientInfo> clientInfos)
{
    if(clientInfos.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"foundUsers\":[";

        for(auto & clientInfo : clientInfos) {
            data.append("{\"login\":\"" + clientInfo.login +
                        "\", \"name\":\"" + clientInfo.name +
                        "\", \"userID\":\"" + clientInfo.userID +"\"},");
        }
        data.remove(data.length()-1,1);
        data.append("]}");
        emit writeSocketInProcess(socket, data);
    } else {
        sockWrite(socket, process, signal, "\"foundUser\":\"-1\"");
    }
}


void Process::printClientInfo(QString header, ClientInfo clientInfo)
{
    qDebug()<< header;
    qDebug()<< "Логин: "<< clientInfo.login;
    qDebug()<< "Пароль: "<< clientInfo.password;
    qDebug()<< "Имя: "<< clientInfo.name;
}
