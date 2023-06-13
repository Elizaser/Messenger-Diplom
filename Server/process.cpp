#include "process.h"

Process::Process(WorkDataBase* db)
{
    this->db = db;
}

QByteArray Process::generateData(QString process, QString signal)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\"}";
    qDebug() << "data1 = " << data;
    return data;
}

QByteArray Process::generateData(QString process, QString signal, QString message)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", " + message.toLocal8Bit() + "}";
    qDebug() << "data2 = " << data;
    return data;
}

QByteArray Process::generateData(QString process, QString signal, QList<ClientChat> chats)
{
    QByteArray data = "";
    if(chats.size() > 0) {
        data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"chats\":[";
        for(auto & chat : chats) {
            data.append("{\"chatID\":\"" + chat.chatID +
                        "\", \"userCreator\":\"" + chat.userCreator
                        + "\", \"name\":\"" + chat.name
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages
                        + "\",  \"countIsLook\":\"" + chat.countIsLook
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
    } else {
        data = generateData(process, signal,
                  "\"chat\":\"-1\"");
    }
    qDebug() << "data chats = " << data;
    return data;
}
QByteArray Process::generateData(QString process, QString signal, ClientChat chat)
{
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                    + "\", \"chatID\":\"" + chat.chatID.toLocal8Bit()
                    + "\", \"userCreator\":\"" + chat.userCreator.toLocal8Bit()
                    + "\", \"name\":\"" + chat.name.toLocal8Bit()
                    + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages.toLocal8Bit()
                    + "\",  \"countIsLook\":\"" + chat.countIsLook.toLocal8Bit()
                    + "\",  \"type\":\"" + chat.type.toLocal8Bit()
                    + "\", \"participants\":[";
    for(auto & participant : chat.participants){
        data.append("{\"participant\":\"" + participant + "\"},");
    }
    data.remove(data.length()-1,1);
    data.append("]}");
    return data;
}
QByteArray Process::generateData(QString process, QString signal, QList<ClientMessage> conntents)
{
    QByteArray data = "";
    if(conntents.size() > 0) {
        data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"conntents\":[";

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
    } else {
        data = generateData(process, signal,
                  "\"conntent\":\"-1\"");
    }
    return data;
}
QByteArray Process::generateData(QString process, QString signal, ClientMessage conntent)
{
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                    + "\", \"messageID\":\"" + conntent.messageID.toLocal8Bit()
                    + "\", \"chatID\":\"" + conntent.chatID.toLocal8Bit()
                    + "\", \"senderID\":\"" + conntent.senderID.toLocal8Bit()
                    + "\", \"senderName\":\"" + conntent.senderName.toLocal8Bit()
                    + "\", \"isRead\":\"" + conntent.isRead.toLocal8Bit()
                    + "\", \"isSystem\":\"" + conntent.isSystem.toLocal8Bit()
                    + "\", \"message\":\"" + conntent.message.toLocal8Bit() +"\"}";
    return data;
}
QByteArray Process::generateData(QString process, QString signal, QList<ClientInfo> clientInfos)
{
    QByteArray data = "";
    if(clientInfos.size() > 0) {
        qDebug() << "data clients 00 = " << data;
        data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"foundUsers\":[";
        for(auto & clientInfo : clientInfos) {
            data.append("{\"login\":\"" + clientInfo.login +
                        "\", \"name\":\"" + clientInfo.name +
                        "\", \"userID\":\"" + clientInfo.userID +"\"},");
        }
        data.remove(data.length()-1,1);
        data.append("]}");
    } else {
        qDebug() << "data clients 01 = " << data;
        data = generateData(process, signal, "\"foundUser\":\"-1\"");
    }
    qDebug() << "data clients = " << data;
    return data;
}
QByteArray Process::generateData(QString process, QString signal, ClientInfo clientInfo)
{
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() +
                    + "\", \"login\":\"" + clientInfo.login.toLocal8Bit()
                    + "\", \"name\":\"" + clientInfo.name.toLocal8Bit()
                    + "\", \"userID\":\"" + clientInfo.userID.toLocal8Bit() + "\"}";
    return data;
}
QByteArray Process::generateData(QString process, QString signal, QList<ClientMessage> conntents, ClientChat chat)
{
    QByteArray data  = generateData(process, signal, conntents);
    data.remove(data.length()-1,1).append(", ");
//    data.append("\"chatID\":\"" + chatID + "\"}");
    data += "\"chatID\":\"" + chat.chatID.toLocal8Bit()
                    + "\", \"userCreator\":\"" + chat.userCreator.toLocal8Bit()
                    + "\", \"name\":\"" + chat.name.toLocal8Bit()
                    + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages.toLocal8Bit()
                    + "\",  \"countIsLook\":\"" + chat.countIsLook.toLocal8Bit()
                    + "\",  \"type\":\"" + chat.type.toLocal8Bit()
                    + "\", \"participants\":[";
    for(auto & participant : chat.participants){
        data.append("{\"participant\":\"" + participant + "\"},");
    }
    if(chat.participants.size() > 0){
        data.remove(data.length()-1,1);
    }
    data.append("]}");

    qDebug() << "super data = " << data;
    return data;
}
void Process::sockWrite(QTcpSocket* socket, QByteArray data)
{
    emit writeSocketInProcess(socket, data);

}
void Process::printClientInfo(QString header, ClientInfo clientInfo)
{
    qDebug()<< header;
    qDebug()<< "Логин: "<< clientInfo.login;
    qDebug()<< "Пароль: "<< clientInfo.password;
    qDebug()<< "Имя: "<< clientInfo.name;
}
