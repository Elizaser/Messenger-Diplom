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
#include "createchat.h"

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


public slots:
    void sockReady(DataParsing messageFromServer);
private slots:
    void search();
    void quit();
    void clickChatList(int i, int j);
    void clickedDeleteChat(int i);
    void clickChatWindow(int i, int j);
    void clickedDeleteMessage(int i);
    void clickedEdditMessage(int i);
    void on_pushButton_sendReply_clicked();

    void on_pushButton_createChat_clicked();

    void sockWrite(QString process, QString signal);
    void sockWrite(QString process, QString signal, QString message);
    void sockWrite(QString process, QString signal, QList<UserChat> chats);
    void sockWrite(QString process, QString signal, UserChat chat);
    void sockWrite(QString process, QString signal, QList<UserMessage> conntents);
    void sockWrite(QString process, QString signal, UserMessage conntent);
    void sockWrite(QString process, QString signal, QList<UserInfo> clientInfos);
    void sockWrite(QString process, QString signal, UserInfo clientInfo);

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;

    UserInfo userInfo;
    QList<UserChat> chats;
    QList<UserInfo> users;
    UserChat currentChat;
    QList<UserMessage> curChatContent;
    UserMessage curMessage;
    CreateChat* createChat;


    void setUserInfo(UserInfo userInfo);
    void setUserChats(QList<UserChat> chat);
    void searchPeople(QList<UserInfo> user);
    void searchChats(QList<UserChat> chats);
    void setChatContent(QList<UserMessage> conntent);
    void setNewMessage(UserMessage message);
    void setNewChat(UserChat chat);

    void showListChats(QString headerLabel);
    void showFoundUsers(QString headerLabel);
    void showChatContents(QList<UserMessage> conntents);
    void showNewMessage(QList<UserMessage> message);
    void deleteMessage(QString messageID);
    void updateMessage(QString messageID, QString message);
    void deleteParticipant(UserInfo userInfo);


};
#endif // MAINWINDOW_H
