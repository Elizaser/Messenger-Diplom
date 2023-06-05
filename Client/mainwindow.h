#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTcpSocket>
#include <QString>
#include <QStandardItem>
#include <QTcpSocket>
#include "dataparsing.h"
#include "userchat.h"
#include "userinfo.h"
#include "usermessage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QTcpSocket* socket, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openWindow();
    void sUserOutput(int i);


private slots:
    void search();
    void quit();
    void clickChatList(int i, int j);
    void clickedDeleteChat(int i);
    void clickChatWindow(int i, int j);
    void clickedDeleteMessage(int i);
    void clickedEdditMessage(int i);
    void on_pushButton_sendReply_clicked();

public slots:
    void sockReady(DataParsing messageFromServer);

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;

    UserInfo userInfo;
    QList<UserChat> chats;
    QList<UserInfo> users;
    UserChat currentChat;
    QList<UserMessage> curChatContent;
    UserMessage curMessage;

    void sockWrite(QString process, QString signal, QString message);

    void showListChats(QString headerLabel);
    void showFoundUsers(QString headerLabel);
    void showChatContents(QList<UserMessage> conntents);
    void showNewMessage(QList<UserMessage> message);
    void deleteMessage(QString messageID);
    void updateMessage(QString messageID, QString message);
    void deleteParticipant(UserInfo userInfo);

};
#endif // MAINWINDOW_H
