#include "dataparsing.h"
#include <QDebug>
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

ClientInfo DataParsing::getClientInfo(){
    ClientInfo clientInfo;
    clientInfo.login = jDoc.object().value("login").toString();
    clientInfo.password = jDoc.object().value("password").toString();
    clientInfo.name = jDoc.object().value("name").toString();

    return clientInfo;
}


QString DataParsing::getChatID()
{
    return jDoc.object().value("chatID").toString();
}
QString DataParsing::getMessageID()
{
    return jDoc.object().value("messageID").toString();
}
QString DataParsing::getMessageText()
{
    return jDoc.object().value("message").toString();
}
QString DataParsing::getSearchedUser()
{
    return jDoc.object().value("user").toString();
}

ClientMessage DataParsing:: getMessage()
{
    ClientMessage message;
    message.chatID = jDoc.object().value("chatID").toString();
    message.senderID = jDoc.object().value("senderID").toString();
    qDebug() << " message.senderID = " <<  message.senderID;
    message.senderName = jDoc.object().value("senderName").toString();
    message.message = jDoc.object().value("message").toString();

    return message;
}



