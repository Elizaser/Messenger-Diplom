#ifndef CLIENTCHATS_H
#define CLIENTCHATS_H

#include "QString"
#include "QMap"
class ClientChat
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QMap<QString, QString> participants;//id, name
//    QMap <qintptr, QString>participants;
    QString countIsNotReadMessages;
    QString countIsLook;
    QString type;
};
#endif // CLIENTCHATS_H
