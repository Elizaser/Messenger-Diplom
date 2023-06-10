#include "mainprocess.h"
#include "QThread"

MainProcess::MainProcess(QMap <qintptr, QTcpSocket*> sockets, QTcpSocket*socket, WorkDataBase* db):Process(db)
{
    this->threads= threads;
    this->socket = socket;
    this->sockets = sockets;
}

void MainProcess::sendingData(DataParsing messageFromClient)
{
    QString signal  = messageFromClient.getSignal();
    qDebug() << "signal = " << signal;
    if(signal == "setUserInfo") {
        curClientInfo = fillCurClientInfo(messageFromClient.getClient());
//        printClientInfo("В главном процессе клиент ", curClientInfo);
        sendingUserChats();
    } else if(signal == "getUserChats"){
        sendingUserChats();
    } else if(signal == "searchPeople"){
        sendingFoundUsers(messageFromClient.getSearchedUser(), "searchPeople");
    } else if(signal == "searchChats"){
        sendingFoundChats(messageFromClient.getSearchedUser());
    } else if(signal == "getChatContent") {
        sendingChatContent(messageFromClient.getChatID());
    } else if(signal == "sendMessage"){
        sendMessage(messageFromClient.getMessage());
    } else if(signal == "messageEddit"){
        db->updateMessageEddit(messageFromClient.getMessage());
        sendOnlineUsersInChat("updateMessage", messageFromClient.getMessage().chatID, messageFromClient.getMessage());
    } else if(signal == "deleteChat") {
        deleteChat(messageFromClient.getChatID());
    } else if(signal == "exitChat") {
        exitChat(messageFromClient.getChatID());
        sendOnlineUsersInChat("exitChat", messageFromClient.getChatID(), "\"chatID\":\"" + messageFromClient.getChatID() + "\"");
    } else if(signal == "messageDelete"){
        deleteMessage(messageFromClient.getMessage().messageID);
        sockWrite(socket, "main", "deleteMessage", messageFromClient.getMessage());
    } else if(signal == "messageDeleteForEveryone"){
        deleteMessageForEveryone(messageFromClient.getMessage().messageID);
        sendOnlineUsersInChat("deleteMessage", messageFromClient.getMessage().chatID, messageFromClient.getMessage());
    } else if(signal == "isReadingMessage") {
        db->updateUserIsReadingMessages(messageFromClient.getChatID(), curClientInfo.userID);
    } else if(signal == "getUsersCreateChat") {
        sendingFoundUsers(messageFromClient.getSearchedUser(), "setUsersCreateChat");// поиск по имени если осуществляется с пустой строкой, просто вернет все что есть
    } else if(signal == "createChat") {
        createChat(messageFromClient.getChat());
    } else {
          qDebug()<<"Ошибка. Непонятно имя сигнала";
    }
}
void MainProcess:: sendOnlineUsersInChat(QString signal, QString chatID, auto  message)
{
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(chatID);
        foreach (qintptr key, onlineUsers.keys()) {
            sockWrite(sockets[key], "main", signal, message);
        }
}
//void MainProcess:: sendOnlineUsers(QString signal, auto  message)
//{
//        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsers();
//        foreach (qintptr key, onlineUsers.keys()) {
//            sockWrite(sockets[key], "main", signal, message);
//        }
//}
void MainProcess:: sendingUserChats()
{
    QList <ClientChat> chats = db->getUserChats(curClientInfo.userID);
    sockWrite(socket, "main", "setUserChats", chats);
}

void MainProcess:: sendingFoundUsers(QString searchedUser, QString signal)
{
    QList<ClientInfo> foundUsers = db->getFoundUsers(searchedUser);
    sockWrite(socket, "main", signal, foundUsers);

}
void MainProcess:: sendingFoundChats(QString searchedUser)
{
    QList<ClientChat> foundChats = db->getFoundChats(searchedUser, curClientInfo.userID);
    sockWrite(socket, "main", "searchChats", foundChats);
}

void MainProcess:: sendingChatContent(QString chatID)
{
    QList<ClientMessage> conntents = db->getChatConntent(chatID, curClientInfo.userID);
    sockWrite(socket, "main", "setChatContent", conntents);
}

void MainProcess:: sendMessage(ClientMessage message)
{
    message = saveMessage(message);
    qDebug() << "message.senderID" << message.senderID;
    qDebug() << "message.senderName" << message.senderName;
    if(message.messageID == "") return;
    sendOnlineUsersInChat("newMessage", message.chatID, message);
}
void MainProcess:: createChat(ClientChat chat)
{
    chat.userCreator = curClientInfo.userID;
    chat.type = "group";
    chat.countIsNotReadMessages = "1";
    chat = db->insertChat(chat);
    if (chat.chatID == ""){
        return;
    }
    qDebug() << "chat.chatID = " << chat.chatID;
    sockWrite(socket, "main", "newChat", chat);

    ClientMessage message;
    message.senderID = curClientInfo.userID;
    message.message = "Пользователь создал чат";
    message.isSystem = "1";
    message.chatID = chat.chatID;
    sendMessage(message);
}

ClientMessage MainProcess:: saveMessage(ClientMessage message)
{
    message.senderID = curClientInfo.userID;
    message.senderName = curClientInfo.name;
    message.isSystem = "0";
    return db->insertMessage(message);
}
void MainProcess:: deleteChat(QString chatID)
{
    db->deleteChat(chatID, curClientInfo.userID);
}
void MainProcess:: exitChat(QString chatID)
{
    if(!db->exitChat(chatID, curClientInfo.userID)) return;
    ClientMessage message;
    message.chatID = chatID;
    message.isRead = "0";
    message.message = " Пользователь вышел из беседы";
    message.senderID = curClientInfo.userID;
    message.senderName = curClientInfo.name;
    message.isSystem = "1";
    sendMessage(message);
}
void MainProcess:: deleteMessage(QString messageID)
{
    db->deleteMessage(messageID, curClientInfo.userID);
}

void MainProcess:: deleteMessageForEveryone(QString messageID)
{
    QString chatID = db->deleteMessageForEveryone(messageID);
    if(chatID != ""){
        sendOnlineUsersInChat( "deleteMessage", chatID, "\"messageID\":\"" + messageID + "\"");
    }
}

ClientInfo MainProcess:: fillCurClientInfo(ClientInfo inClientInfo)
{
    ClientInfo outClientInfo = db->getUserInfo(inClientInfo.login);
    outClientInfo.login = inClientInfo.login;
    outClientInfo.password = inClientInfo.password;
    return outClientInfo;
}

void MainProcess::updateListSockets(qintptr sockDescriptor, QTcpSocket* socket)
{
    this->sockets.insert(sockDescriptor, socket);
}
void MainProcess::deleteInListSockets(qintptr sockDescriptor)
{
    this->sockets.remove(sockDescriptor);
}
