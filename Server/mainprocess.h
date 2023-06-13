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
    void sendingDialogContent(QString companionUserID);
    void sendingMessage(ClientMessage message);
    void updateMessage(ClientMessage message);

    void createChat(ClientChat chat);
    void sendingMessageInNewDialog(ClientMessage message);
    ClientMessage saveMessage(ClientMessage message);
    void sendOnlineUsersInChat(QString chatID, QByteArray data);
//    void sendOnlineUsersInChatExceptMe(QString signal, QString chatID, auto  message);
    void sendOnlineUsers(QByteArray data);

    void deleteChat(QString chatID);
    void exitChat(QString chatID);
    void edditUser(ClientInfo client);
    void deleteMessage(ClientMessage message);
    void deleteMessageForEveryone(ClientMessage message);
    void updateIsReadingMessages(ClientChat chat);
public:
    MainProcess(QMap <qintptr, QTcpSocket*> sockets, QTcpSocket* socket, WorkDataBase* db);
    void sendingData(DataParsing messageFromClient);
    void updateListSockets(qintptr sockDescriptor, QTcpSocket* sockets);
    void deleteInListSockets(qintptr sockDescriptor);
};

#endif // MAINPROCESS_H
