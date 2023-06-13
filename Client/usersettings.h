#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QWidget>
#include <QMessageBox>
#include "userinfo.h"
#include "confirmationpassword.h"

namespace Ui {
class UserSettings;
}

class UserSettings : public QWidget
{
    Q_OBJECT

signals:
    void sockWrite(QString proces, QString signal, UserInfo user) ;
public:
    explicit UserSettings(UserInfo user, QWidget *parent = nullptr);
    ~UserSettings();

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_password_clicked();

    void on_pushButton_name_clicked();

    void on_pushButton_status_clicked();

    void on_pushButton_OK_clicked();

    void on_pushButton_cancel_clicked();
    void sockWrite(QString password);

private:
    Ui::UserSettings *ui;
    UserInfo user;
    void showUserInfo();
};

#endif // USERSETTINGS_H
