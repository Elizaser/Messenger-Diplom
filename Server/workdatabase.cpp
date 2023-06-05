#include "workdatabase.h"
#include <QTextCodec>
#include <QThread>

WorkDataBase::WorkDataBase()
{
}
WorkDataBase::~WorkDataBase()
{
    db.close();
}
void WorkDataBase::start(qintptr socketDeskriptor)
{
    qDebug()<<"Name connection - " << socketDeskriptor;
    db = QSqlDatabase::addDatabase("QMYSQL", QString::number(socketDeskriptor));
    db.setHostName("localhost");
    db.setDatabaseName("messenger");
    db.setUserName("liza");
    db.setPassword("000000");
    if (db.open()) {
        qDebug()<<"Database open";
         db.exec("SET NAMES utf8");    } else {
        qDebug()<<"Error database: " + db.lastError().text();
    }
}
bool WorkDataBase::isCorrectInfoClient(QString login, QString password)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT EXISTS(SELECT * FROM User where login = '" + login +  "' and password = '" + password + "')");
    query->next();
    if(query->value(0).toString() != "0" && query->value(0).toString() != "") {
        return true;
    }
    return false;
}
bool WorkDataBase::isClientExist(QString login)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT EXISTS(SELECT * FROM User where login = '" + login + "')");
    query->next();

    if(query->value(0).toString() != "0" && query->value(0).toString() != "") {
        return true;
    }
    return false;
}

bool WorkDataBase::insertMessage(ClientMessage &message)
{
    qDebug() << " message.senderID = " <<  message.senderID;
    bool flag = false;
    QSqlQuery* query = new QSqlQuery(db);
//    QString messageID;
    if(query->exec( "INSERT INTO `Messages` (`messageID`, `chatID`, `sender`, `message`) VALUES (NULL, '"
                    + message.chatID + "', '" + message.senderID + "', '" + message.message + "')"))
    {
        query->exec( "SELECT messageID FROM `Messages` ORDER BY messageID DESC");
        query->next();
        message.messageID = query->value(0).toString();
        message.isRead = "0";
        qDebug() << "wdb ins message.messageID = " << message.messageID;
        QSqlQuery* query2 = new QSqlQuery(db);
        query2->exec( "SELECT participantID  FROM `Participants` where chatID =  '" +
                            message.chatID + "'");
        while(query2->next()) {
            flag = true;
            if(query->exec( "INSERT INTO `MessageStatus` (`messageID`, `userID`) VALUES ('"
                            + message.messageID + "', '" + query2->value(0).toString() + "')")){

            } else {
                flag = false;
                break;
            }
        }
    }
    if(!flag)
        query->exec("DELETE FROM `Messages` WHERE messageID = " + message.messageID);
    return flag;
}
QString WorkDataBase::insertChat(ClientMessage message)
{
//    QSqlQuery* query = new QSqlQuery(db);
//    query->exec("SELECT userID FROM User where login = '" + message.recipient  + "'");
//    query->next();
//    message.recipient = query->value(0).toString();
//    query->exec("SELECT userID FROM User where login = '" + message.sender  + "'");
//    query->next();
//    message.sender = query->value(0).toString();

//    if (query->exec("INSERT INTO `Chats` (`ChatID`, `user1`, `user2`) VALUES (NULL, " +
//                   message.sender + ", " + message.recipient + ")")) {
//        query->exec("SELECT chatID FROM Chats where (user1 = " + message.recipient + " and user2 = " + message.sender + ") or (user1 = " + message.sender + " and user2 = " + message.recipient +  ")");
//        query->next();
//        message.chatID = query->value(0).toString();
//        return message.chatID;
//    }
    return "";
}
bool WorkDataBase::insertUser(QString name, QString login, QString password)
{
    QSqlQuery* query = new QSqlQuery(db);
    if(query->exec("INSERT INTO `User` (`name`, `login`, `password`) VALUES ('" + name + "', " + "'" + login + "', '"  + password + "')")){
        return true;
    }
    return false;
}
bool WorkDataBase::insertNewOnlineUser(QString socket, QString userID)
{
    QSqlQuery* query = new QSqlQuery(db);
    if(query->exec("INSERT INTO `UsersOnline` (`userID`, `socket`) VALUES ('" +
                   userID + "', '" + socket + "')"))
    {

        return true;
    }
    return false;
}
bool WorkDataBase::deleteOnlineUser(QString socket)
{
    QSqlQuery* query = new QSqlQuery(db);
    if(query->exec("DELETE FROM `UsersOnline` WHERE socket = " + socket))
    {
        return true;
    }
    return false;
}

QString WorkDataBase::getNameUser(QString userID)
{
    QSqlQuery* query = new QSqlQuery(db);
    if(query->exec("SELECT name FROM User where userID = '" + userID + "'")){
        query->next();
        return query->value(0).toString();
    }
    return "";
}

ClientInfo WorkDataBase::getUserInfo(QString login)
{
    ClientInfo clientInfo;
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT userID, name FROM User where login = '" + login + "'");
    query->next();
    clientInfo.userID = query->value(0).toString();
    clientInfo.name = query->value(1).toString();
    return clientInfo;
}
QList<ClientChats> WorkDataBase::getUserChats(QString userID)
{
    QSqlQuery* query1 = new QSqlQuery(db);

    query1->exec("SELECT chatID FROM Participants where participantID = '" + userID + "' GROUP BY chatID");
    QList<ClientChats> chats;
    for(int i = 0; query1->next(); i++){
        ClientChats chat;
        chat.chatID = query1->value(0).toString();

        QSqlQuery* query2 = new QSqlQuery(db);

        query2->exec("SELECT userCreator, name FROM Chats where chatID = '" +  chat.chatID + "'");
        query2->next();
        chat.userCreator = query2->value(0).toString();
        chat.name = query2->value(1).toString();

        query2->exec("SELECT participantID FROM Participants where chatID = '" +  chat.chatID + "'");
        while(query2->next()){
            chat.participants.append(query2->value(0).toString());
        }

        query2->exec("SELECT messageID FROM Messages where chatID = '" +  chat.chatID + "'");
        int countIsLook = 0;
        int countIsNotReadMessages = 0;
        while(query2->next()){
            QSqlQuery* query3 = new QSqlQuery(db);
            query3->exec("SELECT isRead FROM MessageStatus where messageID = '" +  query2->value(0).toString() + "' and userID = '" + userID + "'");
            query3->next();
            if(query3->value(0).toString() == "0"){
                countIsNotReadMessages++;
            } else if(query3->value(0).toString() == ""){
                countIsLook--;
            }
            countIsLook++;
        }
        if(countIsLook <= 0)
            continue;
        chat.countIsNotReadMessages = QString::number(countIsNotReadMessages);
        chats.append(chat);
    }
    return chats;
}
QList<ClientMessage> WorkDataBase::getChatConntent(QString chatID, QString curUserID)
{
    QList<ClientMessage> conntents;
    ClientMessage conntent;
    QSqlQuery* query1 = new QSqlQuery(db);
    query1->exec("SELECT * FROM Messages where chatID = '" +  chatID + "'");
    for(int i = 0 ; query1->next(); i++){
        qDebug() <<"wdb conntent.messageID = " << conntent.messageID;
        conntent.messageID = query1->value(0).toString();
        conntent.chatID = query1->value(1).toString();
        conntent.senderID = query1->value(2).toString();
        conntent.message = query1->value(3).toString();
        QSqlQuery* query2 = new QSqlQuery(db);
        query2->exec("SELECT name FROM User WHERE userID = " + conntent.senderID);
        query2->next();
        conntent.senderName = query2->value(0).toString();
        query2->exec("SELECT isRead FROM MessageStatus where messageID = '" +
                     conntent.messageID + "' and userID = '" + curUserID + "'");
        query2->next();
        conntent.isRead = query2->value(0).toString();
        if(conntent.isRead == ""){
            continue;
        }
        conntents.append(conntent);
    }
    return conntents;
}
QMap<qintptr, ClientInfo> WorkDataBase::getOnlineUsersInChat(QString chatID, QString curUserID)
{
    QMap<qintptr, ClientInfo> users;
    QSqlQuery* query1 = new QSqlQuery(db);
    qDebug() << "curUserID" << curUserID;
    qDebug() << "chatID" << chatID;
    query1->exec("SELECT participantID FROM Participants where chatID = '" + chatID + "'");

    for(int i = 0 ; query1->next(); i++){
        ClientInfo user;
        qintptr socketID;
        user.userID = query1->value(0).toString();
        qDebug() << "\nuserID in chat" << user.userID;
        QSqlQuery* query2 = new QSqlQuery(db);
        query2->exec("SELECT EXISTS(SELECT socket FROM UsersOnline where userID = '" +  user.userID + "')");
        query2->next();
        if(query2->value(0).toString() != "0") {
            query2->exec("SELECT socket FROM UsersOnline where userID = '" +  user.userID + "'");
            query2->next();
            socketID = query2->value(0).toInt();
            qDebug() << "userID online" << user.userID;
            qDebug() << "socketID" << socketID;
            QSqlQuery* query3 = new QSqlQuery(db);
            query3->exec("SELECT name, login FROM User where userID = '" +  user.userID + "'");
            query3->next();
            user.name = query3->value(0).toString();
            user.userID = query3->value(1).toString();
            users.insert(socketID, user);
        }
    }
    return users;
}
QList<ClientChats> WorkDataBase::getFoundChats(QString desired)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT chatID, userCreator, name FROM Chats where name LIKE '%" + desired +  "%'");
    QList<ClientChats> chats;
    while(query->next()){
        ClientChats chat;
        chat.chatID = query->value(0).toString();
        chat.userCreator = query->value(1).toString();
        chat.name = query->value(2).toString();

        QSqlQuery* query2 = new QSqlQuery(db);
        query2->exec("SELECT participantID FROM Participants where chatID =  " + chat.chatID);
        while(query2->next()){
            chat.participants.append(query->value(0).toString());
        }
        chats.append(chat);
    }
    return chats;
}
QList<ClientInfo> WorkDataBase::getFoundUsers(QString desired)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT userID, login, name FROM User where name LIKE '%" + desired +  "%'");
    QList<ClientInfo> users;
    while(query->next()){
        ClientInfo user;
        user.userID = query->value(0).toString();
        user.login = query->value(1).toString();
        user.name = query->value(2).toString();
        users.append(user);
    }
    return users;
}


bool WorkDataBase::deleteChat(QString chatID, QString userID){
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT messageID FROM Messages WHERE chatID = '" + chatID +  "'");
    while (query->next()) {
        qDebug() << "messageID = "  << query->value(0).toString();
        QSqlQuery* query1 = new QSqlQuery(db);
        query1->exec("DELETE FROM MessageStatus WHERE messageID ='" + query->value(0).toString()
                     +  "' and userID = '" + userID + "'");
    }
    if(!query->exec("DELETE FROM Participants WHERE participantID = " + userID + "and chatID = " + chatID)){
        return false;
    }
    return true;
}
bool WorkDataBase::deleteMessage(QString messageID, QString userID)
{
    QSqlQuery* query = new QSqlQuery(db);
    if(!query->exec("DELETE FROM MessageStatus WHERE messageID ='" + messageID +  "' and userID = '" + userID + "'"))
        return false;
    query->exec("SELECT EXIST (SELECT * FROM MessageStatus WHERE messageID ='" + messageID +  "')");
    query->next();
    if(query->value(0).toString() == "0"){
        if(!query->exec("DELETE FROM Messages WHERE messageID = " + messageID))
            return false;
    }

    return true;
}
QString WorkDataBase::deleteMessageForEveryone(QString messageID)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT chatID FROM Messages WHERE messageID = " + messageID);
    query->next();
    QString chatID = query->value(0).toString();
    if(!query->exec("DELETE FROM MessageStatus WHERE messageID = " + messageID))
        return "";
    if(!query->exec("DELETE FROM Messages WHERE messageID = " + messageID))
        return "";
    return chatID;

}

bool WorkDataBase::updateUserIsReadingMessages(QString chatID, QString curUserID)
{
    bool f = true;
    QSqlQuery* query = new QSqlQuery(db);
    query->exec("SELECT messageID FROM Messages WHERE chatID = " + chatID + " ORDER BY messageID DESC");
    while(query->next()){
        QSqlQuery* query1 = new QSqlQuery(db);
        query1->exec("SELECT isRead FROM MessageStatus WHERE messageID = '" + query->value(0).toString() + "' and userID = '" + curUserID + "'");
        query1->next();
        if(query1->value(0).toString() == "1")
            break;
        if(!query1->exec("UPDATE MessageStatus SET `isRead` = '1' WHERE messageID = '" + query->value(0).toString() + "' and userID = '" + curUserID+ "'"))
            f = false;;
    }
    return f;
}
bool WorkDataBase::updateMessageEddit(QString messageID, QString message)
{
    qDebug() << "messageID" << messageID << " message" << message;
    QSqlQuery* query = new QSqlQuery(db);
    if(!query->exec("UPDATE Messages SET `message` = '" + message + "' WHERE messageID = '" + messageID + "'")) {
        return false;
    }
    return true;
}
