#ifndef WORKDATABASE_H
#define WORKDATABASE_H

#include <QDebug>
#include<QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include "clientinfo.h"
#include "clientchats.h"
#include "clientmessage.h"
class WorkDataBase
{
    QSqlDatabase db;
public:
    WorkDataBase();
    ~WorkDataBase();
    void start(qintptr socketDeskriptor);
    void close(qintptr socketDeskriptor);

    bool insertNewOnlineUser(QString socket, QString userID);
    bool deleteOnlineUser(QString socket);

    QString getNameUser(QString userID);

    ClientInfo getUserInfo(QString login);
    QList<ClientChat> getUserChats(QString login);
    QList<ClientMessage> getChatConntent(QString chatID, QString curUserID);

    QList<ClientInfo> getFoundUsers(QString desired);
    QList<ClientChat> getFoundChats(QString desired, QString curUserID);

    ClientMessage insertMessage(ClientMessage message);
    ClientChat insertChat(ClientChat chat);
    bool insertUser(QString name, QString login, QString password);

    bool isCorrectInfoClient(QString login, QString password);
    bool isClientExist(QString login);
    QMap<qintptr, ClientInfo> getOnlineUsersInChat(QString chatID);

    bool deleteChat(QString chatID, QString userID);
    bool exitChat(QString chatID, QString userID);
    bool deleteMessage(QString messageID, QString userID);
    QString deleteMessageForEveryone(QString messageID);

    bool updateUserIsReadingMessages(QString chatID, QString curUserID);
    bool updateMessageEddit(ClientMessage message);
};

#endif // WORKDATABASE_H
