#ifndef CLIENTCHATS_H
#define CLIENTCHATS_H

#include "QString"
class ClientChats
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QList<QString> participants;
    QString countIsNotReadMessages;
};
#endif // CLIENTCHATS_H
