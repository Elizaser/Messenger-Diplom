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

    bool insertNewOnlineUser(QString socket, QString userID);
    bool deleteOnlineUser(QString socket);

    QString getNameUser(QString userID);

    ClientInfo getUserInfo(QString login);
    QList<ClientChats> getUserChats(QString login);
    QList<ClientMessage> getChatConntent(QString chatID, QString curUserID);

    QList<ClientInfo> getFoundUsers(QString desired);
    QList<ClientChats> getFoundChats(QString desired);

    bool insertMessage(ClientMessage &message);
    QString insertChat(ClientMessage message);
    bool insertUser(QString name, QString login, QString password);

    bool isCorrectInfoClient(QString login, QString password);
    bool isClientExist(QString login);
    QMap<qintptr, ClientInfo> getOnlineUsersInChat(QString chatID, QString userID);

    bool deleteChat(QString chatID, QString userID);
    bool deleteMessage(QString messageID, QString userID);
    QString deleteMessageForEveryone(QString messageID);

    bool updateUserIsReadingMessages(QString chatID, QString curUserID);
    bool updateMessageEddit(QString messageID, QString message);
};

#endif // WORKDATABASE_H
