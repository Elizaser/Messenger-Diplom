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
        curClientInfo = fillCurClientInfo(messageFromClient.getClientInfo());
//        printClientInfo("В главном процессе клиент ", curClientInfo);
        sendingUserChats();
    } else if(signal == "getUserChats"){
        sendingUserChats();
    } else if(signal == "searchPeople"){
        sendingFoundUsers(messageFromClient.getSearchedUser());
    } else if(signal == "searchChats"){
        sendingFoundChats(messageFromClient.getSearchedUser());
    } else if(signal == "getChatContent") {
        sendingChatContent(messageFromClient.getChatID());
    } else if(signal == "sendMessage"){
        sendMessage(messageFromClient.getMessage());
    } else if(signal == "messageEddit"){
        db->updateMessageEddit(messageFromClient.getMessageID(), messageFromClient.getMessageText());
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(messageFromClient.getChatID(), curClientInfo.userID);
//        qDebug() << "Онлайн пользователи, которые должны получить сообщение";
        foreach (qintptr key, onlineUsers.keys()) {
//            qDebug() << key << ":" << onlineUsers.value(key).name << ": " << sockets[key];
            sockWrite(sockets[key], "main", "updateMessage", "\"messageID\":\"" + messageFromClient.getMessageID()
                      + "\", \"message\":\"" + messageFromClient.getMessageText() + "\"");
        }
    } else if(signal == "deleteChat"){
        deleteChat(messageFromClient.getChatID());
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(messageFromClient.getChatID(), curClientInfo.userID);
        foreach (qintptr key, onlineUsers.keys()) {
//            qDebug() << key << ":" << onlineUsers.value(key).name << ": " << sockets[key];
            QList<ClientInfo> clientInfo;
            clientInfo.append(curClientInfo);
            sockWrite(sockets[key], "main", "deleteParticipant", clientInfo);
        }
    } else if(signal == "messageDelete"){
        deleteMessage(messageFromClient.getMessageID());
    } else if(signal == "messageDeleteForEveryone"){
        deleteMessageForEveryone(messageFromClient.getMessageID());
    } else if(signal == "isReadingMessage") {
        db->updateUserIsReadingMessages(messageFromClient.getChatID(), curClientInfo.userID);

    } else {
          qDebug()<<"Ошибка. Непонятно имя сигнала";
    }
}

void MainProcess:: sendingUserChats()
{
    QList <ClientChats> chats = db->getUserChats(curClientInfo.userID);
    sockWrite(socket, "main", "setUserChats", chats);
}

void MainProcess:: sendingFoundUsers(QString searchedUser)
{
    qDebug ()<< "searchedUser = " << searchedUser;
    QList<ClientInfo> foundUsers = db->getFoundUsers(searchedUser);
    sockWrite(socket, "main", "searchPeople", foundUsers);

}
void MainProcess:: sendingFoundChats(QString searchedUser)
{
    qDebug ()<< "searchedUser = " << searchedUser;
    QList<ClientChats> foundChats = db->getFoundChats(searchedUser);
    sockWrite(socket, "main", "searchChats", foundChats);
}

void MainProcess:: sendingChatContent(QString chatID)
{
    QList<ClientMessage> conntents = db->getChatConntent(chatID, curClientInfo.userID);
    sockWrite(socket, "main", "setChatContent", conntents);
}

void MainProcess:: sendMessage(ClientMessage message)
{
//    while(message.chatID == ""){
//        message.chatID = createChat(message);
//    }
    sendMessageOnlineUsers(saveMessage(message));
}
void MainProcess:: sendMessageOnlineUsers(ClientMessage message)
{
    qDebug() << "sendMessageOnlineUsers ins message.messageID = " << message.messageID;
    if(message.messageID != ""){
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(message.chatID, curClientInfo.userID);
        qDebug() << "Онлайн пользователи, которые должны получить сообщение";
        foreach (qintptr key, onlineUsers.keys()) {
            qDebug() << key << ":" << onlineUsers.value(key).name << ": " << sockets[key];
            QList <ClientMessage> messages;
            if(db->getNameUser(message.senderName) != "")
                message.senderName = db->getNameUser(message.senderName);
            messages.append(message);
            sockWrite(sockets[key], "main", "newMessage", messages);
        }
    }
}
QString MainProcess:: createChat(ClientMessage message)
{
    message.chatID = db->insertChat(message);
    if (message.chatID != ""){
         qDebug()<<"Чат успешно создан";
         qDebug()<<"  Обновление чатов пользователя";
         sendingUserChats();
    } else {
        qDebug()<<"Ошибка при попытки создать чат";
    }
    return message.chatID;
}

ClientMessage MainProcess:: saveMessage(ClientMessage message)
{
    if(db->insertMessage(message)){
         qDebug() << "saveMessage ins message.messageID = " << message.messageID;
        qDebug()<<"Сообщение успешно записано";
    } else{
     qDebug()<<" Не удалось записать сообщение";
    }
     return message;
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
    if(db->deleteChat(chatID, curClientInfo.userID))
        qDebug() << "Чат успешно удален";
}

void MainProcess:: deleteMessage(QString messageID)
{
    if(db->deleteMessage(messageID, curClientInfo.userID)){
        qDebug() << "Сообщение успешно удалено у пользователя";
    }
}

void MainProcess:: deleteMessageForEveryone(QString messageID)
{
    QString chatID = db->deleteMessageForEveryone(messageID);
    if(chatID != ""){
        qDebug() << "Сообщение успешно у всех удалено";
        QMap<qintptr, ClientInfo> onlineUsers = db->getOnlineUsersInChat(chatID, curClientInfo.userID);
//        qDebug() << "Онлайн пользователи, которые должны получить сообщение";
        foreach (qintptr key, onlineUsers.keys()) {
//            qDebug() << key << ":" << onlineUsers.value(key).name << ": " << sockets[key];
            sockWrite(sockets[key], "main", "deleteMessage", "\"messageID\":\"" + messageID + "\"");
        }
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
    qDebug()<<"Controller updateListSockets";
    qDebug()<<"Controller sockDescriptor = " << sockDescriptor;
    this->sockets.insert(sockDescriptor, socket);
}
void MainProcess::deleteInListSockets(qintptr sockDescriptor)
{
    qDebug()<<"Controller deleteInListSockets";
    this->sockets.remove(sockDescriptor);
}
