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
//        sendOnlineUsers("deleteParticipant", curClientInfo);
    } else if(signal == "messageDelete"){
        deleteMessage(messageFromClient.getMessageID());
    } else if(signal == "messageDeleteForEveryone"){
        deleteMessageForEveryone(messageFromClient.getMessageID());
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
//            if(db->getNameUser(message.senderName) != "")
//                message.senderName = db->getNameUser(message.senderName);
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
    QList<ClientChat> foundChats = db->getFoundChats(searchedUser);
    sockWrite(socket, "main", "searchChats", foundChats);
}

void MainProcess:: sendingChatContent(QString chatID)
{
    QList<ClientMessage> conntents = db->getChatConntent(chatID, curClientInfo.userID);
    sockWrite(socket, "main", "setChatContent", conntents);
}

void MainProcess:: sendMessage(ClientMessage message)
{
    sendOnlineUsersInChat("newMessage", message.chatID, saveMessage(message));
}
void MainProcess:: createChat(ClientChat chat)
{
    chat.userCreator = curClientInfo.userID;
    chat = db->insertChat(chat);
    if (chat.chatID == ""){
        return;
    }
    sockWrite(socket, "main", "newChat");

    ClientMessage message;
    message.senderID = curClientInfo.userID;
    message.message = "Пользователь создал чат";
    message.chatID = chat.chatID;
    sendMessage(message);
}

ClientMessage MainProcess:: saveMessage(ClientMessage message)
{
    return db->insertMessage(message);
}
void MainProcess:: deleteChat(QString chatID)
{
    ClientMessage message;
    message.chatID = chatID;
    message.isRead = "1";
    message.message = " Пользователь вышел из беседы";
    message.senderID = curClientInfo.userID;
    message.senderName = curClientInfo.name;
    sendMessage(message);
    db->deleteChat(chatID, curClientInfo.userID);
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
