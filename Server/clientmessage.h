#ifndef CLIENTMESSAGE_H
#define CLIENTMESSAGE_H
#include <QString>
class ClientMessage
{
public:
    QString messageID;
    QString chatID;
    QString senderName;
    QString senderID;
    QString message;
    QString isRead;
    QString isSystem;
    QString date;
    QString time;
};
#endif // CLIENTMESSAGE_H
