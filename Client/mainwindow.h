#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QStandardItem>
#include <QSslSocket>
#include <QTranslator>
#include "dataparsing.h"
#include "userchat.h"
#include "userinfo.h"
#include "usermessage.h"
#include "createchat.h"
#include "userlook.h"
#include "usersettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSslSocket* socket, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openWindow();
    void sUserOutput(int i);


public slots:
    void sockReady(DataParsing messageFromServer);
private slots:
    void quit();
    void lookSelfUser();
    void settingsSelfUser();
    void searchChatList();
    void clickChatList(int i, int j);
    void clickedDeleteChat(int i, QString signal);
    void clickChatWindow(int i, int j);
    void clickedDeleteMessage(int i);
    void clickedEdditMessage(int i);
    void on_pushButton_sendReply_clicked();

    void on_pushButton_createChat_clicked();

    void sockWrite(QString process, QString signal);
    void sockWrite(QString process, QString signal, QString message);
    void sockWrite(QString process, QString signal, QList<UserChat> allChats);
    void sockWrite(QString process, QString signal, UserChat chat);
    void sockWrite(QString process, QString signal, QList<UserMessage> conntents);
    void sockWrite(QString process, QString signal, UserMessage conntent);
    void sockWrite(QString process, QString signal, QList<UserInfo> clientInfos);
    void sockWrite(QString process, QString signal, UserInfo clientInfo);

    void on_radioButtonPeople_clicked();

    void on_radioButton_Chats_clicked();

    void on_tableWidget_chatWindow_cellDoubleClicked(int row, int column);

private:
    enum actions{
        cancelEditMessage,
        deletMessage,
        editMessage,
        sendMessage};
    actions curAction;
    Ui::MainWindow *ui;
    QSslSocket* socket;

    UserInfo curUserInfo;
    QList<UserChat> allChats;
    QList<UserChat> deleteChats;
    QList<UserInfo> users;
    QList<UserMessage> curChatContent;
    UserMessage curMessage;
    UserChat curChat;
    UserInfo curUser;

    CreateChat* createChat;

    bool isSendORisEdit;// 0 - send, 1 edit
    QTranslator qtLanguageTranslator;
    //    QIcon iCancelEdit;
//    QIcon iDelete;
//    QIcon iEdit;
//    QIcon iSendOrEdit;
//    QIcon iIsRead;

    void setUserInfo(UserInfo curUserInfo);
    void setUserChats(QList<UserChat> chat);
    void renameDialogOnNameCompanion();
    void setAllUsers(QList<UserInfo> users);
    void setSearchPeople(QList<UserInfo> user);
    void setSearchChats(QList<UserChat> allChats);
    void setChatContent(QList<UserMessage> conntent);
    void setNewMessage(UserMessage message);
    void setNewChat(UserChat chat);
    void setUsersInWindowCreateChat(QList<UserInfo> users);
    void updateIsReadingMessages(QString chatID);
    void updateEdditUser(UserInfo user);

    void showListChats(QString headerLabel);
    void showUsers(QString headerLabel);
    void showChatContents(QList<UserMessage> conntents);
    void showNewMessage(UserMessage message);
    void showNewChat(UserChat chat);
    void clearChatWindow();

    void deleteMessage(UserMessage message);
    void deleteChat(QString chatID);
    void updateMessage(QString messageID, QString message);
    void deleteParticipant(UserInfo curUserInfo);
    int searchChatByID(QList<UserChat> chats, QString chatID);
    int searchMessageByID(QList<UserMessage> messages, QString messageID);
    int searchUserByID(QList<UserInfo> users, QString userID);

    void deleteMenuEdditMessage();
};
#endif // MAINWINDOW_H
