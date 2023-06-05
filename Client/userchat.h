#ifndef USERCHAT_H
#define USERCHAT_H
#include "QString"
class UserChat
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QList<QString> participants;
    QString countIsNotReadMessages;
};
#endif // USERCHAT_H
