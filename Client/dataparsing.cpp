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
QString DataParsing::getChatID()
{
    return jDoc.object().value("chatID").toString();
}
QString DataParsing::getMessageText()
{
    return jDoc.object().value("message").toString();
}

UserInfo DataParsing::getUser()
{
    UserInfo userInfo;
    userInfo.userID = jDoc.object().value("userID").toString();
    userInfo.name = jDoc.object().value("name").toString();
    userInfo.login = jDoc.object().value("login").toString();
    userInfo.password = jDoc.object().value("password").toString();
    return userInfo;
}

QList<UserInfo>  DataParsing::getUsers()
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

QList<UserChat> DataParsing::getChats()
{
    QList<UserChat> chats;
    QString voidResult = jDoc.object().value("chat").toString();
    if(voidResult == ""){
        QJsonArray jsChats = jDoc.object().value("chats").toArray();
        for(int i = 0; i < jsChats.count();i++) {
            UserChat chat;
            chat.chatID = jsChats[i].toObject().value("chatID").toString();
            chat.name = jsChats[i].toObject().value("name").toString();
            chat.userCreator = jsChats[i].toObject().value("userCreator").toString();
            chat.countIsNotReadMessages = jsChats[i].toObject().value("countIsNotReadMessages").toString();
            chat.countIsLook = jsChats[i].toObject().value("countIsLook").toString();
            chat.type = jsChats[i].toObject().value("type").toString();

            QJsonArray jsParticipants = jsChats[i].toObject().value("participants").toArray();
            for(int j = 0; j < jsParticipants.count();j++){
                chat.participants.insert(jsParticipants[j].toObject().value("participantID").toString(), jsParticipants[j].toObject().value("participantName").toString());
            }

            chats.append(chat);
        }
    }
    return chats;
}
UserChat DataParsing::getChat()
{
    UserChat chat;
    chat.chatID = jDoc.object().value("chatID").toString();
    chat.name = jDoc.object().value("name").toString();
    chat.userCreator = jDoc.object().value("userCreator").toString();
    chat.countIsNotReadMessages = jDoc.object().value("countIsNotReadMessages").toString();
     chat.countIsLook = jDoc.object().value("countIsLook").toString();
    chat.type = jDoc.object().value("type").toString();

    QJsonArray jsParticipants = jDoc.object().value("participants").toArray();
    for(int j = 0; j < jsParticipants.count();j++){
        chat.participants.insert(jsParticipants[j].toObject().value("participantID").toString(), jsParticipants[j].toObject().value("participantName").toString());
    }
    return chat;
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
            conntent.isSystem = jsConntents[i].toObject().value("isSystem").toString();
            conntents.append(conntent);
        }
    }
    return conntents;
}
UserMessage DataParsing::getMessage()
{
    UserMessage message;
    message.chatID = jDoc.object().value("chatID").toString();
    message.messageID = jDoc.object().value("messageID").toString();
    message.senderName = jDoc.object().value("senderName").toString();
    message.senderID = jDoc.object().value("senderID").toString();
    message.message = jDoc.object().value("message").toString();
    message.isRead = jDoc.object().value("isRead").toString();
    message.isSystem = jDoc.object().value("isSystem").toString();
    return message;
}
