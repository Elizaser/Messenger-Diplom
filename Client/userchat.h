#ifndef USERCHAT_H
#define USERCHAT_H
#include "QString"
#include "QList"
class UserChat
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QList<QString> participants;
    QString countIsNotReadMessages;
    QString type;
};
#endif // USERCHAT_H
