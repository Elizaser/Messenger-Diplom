#ifndef USERCHAT_H
#define USERCHAT_H
#include "QString"
#include "QMap"
class UserChat
{
public:
    QString chatID;
    QString userCreator;
    QString name;
    QMap<QString, QString> participants;//id, name
    QString countIsNotReadMessages;
    QString countIsLook;
    QString type;
};
#endif // USERCHAT_H
