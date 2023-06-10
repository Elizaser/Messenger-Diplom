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

void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientChat> chats)
{
    if(chats.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"chats\":[";
        for(auto & chat : chats) {
            data.append("{\"chatID\":\"" + chat.chatID +
                        "\", \"userCreator\":\"" + chat.userCreator
                        + "\", \"name\":\"" + chat.name
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages
                        + "\",  \"type\":\"" + chat.type
                        + "\", \"participants\":[");
            for(auto & participant : chat.participants){
                data.append("{\"participant\":\"" + participant + "\"},");
            }
            data.remove(data.length()-1,1);
            data.append("]},");
        }
        data.remove(data.length()-1,1);
//        data.remove(data.length()-1,1);
        data.append("]}");
        emit writeSocketInProcess(socket, data);
    } else {
        sockWrite(socket, process, signal,
                  "\"chat\":\"-1\"");
    }
}
void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, ClientChat chat)
{
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                        + "\", \"chatID\":\"" + chat.chatID.toLocal8Bit()
                        + "\", \"userCreator\":\"" + chat.userCreator.toLocal8Bit()
                        + "\", \"name\":\"" + chat.name.toLocal8Bit()
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages.toLocal8Bit()
                        + "\",  \"type\":\"" + chat.type.toLocal8Bit()
                        + "\", \"participants\":[";
        for(auto & participant : chat.participants){
            data.append("{\"participant\":\"" + participant + "\"},");
        }
        data.remove(data.length()-1,1);
        data.append("]}");
        qDebug() << "in sockWrite chat";
        emit writeSocketInProcess(socket, data);
}
void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, QList<ClientMessage> conntents)
{
    if(conntents.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"conntents\":[";

        for(auto & conntent : conntents) {
            data.append("{\"messageID\":\"" + conntent.messageID
                        + "\", \"chatID\":\"" + conntent.chatID
                        + "\", \"senderID\":\"" + conntent.senderID
                        + "\", \"senderName\":\"" + conntent.senderName
                        + "\", \"isRead\":\"" + conntent.isRead
                        + "\", \"isSystem\":\"" + conntent.isSystem
                        + "\", \"message\":\"" + conntent.message + "\"},");

        }
        data.remove(data.length()-1,1);
        data.append("]}");
         qDebug() << "in sockWrite messages";
        emit writeSocketInProcess(socket, data);
    } else {
        sockWrite(socket, process, signal,
                  "\"conntent\":\"-1\"");
    }
}
void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, ClientMessage conntent)
{
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                        + "\", \"messageID\":\"" + conntent.messageID.toLocal8Bit()
                        + "\", \"chatID\":\"" + conntent.chatID.toLocal8Bit()
                        + "\", \"senderID\":\"" + conntent.senderID.toLocal8Bit()
                        + "\", \"senderName\":\"" + conntent.senderName.toLocal8Bit()
                        + "\", \"isRead\":\"" + conntent.isRead.toLocal8Bit()
                        + "\", \"isSystem\":\"" + conntent.isSystem.toLocal8Bit()
                        + "\", \"message\":\"" + conntent.message.toLocal8Bit() +"\"}";
        qDebug() << "in sockWrite message";
        emit writeSocketInProcess(socket, data);
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
void Process::sockWrite(QTcpSocket* socket, QString process, QString signal, ClientInfo clientInfo)
{
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() +
                    + "\", \"login\":\"" + clientInfo.login.toLocal8Bit()
                    + "\", \"name\":\"" + clientInfo.name.toLocal8Bit()
                    + "\", \"userID\":\"" + clientInfo.userID.toLocal8Bit() + "\"}";
    emit writeSocketInProcess(socket, data);
}

void Process::printClientInfo(QString header, ClientInfo clientInfo)
{
    qDebug()<< header;
    qDebug()<< "Логин: "<< clientInfo.login;
    qDebug()<< "Пароль: "<< clientInfo.password;
    qDebug()<< "Имя: "<< clientInfo.name;
}
