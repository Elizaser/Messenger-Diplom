#include "createchat.h"
#include "ui_createchat.h"


CreateChat::CreateChat(QTcpSocket* socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateChat)
{
    ui->setupUi(this);
    this->socket = socket;
    connect(ui->tableWidget_allUsers, SIGNAL(cellClicked(int,int)), this, SLOT(addUserInChat(int)));
    connect(ui->tableWidget_addUsers, SIGNAL(cellClicked(int,int)), this, SLOT(deleteUserInChat(int, int)));
    connect(ui->lineEdit_searh, &QLineEdit::textChanged, this, &CreateChat::search);

    ui->tableWidget_addUsers->setRowCount(0);
    ui->tableWidget_addUsers->setColumnCount(2);
    QStringList horzHeaders;
    horzHeaders << "Пользователи в чате" << "";
    ui->tableWidget_allUsers->setHorizontalHeaderLabels(horzHeaders);
}

void CreateChat::start(){
    socket->write("{\"process\":\"main\", \"signal\":\"getUsersCreateChat\"}");
}
void CreateChat::setAllUsers(QList<UserInfo> allUsers){
    this->allUsers = allUsers;
    showAllUsers(allUsers);
}
void CreateChat::showAllUsers(QList<UserInfo> allUsers){
    ui->tableWidget_allUsers-> clear();
    ui->tableWidget_allUsers->setRowCount(0);
    ui->tableWidget_allUsers->setColumnCount(1);

    QStringList horzHeaders;
    horzHeaders << "Все пользователи";
    ui->tableWidget_allUsers->setHorizontalHeaderLabels(horzHeaders);

    for(int i = 0; i < allUsers.count(); i++){
        ui->tableWidget_allUsers->insertRow(i);
        ui->tableWidget_allUsers->setItem(i, 0, new QTableWidgetItem(allUsers.at(i).name));
    }
}

void CreateChat::addUserInChat(int i)
{
    if(isUserAdd(allUsers.at(i))) return;
    usersInChat.append(allUsers.at(i));
    int rowCount  = ui->tableWidget_addUsers->rowCount();
    ui->tableWidget_addUsers->insertRow(rowCount);
    ui->tableWidget_addUsers->setItem(rowCount, 0, new QTableWidgetItem(allUsers.at(i).name));
    ui->tableWidget_addUsers->setItem(rowCount, 1, new QTableWidgetItem("удалить"));
}
bool CreateChat::isUserAdd(UserInfo userInfo)
{
    for(auto&user:usersInChat){
        if(user.userID == userInfo.userID)
            return true;
    }
    return false;
}

void CreateChat::deleteUserInChat(int i, int j){
    if(j == 1){
        usersInChat.removeAt(i);
        ui->tableWidget_addUsers->removeRow(i);
    }
}

void CreateChat::search(){
    socket->write("{\"process\":\"main\", \"signal\":\"getUsersCreateChat\", \"user\":\"" +
                  ui->lineEdit_searh->text().toLocal8Bit() + "\"}");
}

CreateChat::~CreateChat()
{
    delete ui;
}

void CreateChat::on_pushButton_createChat_clicked()
{
    QString nameChat = ui->lineEdit_nameChat->text();
    if( nameChat == "" || nameChat.count() > 100){
        ui->label_errorNameChat->setText("Ошибка");
    }  else if(usersInChat.count() < 2){
        ui->label_errorUsersInChat->setText("Слишком мало пользователей");
    } else {
        UserChat userChat;
        userChat.name = nameChat;
        userChat.type = "group";
        for(auto&user:usersInChat){
            userChat.participants.append(user.userID);
        }
        emit sockWrite("main", "createChat", userChat);
        this->close();
    }
}
