 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QTextEdit>

MainWindow::MainWindow(QSslSocket* socket, QWidget *parent)
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
    connect(ui->action_look, SIGNAL(triggered(bool)),this,  SLOT(lookSelfUser()));
    connect(ui->action_settings, SIGNAL(triggered(bool)),this,  SLOT(settingsSelfUser()));

    ui->tableWidget_chatsList->setShowGrid(false);
    ui->tableWidget_chatWindow->setShowGrid(false);
    ui->tableWidget_chatsList->setEditTriggers(0);
    ui->tableWidget_chatWindow->setEditTriggers(0);
    ui->tableWidget_chatsList->verticalHeader()->hide();
    ui->tableWidget_chatsList->horizontalHeader()->hide();
    ui->tableWidget_chatWindow->verticalHeader()->hide();
    ui->tableWidget_chatWindow->horizontalHeader()->hide();
    ui->tableWidget_chatWindow->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget_chatWindow->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_chatWindow->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_chatWindow->setStyleSheet(
                    "QTableWidget::item:selected:active {"
                    "background: rgb(255, 255,255);"
                    "border: 1px solid transparent;"
                    "selection-color: #3A6B35;"
                    "}"
                    );

    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentIndex(1);

    isSendORisEdit = false;
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
            if(messageFromServer.getChatID() != "") {
                int i = searchChatByID(allChats, messageFromServer.getChat().chatID);
                if(i == -1) {
                    setNewChat(messageFromServer.getChat());
                    i = searchChatByID(allChats, messageFromServer.getChat().chatID);
                }
                allChats[i].countIsNotReadMessages = "0";
                curChat = allChats[i];
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
            curChat = messageFromServer.getChat();
            setNewChat(messageFromServer.getChat());
        } else if(signal == "deleteParticipant") {
        } else if(signal == "setUsersCreateChat") {
            setUsersInWindowCreateChat(messageFromServer.getUsers());
        } else if(signal == "updateIsReadingMessages") {
            updateIsReadingMessages(messageFromServer.getChatID());
        } else if(signal == "updateEdditUser") {
            updateEdditUser(messageFromServer.getUser());
        } else {
            qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(MainWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}



void MainWindow::quit()
{
    curUserInfo.login = "";
    curUserInfo.name = "";
    curUserInfo.password = "";
    ui->tabWidget->setCurrentIndex(1);
    this->close();
    emit openWindow();
}

void MainWindow::lookSelfUser()
{
    UserLook *userLook = new UserLook(curUserInfo);
    userLook->show();
}
void MainWindow::settingsSelfUser()
{
    UserSettings *userSettings = new UserSettings(curUserInfo);
    connect(userSettings, SIGNAL(sockWrite(QString,QString, UserInfo)), this, SLOT(sockWrite(QString, QString, UserInfo)));
    userSettings->show();
}


void MainWindow:: clickChatList(int i, int  j)
{
    ui->tabWidget->setCurrentIndex(0);
//    if(ui->tableWidget_chatsList->item(i, j)->text() == "") return;
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Чаты" ||
            ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Найденные чаты" || ui->comboBox->currentIndex() == 0){
        if(j == 0){
            qDebug() << "before clickChatList curChat.chatID = " << curChat.chatID;
            clearChatWindow();
            qDebug() << "after clickChatList curChat.chatID = " << curChat.chatID;

            curChat = allChats[i];
            ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(curChat.name));
            ui->label_chatName->setText(allChats[i].name);
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
        if(j == 0) {
            UserLook *user = new UserLook(curUser);
            user->show();
        } else if(j == 1) {
            clearChatWindow();
            ui->label_chatName->setText(curUser.name);
            sockWrite("main", "getDialogContent", "\"userID\":\"" + curUser.userID.toLocal8Bit() + "\"");
        }
//        QMessageBox::information(this, "Информация(MainWindow)", "Пока это поле недоступно");
    }
}
void MainWindow::clickedDeleteChat(int i, QString signal)
{
    qDebug() << "clickedDeleteChat i = " << i;
    if(!curChatContent.isEmpty()) {
        if(curChatContent.at(0).chatID == allChats.at(i).chatID)
            ui->tableWidget_chatWindow->clear();
            ui->tableWidget_chatWindow->setRowCount(0);
            curChatContent.clear();
    }
    qDebug() << "ui->tableWidget_chatsList->rowCount() = " << ui->tableWidget_chatsList->rowCount();
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Найденные люди")
        ui->tableWidget_chatsList->removeRow(i);
    sockWrite("main", signal, "\"chatID\":\"" + allChats.at(i).chatID.toLocal8Bit() + "\"");
    if(signal == "deleteChat"){
        deleteChats.append(allChats.at(i));
    }
    allChats.removeAt(i);
    clearChatWindow();
}

void MainWindow:: clickChatWindow(int i, int j)
{
    if(ui->tableWidget_chatWindow->columnCount() != 6 || ui->tableWidget_chatWindow->item(i, 5)->text() != "Отмена") return;
    curMessage = curChatContent.at(i);
    if(j == 3){
        clickedDeleteMessage(i);
    } else if(j == 4){
        clickedEdditMessage(i);
    }
    else if(j == 5){
        deleteMenuEdditMessage();
    }
}

void MainWindow::deleteMenuEdditMessage()
{
    ui->tableWidget_chatWindow->setColumnCount(3);
}
void MainWindow::on_tableWidget_chatWindow_cellDoubleClicked(int i, int j)
{
    if(ui->tableWidget_chatWindow->item(i, 0)->text() == "" ){
        ui->tableWidget_chatWindow->clearSelection();
        return;
    }
    ui->tableWidget_chatWindow->setColumnCount(6);
    QStringList horzHeaders;

    horzHeaders << "История вашего чата" << "" << "" << "" << "" << "";

    ui->tableWidget_chatWindow->setHorizontalHeaderLabels(horzHeaders);
    ui->tableWidget_chatWindow->setItem(i, 3, new QTableWidgetItem());
    ui->tableWidget_chatWindow->setItem(i, 4, new QTableWidgetItem());
    ui->tableWidget_chatWindow->setItem(i, 5, new QTableWidgetItem("Отмена"));


    for(int  j = 0; j < ui->tableWidget_chatWindow->rowCount(); j++) {
        if(j != i) {
            ui->tableWidget_chatWindow->setItem(j, 3, new QTableWidgetItem());
            ui->tableWidget_chatWindow->setItem(j, 4, new QTableWidgetItem());
            ui->tableWidget_chatWindow->setItem(j, 5, new QTableWidgetItem());
        }
    }

    qDebug() << "curChatContent.at(i)" << curChatContent.at(i).message << " isSystem - " << curChatContent.at(i).isSystem;
    QIcon iCancelEdit;
    iCancelEdit.addFile("//home//liza//diplom//Client//icons//cancelEditMenuMessage.png");
    ui->tableWidget_chatWindow->item(i, 5)->setIcon(iCancelEdit);

    QIcon iDelete;
    if(curChatContent.at(i).isSystem != "1") {
        iDelete.addFile("//home//liza//diplom//Client//icons//trash.png");
    }
    ui->tableWidget_chatWindow->item(i, 3)->setIcon(iDelete);


    QIcon iEdit;
    if(curChatContent.at(i).senderID == curUserInfo.userID &&
        curChatContent.at(i).isSystem != "1"){
         iEdit.addFile("//home//liza//diplom//Client//icons//edit.png");
     }
     ui->tableWidget_chatWindow->item(i, 4)->setIcon(iEdit);

     ui->tableWidget_chatWindow->setColumnWidth(0, 80);
     ui->tableWidget_chatWindow->setColumnWidth(1, 400);
     ui->tableWidget_chatWindow->setColumnWidth(2, 20);
     ui->tableWidget_chatWindow->setColumnWidth(3, 3);
     ui->tableWidget_chatWindow->setColumnWidth(4, 3);
     ui->tableWidget_chatWindow->setColumnWidth(5, 50);
}
void MainWindow::clickedDeleteMessage(int i)
{
    QMessageBox msgBox;
    msgBox.setText("Внимание");
    msgBox.setInformativeText("Вы действительно хотите удалить сообщение?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    QCheckBox* cb = new QCheckBox("&Удалить для всех");
    cb->setChecked(false);
    if(curChatContent.at(i).senderID == curUserInfo.userID){
        msgBox.setCheckBox(cb);
    }
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.show();
    int res = msgBox.exec();
    if (res == QMessageBox::Ok)  {
        deleteMenuEdditMessage();
        if(cb->isChecked()) {
            sockWrite("main", "messageDeleteForEveryone", curChatContent.at(i));
        } else {
            if(curChatContent.count() == 1) { // если осталось последнее сообщение, затираем чат
                i = searchChatByID(allChats, curChatContent.at(i).chatID);
                clickedDeleteChat(i, "deleteChat");
            }
            else
                sockWrite("main", "messageDelete", curChatContent.at(i));
        }
    }
}
void MainWindow::clickedEdditMessage(int i)
{
//    deleteMenuEdditMessage();
    ui->textEdit_replyBox->setText(ui->tableWidget_chatWindow->item(i, 1)->text());
    QIcon iEdit;
    iEdit.addFile("/home/liza/diplom/Client/icons/edit.png");
    ui->pushButton_sendReply->setIcon(iEdit);
    ui->pushButton_sendReply->adjustSize();
    isSendORisEdit = true;
}

void MainWindow::setNewChat(UserChat chat)
{
    allChats.append(chat);
    renameDialogOnNameCompanion();
    showNewChat(chat);

}
void MainWindow::deleteChat(QString chatID)
{
    int i = searchChatByID(allChats, chatID);
    if(i != -1){
        if( !(ui->tableWidget_chatsList->horizontalHeaderItem(i)->text() != "Найденные люди")){
            ui->tableWidget_chatsList->removeRow(i);
            allChats.removeAt(i);
        }
    }
}
void MainWindow::setNewMessage(UserMessage message)
{
    qDebug() << "setNewMessage curChat.chatID = " << curChat.chatID;
    if((curChat.chatID == "" || curChat.chatID != message.chatID) &&
    (  ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Найденные чаты"
    || ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты" )) {
        int i = searchChatByID(allChats, message.chatID);
        if(i == -1){
            qDebug() << " i = -1";
            i = searchChatByID(deleteChats, message.chatID);
            if(i == -1) {
                return;
            }
            deleteChats[i].countIsNotReadMessages = "1";
            setNewChat(deleteChats.at(i));
            deleteChats.removeAt(i);
            return;
        }
        allChats[i].countIsNotReadMessages = QString::number(allChats.at(i).countIsNotReadMessages.toInt() + 1);
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(allChats[i].name + " " + allChats[i].countIsNotReadMessages));
    } else {
        curChatContent.append(message);
        showNewMessage(message);
        if(message.senderID != curUserInfo.userID)
            sockWrite("main", "isReadingMessage", "\"chatID\":\""
                      + message.chatID + "\"");
    }

}
void MainWindow::deleteMessage(UserMessage message)
{
    int i = searchMessageByID(curChatContent, message.messageID);

    if(i != -1) {
        if(!(curChatContent.isEmpty()) && curChatContent.at(i).chatID == message.chatID) {
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
                    QIcon iIsRead;
                    if(curChatContent.at(i).isRead == "1")
                        iIsRead.addFile("//home//liza//diplom//Client//icons//double-check.png");
                    else
                        iIsRead.addFile("/home/liza/diplom/Client/icons/check.png");
                    ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(message));
                    ui->tableWidget_chatWindow->item(i, 1)->setIcon(iIsRead);
                    isSendORisEdit = true;
                    curChatContent[i].message = message;
                }
                return;
            }
        }
}
int MainWindow::searchChatByID(QList<UserChat> chats, QString chatID)
{
    for(int i = chats.count() - 1; i >= 0; i--){
        if(chats.at(i).chatID == chatID){
            return i;
        }
    }
    return (-1);
}
int MainWindow::searchMessageByID(QList<UserMessage> messages, QString messageID)
{
    for(int i = messages.count() - 1; i >= 0; i--){
        if(curChatContent.at(i).messageID == messageID){
            return i;
        }
    }
    return (-1);
}
int MainWindow::searchUserByID(QList<UserInfo> users, QString userID)
{
    for(int i = users.count() - 1; i >= 0; i--){
        if(users.at(i).userID == userID){
            return i;
        }
    }
    return (-1);
}

void MainWindow::setUserInfo(UserInfo userInfo)
{
    this->curUserInfo = userInfo;
    sockWrite("main", "setUserInfo", "\"login\":\"" +
              userInfo.login + "\", \"password\":\"" +
              userInfo.password + "\"");
}
void MainWindow::setUserChats(QList<UserChat> chats)
{
    this->allChats = chats;
    for(int  i = 0; i < allChats.count(); i++){
        qDebug() << "allChats[i].name" << allChats[i].name;
//        qDebug() << "allChats[i].name" << allChats[i].name;
    }
    renameDialogOnNameCompanion();
    showListChats("Мои чаты");

}
void MainWindow::renameDialogOnNameCompanion(){
    for(auto&chat:allChats){
        if(chat.type == "dialog"){
            foreach (auto key, chat.participants.keys()){
                if(key != curUserInfo.userID)
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
    this->allChats = chats;
    renameDialogOnNameCompanion();
    showListChats("Найденные чаты");
}
void MainWindow::setChatContent(QList<UserMessage> conntents)
{
    curChatContent.clear();
    qDebug() << "setChatContent curChat.chatID = " << curChat.chatID;
    UserMessage message;
    for(int conntentI = 0; conntentI < conntents.count(); conntentI++) {
        if(conntentI == 0 || conntents.at(conntentI - 1).date != conntents.at(conntentI).date){
            message.date = conntents.at(conntentI).date;
            message.message = "";
            curChatContent.append(message);
        }
        curChatContent.append(conntents.at(conntentI));
    }
    // сверху добавляем пустые поля, для даты в таблице
    showChatContents(curChatContent);
    int i = searchChatByID(allChats, curChat.chatID);
    if(i == -1) return;
    if(allChats[i].countIsNotReadMessages == "0" || allChats[i].countIsNotReadMessages == "") return;
    sockWrite("main", "isReadingMessage", allChats[i]);
    allChats[i].countIsNotReadMessages = "0";
}
void MainWindow::searchChatList()
{
    clearChatWindow();
    QString desired = ui->lineEdit_searchUser->text();
    if(ui->comboBox->currentIndex() == 1) {
        if(desired == "") {
            if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Все пользователи")
                socket->write("{\"process\":\"main\", \"signal\":\"getAllUsers\"}");
            return;
        }
        socket->write("{\"process\":\"main\", \"signal\":\"searchPeople\", \"user\":\"" + desired.toLocal8Bit() + "\"}");
    } else if(ui->comboBox->currentIndex() == 0){
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
    qDebug() << "clearChatWindow";
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
        qDebug() << "clearChatWindow curChat.chatID = " << curChat.chatID;
}

void MainWindow::setUsersInWindowCreateChat(QList<UserInfo> users)
{
    int i = searchUserByID(users, curUserInfo.userID);
    users.removeAt(i);
    createChat->setAllUsers(users);
}
void MainWindow::updateIsReadingMessages(QString chatID)
{
    qDebug() << "curChatContent.size() = " <<curChatContent.size();
//    for()
    for(int i = curChatContent.size() - 1, row = ui->tableWidget_chatWindow->rowCount() - 1; i >= 0; i--, row--){
        if(ui->tableWidget_chatWindow->item(row , 0)->text() == "") break;
        if(curChatContent[i].chatID != chatID) break;
        if(curChatContent[i].isRead == "1") break;
        curChatContent[i].isRead = "1";
        QIcon iIsRead;
        iIsRead.addFile("//home//liza//diplom//Client//icons//double-check.png");
        ui->tableWidget_chatWindow->item(row, 1)->setIcon(iIsRead);
    }
}
void MainWindow:: updateEdditUser(UserInfo user)
{
    if(user.userID == curUserInfo.userID){
        curUserInfo = user;
    } else{
        int i = searchUserByID(users, user.userID);
        if(i != -1)
            users[i] = user;
    }
}

void MainWindow::showListChats(QString headerLabel)
{
//    ui->tableWidget_chatsList->horizontalHeaderItem(0)->setText("Чаты");
    ui->tableWidget_chatsList-> clear();
    ui->tableWidget_chatsList->setRowCount(0);
    ui->tableWidget_chatsList->setColumnCount(3);

    QStringList horzHeaders;
    horzHeaders << headerLabel << "" << "";
    ui->tableWidget_chatsList->setHorizontalHeaderLabels(horzHeaders);


    for(int i = 0; i < allChats.count(); i++) {
        if(allChats[i].countIsLook == "0") break;
        ui->tableWidget_chatsList->insertRow(i);
        QString nameChat = allChats.at(i).name;

        if(allChats[i].countIsNotReadMessages != "0") {
            nameChat += " " + allChats.at(i).countIsNotReadMessages;
        }
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(nameChat));
        ui->tableWidget_chatsList->setItem(i, 1, new QTableWidgetItem());
        ui->tableWidget_chatsList->setItem(i, 2, new QTableWidgetItem());

        QIcon iClear;
        iClear.addFile("//home//liza//diplom//Client//icons//clear.png");
        ui->tableWidget_chatsList->item(i, 1)->setIcon(iClear);

        QIcon iOut;
        if(allChats[i].type == "group") {
            iOut.addFile("//home//liza//diplom//Client//icons//out.png");
            ui->tableWidget_chatsList->item(i, 2)->setIcon(iOut);
        }
    }

    ui->tableWidget_chatsList->setColumnWidth(0, 200);
}
void MainWindow::showUsers(QString headerLabel)
{
//    ui->tableWidget_chatsList->horizontalHeaderItem(0)->setText("Пользователи");
//    ui->tableWidget_chatsList->horizontalHeaderItem(0)->setText("Мои               чаты");
    ui->tableWidget_chatsList-> clear();
    ui->tableWidget_chatsList->setRowCount(0);
    ui->tableWidget_chatsList->setColumnCount(3);

    QStringList horzHeaders;
    horzHeaders << headerLabel << "" << "";
    ui->tableWidget_chatsList->setHorizontalHeaderLabels(horzHeaders);

    for(int i = 0; i < users.count(); i++){
        ui->tableWidget_chatsList->insertRow(i);
        ui->tableWidget_chatsList->setItem(i, 0, new QTableWidgetItem(users.at(i).name));
        ui->tableWidget_chatsList->setItem(i, 1, new QTableWidgetItem());

//        ui->tableWidget_chatsList->setItem(i, 2, new QTableWidgetItem());
        qDebug() << "users.at(i).statusInLine = " << users.at(i).statusInLine;
        QIcon iLive;
        if(users.at(i).statusInLine == "Online")
            iLive.addFile("//home//liza//diplom//Client//icons//online.png");
        else
            iLive.addFile("/home/liza/diplom/Client/icons/offline.png");
        ui->tableWidget_chatsList->item(i, 0)->setIcon(iLive);

        QIcon iWrite;
        iWrite.addFile("//home//liza//diplom//Client//icons//write.png");
        ui->tableWidget_chatsList->item(i, 1)->setIcon(iWrite);
    }
    ui->tableWidget_chatsList->setColumnWidth(0, 200);
}
void MainWindow::showChatContents(QList<UserMessage> conntents)
{
    ui->tableWidget_chatWindow-> clear();
    ui->tableWidget_chatWindow->setRowCount(0);
    ui->tableWidget_chatWindow->setColumnCount(3);
    QStringList horzHeaders;
    horzHeaders << "" << "" << "" << "" << "";
    ui->tableWidget_chatWindow->setHorizontalHeaderLabels(horzHeaders);

    for(int i = 0; i < conntents.count(); i++) {
        if(conntents.at(i).message == ""){
            ui->tableWidget_chatWindow->insertRow(i);
            ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(conntents.at(i).date));
            ui->tableWidget_chatWindow->setItem(i, 0, new QTableWidgetItem());
            QColor color = QColor::fromRgb(255,255,255,255);
            ui->tableWidget_chatWindow->item(i, 1)->setTextColor(color);
            ui->tableWidget_chatWindow->setItem(i, 2, new QTableWidgetItem());

            setColortoRow(ui->tableWidget_chatWindow, i);
            continue;
        }
        ui->tableWidget_chatWindow->insertRow(i);
        ui->tableWidget_chatWindow->setItem(i, 1, new QTableWidgetItem(conntents.at(i).message));
        ui->tableWidget_chatWindow->setItem(i, 0, new QTableWidgetItem(conntents.at(i).senderName));
        if(conntents.at(i).isSystem != "1") {
            QIcon iIsRead;
            if(conntents.at(i).isRead == "1")
                iIsRead.addFile("//home//liza//diplom//Client//icons//double-check.png");
            else
                iIsRead.addFile("/home/liza/diplom/Client/icons/check.png");
            ui->tableWidget_chatWindow->item(i, 1)->setIcon(iIsRead);
        }

        ui->tableWidget_chatWindow->setItem(i, 2, new QTableWidgetItem(conntents.at(i).time));
    }
    ui->tableWidget_chatWindow->setColumnWidth(0, 80);
    ui->tableWidget_chatWindow->setColumnWidth(1, 400);
    ui->tableWidget_chatWindow->setColumnWidth(2, 20);
    ui->tableWidget_chatWindow->scrollToBottom();
}
void MainWindow:: setColortoRow(QTableWidget* table, int rowIndex) {
    QColor color = QColor::fromRgb(39,78,19,120);
    for(int i = 0; i < ui->tableWidget_chatWindow->columnCount(); i++ )
        table->item(rowIndex, i)->setBackgroundColor(color);
//    ui->tableWidget_chatWindow->item(0, 0)->setText("blablabla");
}
void MainWindow::showNewMessage(UserMessage message)
{
    int rowCount  =  ui->tableWidget_chatWindow->rowCount();
    ui->tableWidget_chatWindow->insertRow(rowCount);
    ui->tableWidget_chatWindow->setItem(rowCount, 0, new QTableWidgetItem(message.senderName));
    ui->tableWidget_chatWindow->setItem(rowCount, 1, new QTableWidgetItem(message.message));
    QIcon iIsRead;
    if(message.isRead == "1")
        iIsRead.addFile("//home//liza//diplom//Client//icons//double-check.png");
    else
        iIsRead.addFile("/home/liza/diplom/Client/icons/check.png");
    ui->tableWidget_chatWindow->item(rowCount, 1)->setIcon(iIsRead);
    ui->tableWidget_chatWindow->setItem(rowCount, 2, new QTableWidgetItem(message.time));
}
void MainWindow::showNewChat(UserChat chat)
{
    qDebug() << "showNewChat";
    int rowCount  =  ui->tableWidget_chatsList->rowCount();
    if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() == "Мои чаты") {
        ui->tableWidget_chatsList->insertRow(rowCount);
        QString countIsNotReadMessages = "";
        if(chat.countIsNotReadMessages != "0")
            countIsNotReadMessages = chat.countIsNotReadMessages;
        ui->tableWidget_chatsList->setItem(rowCount, 0, new QTableWidgetItem(chat.name + " " + countIsNotReadMessages));
        ui->tableWidget_chatsList->setItem(rowCount, 1, new QTableWidgetItem());
        ui->tableWidget_chatsList->setItem(rowCount, 2, new QTableWidgetItem());

        QIcon iClear;
        iClear.addFile("//home//liza//diplom//Client//icons//clear.png");
        ui->tableWidget_chatsList->item(rowCount, 1)->setIcon(iClear);

        QIcon iOut;
        if(chat.type == "group") {
            iOut.addFile("//home//liza//diplom//Client//icons//out.png");
            ui->tableWidget_chatsList->item(rowCount, 2)->setIcon(iOut);
        }
    }
    ui->tableWidget_chatsList->setColumnWidth(0, 200);
}

void MainWindow::on_pushButton_createChat_clicked()
{
    createChat = new CreateChat();
    connect(createChat, SIGNAL(sockWrite(QString,QString, UserChat)), this, SLOT(sockWrite(QString, QString, UserChat)));
    connect(createChat, SIGNAL(sockWrite(QString,QString, QString)), this, SLOT(sockWrite(QString, QString, QString)));
    connect(createChat, SIGNAL(sockWrite(QString,QString)), this, SLOT(sockWrite(QString, QString)));

    createChat->show();
    createChat->start();

}
void MainWindow::on_pushButton_sendReply_clicked()
{
    if(isSendORisEdit == false){
        UserMessage message;
        message.chatID = curChat.chatID;
        message.senderName = curUserInfo.name;
        message.senderID = curUserInfo.userID;
        QString m = message.message = ui->textEdit_replyBox->toPlainText();
        if(m.remove(' ').remove('\n') == ""){
            return;
        }
        for(int i = 0; i < message.message.count(); i+=40)
        {
            message.message.insert(i, "\n");
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
    } else {
        curMessage.message =    ui->textEdit_replyBox->toPlainText();
        sockWrite("main", "messageEddit", curMessage);
        QIcon iSend;
        iSend.addFile("/home/liza/diplom/Client/icons/send-message.png");
        ui->pushButton_sendReply->setIcon(iSend);
        ui->pushButton_sendReply->adjustSize();
        ui->textEdit_replyBox->clear();
        isSendORisEdit = false;
        deleteMenuEdditMessage();
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
                         "\", \"date\":\"" + conntent.date +
                         "\", \"time\":\"" + conntent.time +
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
                        + "\", \"date\":\"" + conntent.date.toLocal8Bit()
                        + "\", \"time\":\"" + conntent.time.toLocal8Bit()
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
                        "\", \"status\":\"" + clientInfo.status +
                        "\", \"statusInLine\":\"" + clientInfo.statusInLine +
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
                    + "\", \"status\":\"" + clientInfo.status.toLocal8Bit()
                    + "\", \"password\":\"" + clientInfo.password.toLocal8Bit()
                    + "\", \"statusInLine\":\"" + clientInfo.statusInLine.toLocal8Bit()
                    + "\", \"userID\":\"" + clientInfo.userID.toLocal8Bit() + "\"}";
    socket->write(data);
    socket->waitForBytesWritten();
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index == 1){
        clearChatWindow();
        QString desired = ui->lineEdit_searchUser->text();
        if(desired == "") {
            if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Все пользователи")
                 sockWrite("main", "getAllUsers");
            return;
        }
        sockWrite("main", "searchPeople", "\"user\":\"" + desired.toLocal8Bit() + "\"");
    } else if(index == 0) {
        clearChatWindow();
        QString desired = ui->lineEdit_searchUser->text();
        if(desired == "") {
            if(ui->tableWidget_chatsList->horizontalHeaderItem(0)->text() != "Мои чаты")
                sockWrite("main", "getUserChats");
            return;
        }
        sockWrite("main", "searchChats", "\"user\":\"" + desired.toLocal8Bit() + "\"");
    }
}
