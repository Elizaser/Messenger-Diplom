#ifndef MAINPROCESS_H
#define MAINPROCESS_H
#include "process.h"

class MainProcess: public Process
{
    Q_OBJECT
    QList<QThread*> threads;
    ClientInfo curClientInfo;
    QMap <qintptr, QTcpSocket*> sockets;
    QTcpSocket* socket;
    qintptr sockDeskriptor;
    ClientInfo fillCurClientInfo(ClientInfo clientInfo);
    void sendingUserChats();
    void sendingFoundUsers(QString searchedUser, QString signal);
    void sendingFoundChats(QString searchedUser);
    void sendingChatContent(QString chatID);
    void sendMessage(ClientMessage message);

    void createChat(ClientChat chat);
    ClientMessage saveMessage(ClientMessage message);
    void sendOnlineUsersInChat(QString signal, QString chatID, auto message);
//    void sendOnlineUsers(QString signal, auto message);

    void deleteChat(QString chatID);
    void deleteMessage(QString messageID);
    void deleteMessageForEveryone(QString messageID);
public:
    MainProcess(QMap <qintptr, QTcpSocket*> sockets, QTcpSocket* socket, WorkDataBase* db);
    void sendingData(DataParsing messageFromClient);
    void updateListSockets(qintptr sockDescriptor, QTcpSocket* sockets);
    void deleteInListSockets(qintptr sockDescriptor);
};

#endif // MAINPROCESS_H
