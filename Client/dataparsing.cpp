#include "dataparsing.h"
#include "QDebug"
struct errorFormatJDoc{
    QString error;
};
DataParsing::DataParsing(QByteArray data)
{
    jDoc = QJsonDocument::fromJson(data, &jDocError);
    if(jDocError.errorString().toInt() != QJsonParseError::NoError) {
        throw jDocError.errorString();
    }
}

QString DataParsing::getProccess()
{
    return jDoc.object().value("process").toString();
}

QString DataParsing::getSignal()
{
    return jDoc.object().value("signal").toString();
}

QString DataParsing::getMessageID()
{
    return jDoc.object().value("messageID").toString();
}

QString DataParsing::getMessageText()
{
    return jDoc.object().value("message").toString();
}

UserInfo DataParsing::getUserInfo()
{
    UserInfo userInfo;
    userInfo.userID = jDoc.object().value("userID").toString();
    userInfo.name = jDoc.object().value("name").toString();
    userInfo.login = jDoc.object().value("login").toString();
    userInfo.password = jDoc.object().value("password").toString();
    return userInfo;
}

QList<UserInfo>  DataParsing::getFoundUsers()
{
    QList<UserInfo> foundUsers;
    QString voidResult = jDoc.object().value("foundUsers").toString();
    if(voidResult == ""){
        QJsonArray jsFoundUsers = jDoc.object().value("foundUsers").toArray();
        UserInfo foundUser;
        for(int i = 0; i < jsFoundUsers.count();i++) {
            foundUser.userID = jsFoundUsers[i].toObject().value("userID").toString();
            foundUser.login = jsFoundUsers[i].toObject().value("login").toString();
            foundUser.name = jsFoundUsers[i].toObject().value("name").toString();
            foundUsers.append(foundUser);
        }
    }
    return foundUsers;
}

QList<UserChat> DataParsing::getUserChats()
{
    QList<UserChat> chats;
    QString voidResult = jDoc.object().value("chat").toString();
    if(voidResult == ""){
        QJsonArray jsChats = jDoc.object().value("chats").toArray();
        for(int i = 0; i < jsChats.count();i++) {
            UserChat chat;
            chat.chatID = jsChats[i].toObject().value("chatID").toString();
            chat.name = jsChats[i].toObject().value("name").toString();
            qDebug() <<"chat.chatID" << chat.chatID;
            qDebug() <<"chat.name" << chat.name;
            chat.userCreator = jsChats[i].toObject().value("userCreator").toString();
            chat.countIsNotReadMessages = jsChats[i].toObject().value("countIsNotReadMessages").toString();
            qDebug() << jsChats[i].toObject().value("countIsNotReadMessages").toString();

            QJsonArray jsParticipants = jsChats[i].toObject().value("participants").toArray();
            for(int j = 0; j < jsParticipants.count();j++){
                chat.participants.append(jsParticipants[j].toObject().value("participant").toString());
            }

            chats.append(chat);
        }
    }
    return chats;
}

QList<UserMessage> DataParsing::getChatContent()
{
    QList <UserMessage> conntents;
    QString voidResult = jDoc.object().value("conntent").toString();
    if(voidResult == ""){
        QJsonArray jsConntents = jDoc.object().value("conntents").toArray();
        UserMessage conntent;
        for(int i = 0; i < jsConntents.count();i++) {
            conntent.messageID = jsConntents[i].toObject().value("messageID").toString();
            conntent.chatID = jsConntents[i].toObject().value("chatID").toString();
            conntent.senderName = jsConntents[i].toObject().value("senderName").toString();
            conntent.senderID = jsConntents[i].toObject().value("senderID").toString();
            conntent.message = jsConntents[i].toObject().value("message").toString();
            conntent.isRead = jsConntents[i].toObject().value("isRead").toString();
            conntents.append(conntent);
        }
    }
    return conntents;
}
