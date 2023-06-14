#ifndef USERMESSAGE_H
#define USERMESSAGE_H
#include <QString>
class UserMessage
{
public:
    QString messageID;
    QString chatID;
    QString senderName;
    QString senderID;
    QString message;
    QString isRead;
    QString isSystem = "0";
    QString date;
    QString time;
};
#endif // USERMESSAGE_H
