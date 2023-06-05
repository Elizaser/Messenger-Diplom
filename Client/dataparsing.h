#ifndef DATAPARSING_H
#define DATAPARSING_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QJsonParseError>
//#include <QStandardItem>
#include <QMessageBox>
#include <QTcpSocket>
#include "userchat.h"
#include "userinfo.h"
#include "usermessage.h"

class DataParsing
{
    QJsonDocument jDoc;
    QJsonParseError jDocError;
public:
    DataParsing(QByteArray data);

    QString getProccess();
    QString getSignal();
    QString getMessageID();
    QString getMessageText();

    UserInfo getUserInfo();
    QList<UserChat> getUserChats();

    QList<UserInfo> getFoundUsers();

    QList<UserMessage> getChatContent();
};

#endif // DATAPARSING_H
