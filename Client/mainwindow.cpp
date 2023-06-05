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
    connect(ui->lineEdit_searchUser, &QLineEdit::textChanged, this, &MainWindow::search);
    connect(ui->radioButtonPeople, SIGNAL(clicked()), SLOT(search()));
    connect(ui->radioButton_Chats, SIGNAL(clicked()), SLOT(search()));
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
            userInfo = messageFromServer.getUserInfo();
            sockWrite("main", "setUserInfo", "\"login\":\"" +
                      userInfo.login + "\", \"password\":\"" +
                      userInfo.password + "\"");
        } else if(signal == "setUserChats") {
            chats = messageFromServer.getUserChats();
            showListChats("Мои чаты");

        } else if(signal == "searchPeople") {
            users = messageFromServer.getFoundUsers();
            showFoundUsers("Найденные люди");
        } else if(signal == "searchChats") {
            chats = messageFromServer.getUserChats();
            showListChats("Найденные чаты");
        } else if(signal == "setChatContent") {
            curChatContent = messageFromServer.getChatContent();
            showChatContents(curChatContent);
            socket->write("{\"process\":\"main\", \"signal\":\"isReadingMessage\", \"chatID\":\"" + currentChat.chatID.toLocal8Bit() + "\"}");
        } else if(signal == "newMessage") {
            for(int i = 0; i < chats.count(); i++){
                if(chats.at(i).chatID == currentChat.chatID){
//                    chats.at(i).countIsNotReadMessages = QString::number(chats.at(i).countIsNotReadMessages.toInt()+1);
                    ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(chats.at(i).name + " " + chats.at(i).countIsNotReadMessages.toInt() + 1));
                    break;
                }
            }

            if(currentChat.chatID == messageFromServer.getChatContent().at(0).chatID){
                showNewMessage(messageFromServer.getChatContent());
                socket->write("{\"process\":\"main\", \"signal\":\"isReadingMessage\", \"chatID\":\""
                              + currentChat.chatID.toLocal8Bit() + "\"}");
            }
        } else if(signal == "deleteMessage") {
            deleteMessage(messageFromServer.getMessageID());
        } else if(signal == "updateMessage") {
            updateMessage(messageFromServer.getMessageID(), messageFromServer.getMessageText());
        } else if(signal == "deleteParticipant") {
            deleteParticipant(messageFromServer.getUserInfo());
        } else {
            qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}

void MainWindow::sockWrite(QString process, QString signal, QString message)
{
    socket->write("{\"process\":\"" +  process.toLocal8Bit() + "\", \"signal\":\"" + signal.toLocal8Bit() + "\", " + message.toLocal8Bit() + "}");
    socket->waitForBytesWritten(500);
}

void MainWindow::search()
{
    //очистка текущего чата и его контента
    ui->tableWidget_chatWindow->clear();
    ui->label_chatName->clear();
    ui->label_chatPhoto->clear();
    currentChat.chatID = "";
    currentChat.countIsNotReadMessages = "";
    currentChat.name = "";
    //currentChat.participants = "";
    currentChat.userCreator = "";

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

void MainWindow::on_pushButton_sendReply_clicked()
{
    if(ui->pushButton_sendReply->text() == "Отправить"){
        UserMessage message;
        message.chatID = currentChat.chatID;
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
            socket->write("{\"process\":\"main\", \"signal\":\"messageEddit\", \"messageID\":\"" + curMessage.messageID.toLocal8Bit()
                          + "\", \"message\":\"" + ui->textEdit_replyBox->toPlainText().toLocal8Bit()
                          + "\", \"chatID\":\"" + curMessage.chatID.toLocal8Bit() + "\"}");
            ui->pushButton_sendReply->setText("Отправить");
            ui->textEdit_replyBox->clear();
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

void MainWindow::showListChats(QString headerLabel)
{
    ui->tableWidget_chatsList-> clear();
    ui->tableWidget_chatsList->setRowCount(0);
    ui->tableWidget_chatsList->setColumnCount(2);

    QStringList horzHeaders;
    horzHeaders << headerLabel << "";
    ui->tableWidget_chatsList->setHorizontalHeaderLabels(horzHeaders);


    for(int i = 0; i < chats.count(); i++){
        ui->tableWidget_chatsList->insertRow(i);
        QString nameChat = chats.at(i).name;
        if(chats[i].countIsNotReadMessages > 0)
            nameChat += " " + chats.at(i).countIsNotReadMessages;
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(nameChat));
        ui->tableWidget_chatsList->setItem(i, 1, new QTableWidgetItem("удалить"));
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
    horzHeaders << "История вашего чата" << "" << "";
    ui->tableWidget_chatWindow->setHorizontalHeaderLabels(horzHeaders);
    if(conntents.count() == 0){
    } else {
        for(int i = 0; i < conntents.count(); i++){
            ui->tableWidget_chatWindow->insertRow(i);
            ui->tableWidget_chatWindow->setItem(i, 0, new QTableWidgetItem(conntents.at(i).senderName));
            ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(conntents.at(i).message));
            ui->tableWidget_chatWindow->setItem(i, 2, new QTableWidgetItem("удалить"));
            QString text = "";
            if(conntents.at(i).senderID == userInfo.userID){
                text+="ред.";
            }
            ui->tableWidget_chatWindow->setItem(i, 3, new QTableWidgetItem(text));
        }
    }
    ui->tableWidget_chatWindow->resizeColumnsToContents();
    ui->tableWidget_chatWindow->resizeRowsToContents();
}
void MainWindow::showNewMessage(QList<UserMessage> messages)
{
    int rowCount = ui->tableWidget_chatWindow->rowCount();
    for(auto&message:messages){
        curChatContent.append(message);
        ui->tableWidget_chatWindow->insertRow(rowCount);
        ui->tableWidget_chatWindow->setItem(rowCount, 0, new QTableWidgetItem(message.senderName));
        ui->tableWidget_chatWindow->setItem(rowCount, 1, new QTableWidgetItem(message.message));
        ui->tableWidget_chatWindow->setItem(rowCount, 2, new QTableWidgetItem("удалить"));
        QString text = "";
        if(message.senderID == userInfo.userID){
            text+="ред.";
        }
        ui->tableWidget_chatWindow->setItem(rowCount, 3, new QTableWidgetItem(text));
    }
}

void MainWindow:: clickChatList(int i, int  j)
{
    if(j == 1){
        clickedDeleteChat(i);
    }
    else {
        ui->tableWidget_chatWindow->clear();
        if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Найденные люди"){
            QMessageBox::information(this, "Информация(MainWindow)", "Пока это поле недоступно");
        } else {
            currentChat = chats[i];
            ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(currentChat.name));
            ui->label_chatName->setText(chats[i].name);
            ui->label_chatPhoto->setText(chats[i].chatID);
            socket->write("{\"process\":\"main\", \"signal\":\"getChatContent\", \"chatID\":\"" + chats[i].chatID.toLocal8Bit() + "\"}");
        }
    }
}

void MainWindow::clickedDeleteChat(int i)
{
    QMessageBox::information(this, "Информация(MainWindow)", "row is " + chats.at(i).name);
    socket->write("{\"process\":\"main\", \"signal\":\"deleteChat\", \"chatID\":\"" + chats.at(i).chatID.toLocal8Bit() + "\"}");
    ui->tableWidget_chatsList->removeRow(i);
}
void MainWindow:: clickChatWindow(int i, int j)
{
    curMessage = curChatContent.at(i);
    if(j == 2){
        clickedDeleteMessage(i);
    } else if(j == 3 && (ui->tableWidget_chatWindow->item(i, j)->text() == "ред.")){
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
            ui->tableWidget_chatWindow->removeRow(i);
            curChatContent.removeAt(i);
        }
    }
}

void MainWindow::clickedEdditMessage(int i)
{
    ui->textEdit_replyBox->setText(ui->tableWidget_chatWindow->item(i, 1)->text());
    ui->pushButton_sendReply->setText("Редактировать");
    ui->pushButton_sendReply->adjustSize();
}

void MainWindow::deleteMessage(QString messageID)
{
    for(int i = curChatContent.count() - 1; i >= 0; i--){
            if(curChatContent.at(i).messageID == messageID){
                if(currentChat.chatID == curChatContent.at(i).chatID){
                    ui->tableWidget_chatWindow->removeRow(i);
                    curChatContent.removeAt(i);
                }
                return;
            }
        }
}
void MainWindow::updateMessage(QString messageID, QString message)
{
    for(int i = curChatContent.count() - 1; i >= 0; i--){
            if(curChatContent.at(i).messageID == messageID){
                if(currentChat.chatID == curChatContent.at(i).chatID){
                    ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(message));
                    curChatContent[i].message = message;
                }
                return;
            }
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
