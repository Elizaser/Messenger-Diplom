#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QTextEdit>

MainWindow::MainWindow(QTcpSocket* socket, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->socket = socket;
    connect(ui->lineEdit_searchUser, &QLineEdit::textChanged, this, &MainWindow::searchChatList);
//    connect(ui->radioButtonPeople, SIGNAL(clicked()), SLOT(searchChatList()));
//    connect(ui->radioButton_Chats, SIGNAL(clicked()), SLOT(searchChatList()));
    connect(ui->tableWidget_chatsList, SIGNAL(cellClicked(int, int)), SLOT(clickChatList(int, int )));
    connect(ui->tableWidget_chatWindow, SIGNAL(cellClicked(int, int)), SLOT(clickChatWindow(int, int)));
    connect(ui->action_quit, SIGNAL(triggered(bool)),this,  SLOT(quit()));

    ui->tableWidget_chatsList->setShowGrid(false);
    ui->tableWidget_chatWindow->setShowGrid(false);
    ui->tableWidget_chatsList->setEditTriggers(0);
    ui->tableWidget_chatWindow->setEditTriggers(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockReady(DataParsing messageFromServer)
{
    try {
        QString  signal = messageFromServer.getSignal();
        qDebug() << " signal = " << signal;
        if(signal == "setUserInfo"){
            setUserInfo(messageFromServer.getUser());
        } else if(signal == "setUserChats") {
            setUserChats(messageFromServer.getChats());
        } else if(signal == "setAllUsers") {
            setAllUsers(messageFromServer.getUsers());
        } else if(signal == "searchPeople") {
            setSearchPeople(messageFromServer.getUsers());
        } else if(signal == "searchChats") {
            setSearchChats(messageFromServer.getChats());
        } else if(signal == "setChatContent") {
            qDebug() << "signal setContent messageFromServer.getChatID() =  " << messageFromServer.getChatID();
            if(messageFromServer.getChatID() != "") {
                int i = searchChatByID(messageFromServer.getChat().chatID);
                if(i == -1) {
                    setNewChat(messageFromServer.getChat());
                    i = searchChatByID(messageFromServer.getChat().chatID);
                }
                qDebug() << "signal setContent i =  " << i;
                chats[i].countIsNotReadMessages = "0";
                curChat = chats[i];
                qDebug() << "signal setContent curChat =  " << curChat.name;
            }
            setChatContent(messageFromServer.getChatContent());
        } else if(signal == "newMessage") {
            setNewMessage(messageFromServer.getMessage());
        } else if(signal == "deleteMessage") {
            deleteMessage(messageFromServer.getMessage());
        } else if(signal == "updateMessage") {
            updateMessage(messageFromServer.getMessageID(), messageFromServer.getMessageText());
        } else if(signal == "newChat") {
            setNewChat(messageFromServer.getChat());
        } else if(signal == "newDialog") {
//            curChat = messageFromServer.getChat();
//            qDebug () << "signal newDialog curChat =  " << curChat.name;
            setNewChat(messageFromServer.getChat());
        } else if(signal == "deleteParticipant") {
//            deleteParticipant(messageFromServer.getUser());
        } else if(signal == "setUsersCreateChat") {
            setUsersInWindowCreateChat(messageFromServer.getUsers());
        } else if(signal == "updateIsReadingMessages") {
            updateIsReadingMessages(messageFromServer.getChatID());
        } else if(signal == "setCurChat") {
//            int i = searchChatByID( messageFromServer.getChatID());
//            curChat = chats[i];
        } else {
            qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}



void MainWindow::quit()
{
    userInfo.login = "";
    userInfo.name = "";
    userInfo.password = "";

    this->close();
    emit openWindow();
}



void MainWindow:: clickChatList(int i, int  j)
{
    if(ui->tableWidget_chatsList->item(i, j)->text() == "") return;
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты" ||
            ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Найденные чаты"){
        if(j == 0){
            clearChatWindow();
//            ui->tableWidget_chatWindow->clear();
//            chats[i].countIsNotReadMessages = "0";
            qDebug() << "clickChatList chats[i].countIsNotReadMessages = " << chats[i].countIsNotReadMessages;
            curChat = chats[i];
            ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(curChat.name));
            ui->label_chatName->setText(chats[i].name);
            sockWrite("main", "getChatContent", "\"chatID\":\"" + curChat.chatID.toLocal8Bit() + "\"");
        }
        if(j == 1){
            clickedDeleteChat(i, "deleteChat");
        }
        else if (j == 2){
            clickedDeleteChat(i, "exitChat");
//            chats.removeAt(i);
        }
    } else {
        curUser = users[i];
        clearChatWindow();
        if(j == 0) {

        } else if(j == 1) {
            sockWrite("main", "getDialogContent", "\"userID\":\"" + curUser.userID.toLocal8Bit() + "\"");
        }
//        QMessageBox::information(this, "Информация(MainWindow)", "Пока это поле недоступно");
    }
}
void MainWindow::clickedDeleteChat(int i, QString signal)
{
    if(!curChatContent.isEmpty()) {
        if(curChatContent.at(0).chatID == chats.at(i).chatID)
            ui->tableWidget_chatWindow->clear();
            ui->tableWidget_chatWindow->setRowCount(0);
            curChatContent.clear();
    }
    if(ui->tableWidget_chatsList->horizontalHeaderItem(i)->text() != "Найденные люди")
        ui->tableWidget_chatsList->removeRow(i);
    qDebug() << "chats.at(i).chatID" << chats.at(i).chatID;
    sockWrite("main", signal, "\"chatID\":\"" + chats.at(i).chatID.toLocal8Bit() + "\"");
    chats.removeAt(i);
    clearChatWindow();
}

void MainWindow:: clickChatWindow(int i, int j)
{
    if(ui->tableWidget_chatWindow->item(i, j)->text() == "") return;
    curMessage = curChatContent.at(i);
    if(j == 2){
        clickedDeleteMessage(i);
    } else if(j == 3){
        clickedEdditMessage(i);
    }
}
void MainWindow::clickedDeleteMessage(int i)
{
    QMessageBox msgBox;
    msgBox.setText("Внимание");
    msgBox.setInformativeText("Вы действительно хотите удалить сообщение?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    QCheckBox* cb = new QCheckBox("&Удалить для всех");
    cb->setChecked(false);
    if(curChatContent.at(i).senderID == userInfo.userID){
        msgBox.setCheckBox(cb);
    }
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.show();
    int res = msgBox.exec();
    if (res == QMessageBox::Ok)  {
        if(cb->isChecked()) {
            sockWrite("main", "messageDeleteForEveryone", curChatContent.at(i));
        } else {
            sockWrite("main", "messageDelete", curChatContent.at(i));
        }
    }
}
void MainWindow::clickedEdditMessage(int i)
{
    ui->textEdit_replyBox->setText(ui->tableWidget_chatWindow->item(i, 1)->text());
    ui->pushButton_sendReply->setText("Редактировать");
    ui->pushButton_sendReply->adjustSize();
}

void MainWindow::setNewChat(UserChat chat)
{
    qDebug() << "setNewChat chat.countIsNotReadMessages = " << chat.countIsNotReadMessages;
    qDebug() << "setNewChat chat.countIsLook = " << chat.countIsLook;

    chats.append(chat);
    renameDialogOnNameCompanion();
    showNewChat(chat);

}
void MainWindow::deleteChat(QString chatID)
{
    int i = searchChatByID(chatID);
    if(i != -1){
        if( !(ui->tableWidget_chatsList->horizontalHeaderItem(i)->text() != "Найденные люди")){
            ui->tableWidget_chatsList->removeRow(i);
            chats.removeAt(i);
        }
    }
}
void MainWindow::setNewMessage(UserMessage message)
{
//    if(curChat.chatID == ""){
//        int i = searchChatByID(message.chatID);
//        chats[i].countIsLook = "1";
//        chats[i].countIsNotReadMessages = "1";
//        curChat = chats[i];
//        showNewChat(curChat);
    /*} else*/
    qDebug() << "setNewMessage curChat.chatID = " << curChat.chatID;
    qDebug() << "setNewMessage message.chatID = " << message.chatID;
    if((curChat.chatID == "" || curChat.chatID != message.chatID) &&
    (  ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Найденные чаты"
    || ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты" )) {
        int i = searchChatByID(message.chatID);
        chats[i].countIsNotReadMessages = QString::number(chats.at(i).countIsNotReadMessages.toInt() + 1);
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(chats[i].name + " " + chats[i].countIsNotReadMessages));
    } else {
        curChatContent.append(message);
        showNewMessage(message);
        qDebug() << "message.senderID = " << message.senderID;
        qDebug() << "userInfo.userID = " << userInfo.userID;
        if(message.senderID != userInfo.userID)
            sockWrite("main", "isReadingMessage", "\"chatID\":\""
                      + message.chatID + "\"");
    }

}
void MainWindow::deleteMessage(UserMessage message)
{
    int i = searchMessageByID(message.messageID);

    if(i != -1) {
        qDebug() << "i = " << i << " message.messageID = " << message.messageID;
        qDebug() << "1curChatContent.at(i).chatID = " << curChatContent.at(i).chatID << " message.chatID = " << message.chatID;
        if(!(curChatContent.isEmpty()) && curChatContent.at(i).chatID == message.chatID) {
            qDebug() << "2curChatContent.at(i).chatID = " << curChatContent.at(i).chatID << " message.chatID = " << message.chatID;

            ui->tableWidget_chatWindow->removeRow(i);
            curChatContent.removeAt(i);
        }
    }
}
void MainWindow::updateMessage(QString messageID, QString message)
{
    for(int i = curChatContent.count() - 1; i >= 0; i--){
            if(curChatContent.at(i).messageID == messageID){
                if(curChat.chatID == curChatContent.at(i).chatID){
                    ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(message));
                    curChatContent[i].message = message;
                }
                return;
            }
        }
}
int MainWindow::searchChatByID(QString chatID)
{
    for(int i = chats.count() - 1; i >= 0; i--){
        if(chats.at(i).chatID == chatID){
            return i;
        }
    }
    return (-1);
}
int MainWindow::searchMessageByID(QString messageID)
{
    for(int i = curChatContent.count() - 1; i >= 0; i--){
        if(curChatContent.at(i).messageID == messageID){
            return i;
        }
    }
    return (-1);
}
int MainWindow::searchUserByID(QString userID, QList<UserInfo> usersForCreateChat)
{
    for(int i = usersForCreateChat.count() - 1; i >= 0; i--){
        if(usersForCreateChat.at(i).userID == userID){
            return i;
        }
    }
    return (-1);
}

void MainWindow::setUserInfo(UserInfo userInfo)
{
    this->userInfo = userInfo;
    sockWrite("main", "setUserInfo", "\"login\":\"" +
              userInfo.login + "\", \"password\":\"" +
              userInfo.password + "\"");
}
void MainWindow::setUserChats(QList<UserChat> chats)
{

    this->chats = chats;
    renameDialogOnNameCompanion();
    showListChats("Мои чаты");

}
void MainWindow::renameDialogOnNameCompanion(){
    for(auto&chat:chats){
        if(chat.type == "dialog"){
            foreach (auto key, chat.participants.keys()){
                if(key != userInfo.userID)
                    chat.name = chat.participants.value(key);
            }
        }
    }
}
void MainWindow::setAllUsers(QList<UserInfo> users)
{
    this->users = users;
    showUsers("Все пользователи");

}
void MainWindow::setSearchPeople(QList<UserInfo> users)
{
    this->users = users;
    showUsers("Найденные люди");
}
void MainWindow::setSearchChats(QList<UserChat> chats)
{
    this->chats = chats;
    renameDialogOnNameCompanion();
    showListChats("Найденные чаты");
}
void MainWindow::setChatContent(QList<UserMessage> conntent)
{
    curChatContent = conntent;
    showChatContents(curChatContent);
    int i = searchChatByID(curChat.chatID);
    if(i == -1) return;
    if(chats[i].countIsNotReadMessages == "0" || chats[i].countIsNotReadMessages == "") return;
    sockWrite("main", "isReadingMessage", chats[i]);
    chats[i].countIsNotReadMessages = "0";
}
void MainWindow::searchChatList()
{
    clearChatWindow();
    QString desired = ui->lineEdit_searchUser->text();
    if(ui->radioButtonPeople->isChecked()) {
        if(desired == "") {
            if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Все пользователи")
                socket->write("{\"process\":\"main\", \"signal\":\"getAllUsers\"}");
            return;
        }
        socket->write("{\"process\":\"main\", \"signal\":\"searchPeople\", \"user\":\"" + desired.toLocal8Bit() + "\"}");
    } else if(ui->radioButton_Chats->isChecked()){
        if(desired == "") {
            if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Мои чаты")
                socket->write("{\"process\":\"main\", \"signal\":\"getUserChats\"}");
            return;
        }
        socket->write("{\"process\":\"main\", \"signal\":\"searchChats\", \"user\":\"" + desired.toLocal8Bit() + "\"}");
    }
}
void MainWindow::clearChatWindow()
{
    //    очистка текущего чата и его контента
        ui->tableWidget_chatWindow->clear();
        ui->tableWidget_chatWindow->setRowCount(0);
        ui->tableWidget_chatWindow->setColumnCount(0);
        ui->label_chatName->clear();
        curChat.chatID = "";
        curChat.countIsNotReadMessages = "";
        curChat.name = "";
        //currentChat.participants = "";
        curChat.userCreator = "";
}

void MainWindow::setUsersInWindowCreateChat(QList<UserInfo> users)
{
    int i = searchUserByID(userInfo.userID, users);
    users.removeAt(i);
    createChat->setAllUsers(users);
}
void MainWindow::updateIsReadingMessages(QString chatID)
{
    for(int i = curChatContent.size() - 1; i >= 0; i--){
        if(curChatContent[i].chatID != chatID) break;
        if(curChatContent[i].isRead == "1") break;
        curChatContent[i].isRead = "1";
        ui->tableWidget_chatWindow->setItem(i, 4, new QTableWidgetItem("VV"));
    }
}

void MainWindow::showListChats(QString headerLabel)
{
    ui->tableWidget_chatsList-> clear();
    ui->tableWidget_chatsList->setRowCount(0);
    ui->tableWidget_chatsList->setColumnCount(3);

    QStringList horzHeaders;
    horzHeaders << headerLabel << "" << "";
    ui->tableWidget_chatsList->setHorizontalHeaderLabels(horzHeaders);


    for(int i = 0; i < chats.count(); i++) {
        if(chats[i].countIsLook == "0") break;
        ui->tableWidget_chatsList->insertRow(i);
        QString nameChat = chats.at(i).name;

        if(chats[i].countIsNotReadMessages != "0") {
            nameChat += " " + chats.at(i).countIsNotReadMessages;
        }
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(nameChat));
        ui->tableWidget_chatsList->setItem(i, 1, new QTableWidgetItem("очистить"));
        QString out = "";
        if(chats[i].type == "group")
            out = "выйти";
        ui->tableWidget_chatsList->setItem(i, 2, new QTableWidgetItem(out));

    }
}
void MainWindow::showUsers(QString headerLabel)
{
    ui->tableWidget_chatsList-> clear();
    ui->tableWidget_chatsList->setRowCount(0);
    ui->tableWidget_chatsList->setColumnCount(2);

    QStringList horzHeaders;
    horzHeaders << headerLabel << "";
    ui->tableWidget_chatsList->setHorizontalHeaderLabels(horzHeaders);

    for(int i = 0; i < users.count(); i++){
        ui->tableWidget_chatsList->insertRow(i);
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(users.at(i).name));
        ui->tableWidget_chatsList->setItem(i, 1, new QTableWidgetItem("написать"));
    }
}
void MainWindow::showChatContents(QList<UserMessage> conntents)
{
    ui->tableWidget_chatWindow-> clear();
    ui->tableWidget_chatWindow->setRowCount(0);
    ui->tableWidget_chatWindow->setColumnCount(5);
    QStringList horzHeaders;
    horzHeaders << "История вашего чата" << "" << "" << "" << "";
    ui->tableWidget_chatWindow->setHorizontalHeaderLabels(horzHeaders);
    for(int i = 0; i < conntents.count(); i++){
        ui->tableWidget_chatWindow->insertRow(i);
        ui->tableWidget_chatWindow->setItem(i, 0, new QTableWidgetItem(conntents.at(i).senderName));
        ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(conntents.at(i).message));
        QString text = "";
        if(conntents.at(i).isSystem != "1")
            text = "удалить";
        ui->tableWidget_chatWindow->setItem(i, 2, new QTableWidgetItem(text));
        text = "";
        if(conntents.at(i).senderID == userInfo.userID &&
                conntents.at(i).isSystem != "1"){
            text ="ред.";
        }
        ui->tableWidget_chatWindow->setItem(i, 3, new QTableWidgetItem(text));
        if(conntents.at(i).isRead == "1")
            text = "VV";
        else
            text = "V";
        ui->tableWidget_chatWindow->setItem(i, 4, new QTableWidgetItem(text));
    }
    ui->tableWidget_chatWindow->resizeColumnsToContents();
    ui->tableWidget_chatWindow->resizeRowsToContents();
}
void MainWindow::showNewMessage(UserMessage message)
{
    int rowCount  =  ui->tableWidget_chatWindow->rowCount();
    ui->tableWidget_chatWindow->insertRow(rowCount);
    ui->tableWidget_chatWindow->setItem(rowCount, 0, new QTableWidgetItem(message.senderName));
    ui->tableWidget_chatWindow->setItem(rowCount, 1, new QTableWidgetItem(message.message));
    ui->tableWidget_chatWindow->setItem(rowCount, 2, new QTableWidgetItem("удалить"));

    QString red = "";
    if(message.senderID == userInfo.userID)
        red = "редактировать";
    ui->tableWidget_chatWindow->setItem(rowCount, 3, new QTableWidgetItem(red));
    QString text = "";
    if(message.isRead == "1")
        text = "VV";
    else
        text = "V";
    ui->tableWidget_chatWindow->setItem(rowCount, 4, new QTableWidgetItem(text));
}
void MainWindow::showNewChat(UserChat chat)
{
    qDebug() << "showNewChat chat.countIsNotReadMessages = " << chat.countIsNotReadMessages;
    int rowCount  =  ui->tableWidget_chatsList->rowCount();
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты") {
        ui->tableWidget_chatsList->insertRow(rowCount);
        QString countIsNotReadMessages = "";
        if(chat.countIsNotReadMessages != "0")
            countIsNotReadMessages = chat.countIsNotReadMessages;
        ui->tableWidget_chatsList->setItem(rowCount, 0, new QTableWidgetItem(chat.name + " " + countIsNotReadMessages));
        ui->tableWidget_chatsList->setItem(rowCount, 1, new QTableWidgetItem("Очистить"));
        QString out = "";
        if(chat.type == "group")
            out = "выйти";
        ui->tableWidget_chatsList->setItem(rowCount, 2, new QTableWidgetItem(out));
    }
}
void MainWindow::deleteParticipant(UserInfo userInfo)
{
//    int rowCount = ui->tableWidget_chatWindow->rowCount();
//    for(auto&message:messages){
//        curChatContent.append(message);
//        ui->tableWidget_chatWindow->insertRow(rowCount);
//        ui->tableWidget_chatWindow->setItem(rowCount, 0, new QTableWidgetItem(message.senderName));
//        ui->tableWidget_chatWindow->setItem(rowCount, 1, new QTableWidgetItem(message.message));
//        ui->tableWidget_chatWindow->setItem(rowCount, 2, new QTableWidgetItem("удалить"));
//        QString text = "";
//        if(message.senderID == userInfo.userID){
//            text+="ред.";
//        }
//        ui->tableWidget_chatWindow->setItem(rowCount, 3, new QTableWidgetItem(text));
//    }
}



void MainWindow::on_pushButton_createChat_clicked()
{
    createChat = new CreateChat(socket);
//    connect(createChat, &CreateChat::sockWrite, this, &MainWindow::sockWrite);
    connect(createChat, SIGNAL(sockWrite(QString,QString, UserChat)), this, SLOT(sockWrite(QString, QString, UserChat)));

    createChat->show();
    createChat->start();

}
void MainWindow::on_pushButton_sendReply_clicked()
{
    if(ui->pushButton_sendReply->text() == "Отправить"){
        UserMessage message;
        message.chatID = curChat.chatID;
        message.senderName = userInfo.name;
        message.senderID = userInfo.userID;
        QString m = message.message = ui->textEdit_replyBox->toPlainText();
        if(m.remove(' ').remove('\n') == ""){
            return;
        }
        if(socket->isOpen()) {
            if(curChat.chatID == "") {
                message.senderID = curUser.userID;
                message.senderName = curUser.name;
                sockWrite("main", "sendMessageInNewDialog", message);
            } else {
                sockWrite("main", "sendMessage", message);
            }
            ui->textEdit_replyBox->clear();
        } else {
            QMessageBox::information(this, "Информация(MainWindow)", "Соединение не установлено");
        }
    } else if(ui->pushButton_sendReply->text() == "Редактировать"){
        qDebug() << "messageID" << curMessage.messageID << " message" << ui->textEdit_replyBox->toPlainText().toLocal8Bit();
        curMessage.message =    ui->textEdit_replyBox->toPlainText();
        sockWrite("main", "messageEddit", curMessage);
        ui->pushButton_sendReply->setText("Отправить");
        ui->textEdit_replyBox->clear();
    }
}

void MainWindow::sockWrite(QString process, QString signal)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\"}";
    socket->write(data);
    socket->waitForBytesWritten();
}
void MainWindow::sockWrite(QString process, QString signal, QString message)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", " + message.toLocal8Bit() + "}";
    socket->write(data);
    socket->waitForBytesWritten();
}
void MainWindow::sockWrite(QString process, QString signal, QList<UserChat> chats)
{
    if(chats.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"chats\":[";
        for(auto & chat : chats) {
            data.append("{\"chatID\":\"" + chat.chatID +
                        "\", \"userCreator\":\"" + chat.userCreator
                        + "\", \"name\":\"" + chat.name
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages
                        + "\",  \"countIsLook\":\"" + chat.countIsLook
                        + "\",  \"type\":\"" + chat.type
                        + "\", \"participants\":[");
            foreach (auto key, chat.participants.keys()){
                data.append("{\"participantID\":\"" + key + "\", \"participantName\":\"" + chat.participants.value(key) + "\"},");
            }
//            for(auto & participant : chat.participants){
//                data.append("{\"participant\":\"" + participant + "\"},");
//            }
            data.remove(data.length()-1,1);
            data.append("]},");
        }
        data.remove(data.length()-1,1);
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
        socket->waitForBytesWritten();
    } else {
        sockWrite(process, signal,
                  "\"chat\":\"-1\"");
    }
}
void MainWindow::sockWrite(QString process, QString signal, UserChat chat)
{
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                        + "\", \"chatID\":\"" + chat.chatID.toLocal8Bit()
                        + "\", \"userCreator\":\"" + chat.userCreator.toLocal8Bit()
                        + "\", \"name\":\"" + chat.name.toLocal8Bit()
                        + "\",  \"countIsNotReadMessages\":\"" + chat.countIsNotReadMessages.toLocal8Bit()
                        + "\",  \"countIsLook\":\"" + chat.countIsLook.toLocal8Bit()
                        + "\",  \"type\":\"" + chat.type.toLocal8Bit()
                        + "\", \"participants\":[";
        foreach (auto key, chat.participants.keys()){
            data.append("{\"participantID\":\"" + key + "\", \"participantName\":\"" + chat.participants.value(key) + "\"},");
        }
//        for(auto & participant : chat.participants){
//            data.append("{\"participant\":\"" + participant + "\"},");
//        }
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
        socket->waitForBytesWritten();
}
void MainWindow::sockWrite(QString process, QString signal, QList<UserMessage> conntents)
{
    if(conntents.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"conntents\":[";

        for(auto & conntent : conntents) {
            conntent.isSystem = "0";
            data.append("{\"messageID\":\"" + conntent.messageID +
                        "\", \"chatID\":\"" + conntent.chatID +
                        "\", \"senderID\":\"" + conntent.senderID +
                        "\", \"senderName\":\"" + conntent.senderName +
                        "\", \"isRead\":\"" + conntent.isRead +
                        "\", \"isSystem\":\"" + conntent.isSystem +
                        "\", \"message\":\"" + conntent.message +"\"},");

        }
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
        socket->waitForBytesWritten();
    } else {
        sockWrite(process, signal,
                  "\"conntent\":\"-1\"");
    }
}
void MainWindow::sockWrite(QString process, QString signal, UserMessage conntent)
{
    conntent.isSystem = "0";
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                        + "\", \"messageID\":\"" + conntent.messageID.toLocal8Bit()
                        + "\", \"chatID\":\"" + conntent.chatID.toLocal8Bit()
                        + "\", \"senderID\":\"" + conntent.senderID.toLocal8Bit()
                        + "\", \"senderName\":\"" + conntent.senderName.toLocal8Bit()
                        + "\", \"isRead\":\"" + conntent.isRead.toLocal8Bit()
                        + "\", \"isSystem\":\"" + conntent.isSystem.toLocal8Bit()
                        + "\", \"message\":\"" + conntent.message.toLocal8Bit() +"\"}";
        socket->write(data);
        socket->waitForBytesWritten();
}
void MainWindow::sockWrite(QString process, QString signal, QList<UserInfo> clientInfos)
{
    if(clientInfos.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"foundUsers\":[";

        for(auto & clientInfo : clientInfos) {
            data.append("{\"login\":\"" + clientInfo.login +
                        "\", \"name\":\"" + clientInfo.name +
                        "\", \"userID\":\"" + clientInfo.userID +"\"},");
        }
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
        socket->waitForBytesWritten();
    } else {
        sockWrite(process, signal, "\"foundUser\":\"-1\"");
    }
}
void MainWindow::sockWrite(QString process, QString signal, UserInfo clientInfo)
{
    QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() +
                    + "\", \"login\":\"" + clientInfo.login.toLocal8Bit()
                    + "\", \"name\":\"" + clientInfo.name.toLocal8Bit()
                    + "\", \"userID\":\"" + clientInfo.userID.toLocal8Bit() + "\"},";
    socket->write(data);
    socket->waitForBytesWritten();
}

void MainWindow::on_radioButtonPeople_clicked()
{
    clearChatWindow();
    QString desired = ui->lineEdit_searchUser->text();
    if(desired == "") {
        if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Все пользователи")
             sockWrite("main", "getAllUsers");
        return;
    }
    sockWrite("main", "searchPeople", "\"user\":\"" + desired.toLocal8Bit() + "\"");

}
void MainWindow::on_radioButton_Chats_clicked()
{
    clearChatWindow();
    QString desired = ui->lineEdit_searchUser->text();
    if(desired == "") {
        if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Мои чаты")
            sockWrite("main", "getUserChats");
        return;
    }
    sockWrite("main", "searchChats", "\"user\":\"" + desired.toLocal8Bit() + "\"");
}
