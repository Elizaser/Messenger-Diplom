#include "mainprocess.h"
#include "QThread"

MainProcess::MainProcess(QMap <qintptr, QSslSocket*> sockets, QSslSocket*socket, WorkDataBase* db):Process(db)
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
        sendingUserChats();
    } else if(signal == "getUserChats"){
        sendingUserChats();
    } else if(signal == "getAllUsers"){
        sendingFoundUsers("", "setAllUsers");// если поиск по имени  осуществляется с пустой строкой, то просто вернется все что есть
    } else if(signal == "searchPeople"){
        sendingFoundUsers(messageFromClient.getSearchedUser(), "searchPeople");
    } else if(signal == "searchChats"){
        sendingFoundChats(messageFromClient.getSearchedUser());
    } else if(signal == "getChatContent") {
        sendingChatContent(messageFromClient.getChatID());
    } else if(signal == "getDialogContent") {
        sendingDialogContent(messageFromClient.getUserID());
    } else if(signal == "sendMessage"){
        sendingMessage(messageFromClient.getMessage());
    } else if(signal == "sendMessageInNewDialog") {
        sendingMessageInNewDialog(messageFromClient.getMessage());
    } else if(signal == "messageEddit"){
        updateMessage(messageFromClient.getMessage());
    } else if(signal == "deleteChat") {
        deleteChat(messageFromClient.getChatID());
    } else if(signal == "exitChat") {
        exitChat(messageFromClient.getChatID());
    } else if(signal == "messageDelete"){
        deleteMessage(messageFromClient.getMessage());
    } else if(signal == "messageDeleteForEveryone"){
        deleteMessageForEveryone(messageFromClient.getMessage());
    } else if(signal == "isReadingMessage") {
        updateIsReadingMessages(messageFromClient.getChat());
    } else if(signal == "getUsersCreateChat") {
        sendingFoundUsers(messageFromClient.getSearchedUser(), "setUsersCreateChat");// поиск по имени если осуществляется с пустой строкой, просто вернет все что есть
    } else if(signal == "createChat") {
        createChat(messageFromClient.getChat());
    } else if(signal == "edditUser") {
        edditUser(messageFromClient.getClient());
    } else {
          qDebug()<<"Ошибка. Непонятно имя сигнала";
    }
}
void MainProcess::sendOnlineUsersInChat(QString chatID, QByteArray data)
{
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(chatID);
        foreach (qintptr key, onlineUsers.keys()) {
            if(sockets[key]->isOpen())
                sockWrite(sockets[key], data);
        }
}
//void MainProcess::sendOnlineUsersInChatExceptMe(QString signal, QString chatID, auto  message)
//{
//        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(chatID);
//        foreach (qintptr key, onlineUsers.keys()) {
//            if(sockets[key]->isOpen() && socket != sockets[key])
//                generateData(sockets[key], "main", signal, message);
//        }
//}

void MainProcess:: sendOnlineUsers(QByteArray  data)
{
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsers();
        foreach (qintptr key, onlineUsers.keys()) {
            sockWrite(sockets[key], data);
        }
}

void MainProcess:: sendingUserChats()
{
    QList <ClientChat> chats = db->getUserChats(curClientInfo.userID);
    sockWrite(socket, generateData("main", "setUserChats", chats));
}
void MainProcess::sendingFoundUsers(QString searchedUser, QString signal)
{
    QList<ClientInfo> foundUsers = db->getFoundUsers(searchedUser, curClientInfo.userID);
    sockWrite(socket, generateData("main", signal, foundUsers));
}
void MainProcess::sendingFoundChats(QString searchedUser)
{
    QList<ClientChat> foundChats = db->getFoundChats(searchedUser, curClientInfo.userID);
    sockWrite(socket, generateData("main", "searchChats", foundChats));
}
void MainProcess:: sendingChatContent(QString chatID)
{
    QList<ClientMessage> conntents = db->getChatConntent(chatID, curClientInfo.userID);
    sockWrite(socket, generateData("main", "setChatContent", conntents));
}
void MainProcess::sendingDialogContent(QString companionUserID)
{
    QString chatID = db->getDialogID(companionUserID, curClientInfo.userID);
    QList<ClientMessage> conntents = db->getDialogConntent(companionUserID, curClientInfo.userID);
    ClientChat chat = db->getChat(chatID, curClientInfo.userID);
    sockWrite(socket, generateData("main", "setChatContent", conntents, chat));
}
void MainProcess::sendingMessage(ClientMessage message)
{
    message = saveMessage(message);

    qDebug() << "message.messageID = " << message.messageID;// без этой строки дает сбой при отправки в новое сообщение не отправляет сигнал о новом чате
    if(message.messageID == "") return;
    sendOnlineUsersInChat(message.chatID, generateData("main", "newMessage", message));
}
ClientMessage MainProcess::saveMessage(ClientMessage message)
{
    message.senderID = curClientInfo.userID;
    message.senderName = curClientInfo.name;
//    if(message.isSystem == "")
//        message.isSystem = "0";
    message.date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
    message.time = QDateTime::currentDateTime().toString("hh:mm");
    return db->insertMessage(message);
}
void MainProcess::sendingMessageInNewDialog(ClientMessage message)
{
    QString companionID = message.senderID;
    QString companionName = message.senderName;
    message.senderID = curClientInfo.userID;
    message.senderName = curClientInfo.name;
    ClientChat chat;
    chat.name = message.senderName + " - " + companionName;
    chat.userCreator = curClientInfo.userID;
    chat.type = "dialog";
    chat.countIsNotReadMessages = "0";
    chat.countIsLook = "1";
    chat.participants.insert(companionID, companionName);
    chat.participants.insert(curClientInfo.userID, curClientInfo.name);
    chat = db->insertChat(chat);
    if (chat.chatID == ""){
        return;
    }
    message.chatID = chat.chatID;
    message = saveMessage(message);
    if(message.messageID == "") return;
    sendOnlineUsersInChat(chat.chatID, generateData("main", "newDialog", chat));
    sendOnlineUsersInChat(message.chatID, generateData("main", "newMessage", message));
}
void MainProcess::updateMessage(ClientMessage message)
{
    db->updateMessageEddit(message);
    sendOnlineUsersInChat(message.chatID, generateData("main", "updateMessage", message));
}
void MainProcess::deleteMessage(ClientMessage message)
{
    db->deleteMessage(message.messageID, curClientInfo.userID);
    sockWrite(socket, generateData("main", "deleteMessage", message));
}
void MainProcess::deleteMessageForEveryone(ClientMessage message)
{
    message.chatID = db->deleteMessageForEveryone(message.messageID);
    if(message.chatID != ""){
        sendOnlineUsersInChat(message.chatID, generateData("main", "deleteMessage", message));
    }
}
void MainProcess::updateIsReadingMessages(ClientChat chat)
{
//    if(chat.countIsNotReadMessages != "0"){
        db->updateUserIsReadingMessages(chat.chatID, curClientInfo.userID);
        sendOnlineUsersInChat(chat.chatID, generateData("main", "updateIsReadingMessages", "\"chatID\":\"" + chat.chatID + "\""));
//    }
}


void MainProcess::createChat(ClientChat chat)
{
    chat.userCreator = curClientInfo.userID;
    chat.type = "group";
    chat.countIsNotReadMessages = "0";
    chat.participants.insert(curClientInfo.userID, curClientInfo.name);
    chat = db->insertChat(chat);
    if (chat.chatID == ""){
        return;
    }
    sendOnlineUsersInChat(chat.chatID, generateData("main", "newChat", chat));

    ClientMessage message;
    message.senderID = curClientInfo.userID;
    message.message = "Пользователь создал чат";
    message.isSystem = "1";
    message.chatID = chat.chatID;
    sendingMessage(message);
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
    sendingMessage(message);
    sendOnlineUsersInChat(chatID, generateData("main", "exitChat",  "\"chatID\":\"" + chatID + "\""));
}

void MainProcess::edditUser(ClientInfo client)
{
    if(db->updateUser(client)) {
        sendOnlineUsers(generateData("main", "updateEdditUser", client));
    }
}
ClientInfo MainProcess:: fillCurClientInfo(ClientInfo inClientInfo)
{
    ClientInfo outClientInfo = db->getUserInfo(inClientInfo.login);
    outClientInfo.login = inClientInfo.login;
    outClientInfo.password = inClientInfo.password;
    return outClientInfo;
}

void MainProcess::updateListSockets(qintptr sockDescriptor, QSslSocket* socket)
{
    this->sockets.insert(sockDescriptor, socket);
}
void MainProcess::deleteInListSockets(qintptr sockDescriptor)
{
    this->sockets.remove(sockDescriptor);
}
