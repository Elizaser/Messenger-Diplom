#ifndef DATAPARSING_H
#define DATAPARSING_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QJsonParseError>
#include "clientinfo.h"
#include "clientmessage.h"

class DataParsing
{
    QJsonDocument jDoc;
    QJsonParseError jDocError;
public:
    DataParsing(QByteArray data);

    QString getProccess();
    QString getSignal();

    ClientInfo getClientInfo();
    QString getChatID();
    QString getMessageID();
    QString getMessageText();

    QString getSearchedUser();
    ClientMessage getMessage();
};

#endif // DATAPARSING_H
