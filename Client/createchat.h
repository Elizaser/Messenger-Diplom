#ifndef CREATECHAT_H
#define CREATECHAT_H

#include <QWidget>
#include "QTcpSocket"
#include "userinfo.h"
#include "userchat.h"

namespace Ui {
class CreateChat;
}

class CreateChat : public QWidget
{
    Q_OBJECT

signals:
    void sockWrite(QString proces, QString signal, UserChat userChat) ;

public:
    explicit CreateChat(QTcpSocket* socket, QWidget *parent = nullptr);
    ~CreateChat();
private slots:
    void addUserInChat(int i);
    bool isUserAdd(UserInfo userInfo);
    void deleteUserInChat(int i, int j);
    void on_pushButton_createChat_clicked();

private:
    Ui::CreateChat *ui;
    QTcpSocket* socket;
    QList<UserInfo> usersInChat;
    QList<UserInfo> allUsers;
    void search();
    void showAllUsers(QList<UserInfo> allUsers);

public:
    void start();
    void setAllUsers(QList<UserInfo> allUsers);
};

#endif // CREATECHAT_H
