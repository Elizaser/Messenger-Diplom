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
    connect(ui->radioButtonPeople, SIGNAL(clicked()), SLOT(searchChatList()));
    connect(ui->radioButton_Chats, SIGNAL(clicked()), SLOT(searchChatList()));
    connect(ui->tableWidget_chatsList, SIGNAL(cellClicked(int, int)), SLOT(clickChatList(int, int )));
    connect(ui->tableWidget_chatWindow, SIGNAL(cellClicked(int, int)), SLOT(clickChatWindow(int, int)));
    connect(ui->action_quit, SIGNAL(triggered(bool)),this,  SLOT(quit()));

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
        } else if(signal == "searchPeople") {
            searchPeople(messageFromServer.getUsers());
        } else if(signal == "searchChats") {
            searchChats(messageFromServer.getChats());
        } else if(signal == "setChatContent") {
            setChatContent(messageFromServer.getChatContent());
        } else if(signal == "newMessage") {
            setNewMessage(messageFromServer.getMessage());
        } else if(signal == "deleteMessage") {
            deleteMessage(messageFromServer.getMessage());
        } else if(signal == "updateMessage") {
            updateMessage(messageFromServer.getMessageID(), messageFromServer.getMessageText());
        } else if(signal == "newChat") {
            setNewChat(messageFromServer.getChat());
        } else if(signal == "deleteParticipant") {
//            deleteParticipant(messageFromServer.getUser());
        } else if(signal == "setUsersCreateChat") {
            createChat->setAllUsers(messageFromServer.getUsers());
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
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Найденные люди"){
        if(j == 0){
            ui->tableWidget_chatWindow->clear();
            chats[i].countIsNotReadMessages = "0";
            curChat = chats[i];
            ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(curChat.name));
            ui->label_chatName->setText(chats[i].name);
            socket->write("{\"process\":\"main\", \"signal\":\"getChatContent\", \"chatID\":\"" + chats[i].chatID.toLocal8Bit() + "\"}");
        }
        if(j == 1){
            clickedDeleteChat(i, "deleteChat");
        }
        else if (j == 2){
            clickedDeleteChat(i, "exitChat");
            chats.removeAt(i);
        }
    } else {
        QMessageBox::information(this, "Информация(MainWindow)", "Пока это поле недоступно");
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
    sockWrite("main", signal, "\"chatID\":\"" + chats.at(i).chatID.toLocal8Bit() + "\"");
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
            socket->write("{\"process\":\"main\", \"signal\":\"messageDeleteForEveryone\", \"messageID\":\"" + curChatContent.at(i).messageID.toLocal8Bit() + "\"}");
        } else {
            socket->write("{\"process\":\"main\", \"signal\":\"messageDelete\", \"messageID\":\"" + curChatContent.at(i).messageID.toLocal8Bit() + "\"}");
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
    chats.append(chat);
    int rowCount  =  ui->tableWidget_chatsList->rowCount();
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты") {
        qDebug() << "chat.name - " << chat.name;
        ui->tableWidget_chatsList->insertRow(rowCount);
        ui->tableWidget_chatsList->setItem(rowCount, 0, new QTableWidgetItem(chat.name + " " + chat.countIsNotReadMessages));
        ui->tableWidget_chatsList->setItem(rowCount, 1, new QTableWidgetItem("Очистить"));
        QString out = "";
        if(chat.type == "group")
            out = "выйти";
        ui->tableWidget_chatsList->setItem(rowCount, 2, new QTableWidgetItem(out));
    }
}
void MainWindow::deleteChat(QString chatID)
{
    if(int i = searchChatByID(chatID) != -1){
        if( !(ui->tableWidget_chatsList->horizontalHeaderItem(i)->text() != "Найденные люди")){
            ui->tableWidget_chatsList->removeRow(i);
            chats.removeAt(i);
        }
    }
}
void MainWindow::setNewMessage(UserMessage message)
{
    curChatContent.append(message);
    if(curChatContent.at(0).chatID == message.chatID) {
        qDebug() << "new Message curChatContent = " << message.message;
        int rowCount  =  ui->tableWidget_chatsList->rowCount();
        ui->tableWidget_chatWindow->setItem(rowCount, 0, new QTableWidgetItem(message.senderName));
        ui->tableWidget_chatWindow->setItem(rowCount, 1, new QTableWidgetItem("удалить"));

        QString red = "";
        if(message.senderID == userInfo.userID)
            red == "редактировать";
        ui->tableWidget_chatWindow->setItem(rowCount, 2, new QTableWidgetItem(red));
    }

//    socket->write("{\"process\":\"main\", \"signal\":\"isReadingMessage\", \"chatID\":\""
//                    + message.chatID.toLocal8Bit() + "\"}");
}
void MainWindow::deleteMessage(UserMessage message)
{
    if(int i = searchMessageByID(message.messageID) != -1) {
        if(!curChatContent.isEmpty() && curChatContent.at(i).chatID == message.messageID) {
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
    showListChats("Мои чаты");

}
void MainWindow::searchPeople(QList<UserInfo> users)
{
    this->users = users;
    showFoundUsers("Найденные люди");
}
void MainWindow::searchChats(QList<UserChat> chats)
{
    this->chats = chats;
    showListChats("Найденные чаты");
}
void MainWindow::setChatContent(QList<UserMessage> conntent)
{
    curChatContent = conntent;
    showChatContents(curChatContent);
    socket->write("{\"process\":\"main\", \"signal\":\"isReadingMessage\", \"chatID\":\"" + curChat.chatID.toLocal8Bit() + "\"}");
}
void MainWindow::searchChatList()
{
    //очистка текущего чата и его контента
    ui->tableWidget_chatWindow->clear();
    ui->label_chatName->clear();
    curChat.chatID = "";
    curChat.countIsNotReadMessages = "";
    curChat.name = "";
    //currentChat.participants = "";
    curChat.userCreator = "";

    QString desired = ui->lineEdit_searchUser->text();
    if(desired == ""){
        if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Мои чаты")
            socket->write("{\"process\":\"main\", \"signal\":\"getUserChats\"}");
        return;
    }
    if(ui->radioButtonPeople->isChecked()) {
        socket->write("{\"process\":\"main\", \"signal\":\"searchPeople\", \"user\":\"" + desired.toLocal8Bit() + "\"}");
    } else if(ui->radioButton_Chats->isChecked()){
        socket->write("{\"process\":\"main\", \"signal\":\"searchChats\", \"user\":\"" + desired.toLocal8Bit() + "\"}");
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


    for(int i = 0; i < chats.count(); i++){
        ui->tableWidget_chatsList->insertRow(i);
        QString nameChat = chats.at(i).name;
        if(chats[i].countIsNotReadMessages.toInt() > 0) {
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
void MainWindow::showFoundUsers(QString headerLabel)
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
    ui->tableWidget_chatWindow->setColumnCount(4);
    QStringList horzHeaders;
    horzHeaders << "История вашего чата" << "" << "" << "";
    ui->tableWidget_chatWindow->setHorizontalHeaderLabels(horzHeaders);
    for(int i = 0; i < conntents.count(); i++){
        ui->tableWidget_chatWindow->insertRow(i);
        ui->tableWidget_chatWindow->setItem(i, 0, new QTableWidgetItem(conntents.at(i).senderName));
        ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(conntents.at(i).message));
        QString text = "";
        if(!conntents.at(i).isSystem.toInt())
            text = "удалить";
        ui->tableWidget_chatWindow->setItem(i, 2, new QTableWidgetItem(text));
        text = "";
        if(conntents.at(i).senderID == userInfo.userID &&
                !conntents.at(i).isSystem.toInt()){
            text+="ред.";
        }
        ui->tableWidget_chatWindow->setItem(i, 3, new QTableWidgetItem(text));
    }
    ui->tableWidget_chatWindow->resizeColumnsToContents();
    ui->tableWidget_chatWindow->resizeRowsToContents();
}
void MainWindow::showNewMessage(QList<UserMessage> messages)
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
            socket->write("{\"process\":\"main\", \"signal\":\"sendMessage\", \"message\":\"" +
                           message.message.toLocal8Bit() +
                           "\", \"chatID\":\"" + message.chatID.toLocal8Bit() +
                           "\", \"senderName\":\"" + message.senderName.toLocal8Bit() +
                           "\", \"senderID\":\"" + message.senderID.toLocal8Bit() +  "\"}");
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
}
void MainWindow::sockWrite(QString process, QString signal, QString message)
{
    QByteArray data = "{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", " + message.toLocal8Bit() + "}";
    socket->write(data);
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
                        + "\",  \"type\":\"" + chat.type
                        + "\", \"participants\":[");
            for(auto & participant : chat.participants){
                data.append("{\"participant\":\"" + participant + "\"},");
            }
            data.remove(data.length()-1,1);
            data.append("]},");
        }
        data.remove(data.length()-1,1);
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
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
                        + "\",  \"type\":\"" + chat.type.toLocal8Bit()
                        + "\", \"participants\":[";
        for(auto & participant : chat.participants){
            data.append("{\"participant\":\"" + participant + "\"},");
        }
        data.remove(data.length()-1,1);
        data.append("]}");
        socket->write(data);
}
void MainWindow::sockWrite(QString process, QString signal, QList<UserMessage> conntents)
{
    if(conntents.size() > 0) {
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", \"conntents\":[";

        for(auto & conntent : conntents) {
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
    } else {
        sockWrite(process, signal,
                  "\"conntent\":\"-1\"");
    }
}
void MainWindow::sockWrite(QString process, QString signal, UserMessage conntent)
{
        QByteArray data = "{\"process\":\"" + process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit()
                        + "\", \"messageID\":\"" + conntent.messageID.toLocal8Bit()
                        + "\", \"chatID\":\"" + conntent.chatID.toLocal8Bit()
                        + "\", \"senderID\":\"" + conntent.senderID.toLocal8Bit()
                        + "\", \"senderName\":\"" + conntent.senderName.toLocal8Bit()
                        + "\", \"isRead\":\"" + conntent.isRead.toLocal8Bit()
                        + "\", \"isSystem\":\"" + conntent.isSystem.toLocal8Bit()
                        + "\", \"message\":\"" + conntent.message.toLocal8Bit() +"\"}";
        socket->write(data);
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
}
