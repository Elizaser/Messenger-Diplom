#ifndef CLIENTCHATS_H
#define CLIENTCHATS_H

#include "QString"
#include "QList"
class ClientChat
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QList<QString> participants;
    QString countIsNotReadMessages;
    QString countIsLook;
    QString type;
};
#endif // CLIENTCHATS_H
