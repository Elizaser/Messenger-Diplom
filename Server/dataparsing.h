#ifndef DATAPARSING_H
#define DATAPARSING_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QJsonParseError>
#include "clientinfo.h"
#include "clientmessage.h"
#include "clientchats.h"

class DataParsing
{
    QJsonDocument jDoc;
    QJsonParseError jDocError;
public:
    DataParsing(QByteArray data);

    QString getProccess();
    QString getSignal();

    QString getUserID();
    QString getChatID();
    QString getMessageID();
    QString getMessageText();

    QString getSearchedUser();

    ClientInfo getClient();
    ClientMessage getMessage();
    ClientChat getChat();

    QList<ClientChat> getChats();
    QList<ClientMessage> getMessages();
    QList<ClientInfo> getClients();
};

#endif // DATAPARSING_H
