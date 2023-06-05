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
};
#endif // CLIENTMESSAGE_H
