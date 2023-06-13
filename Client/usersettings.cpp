#include "usersettings.h"
#include "ui_usersettings.h"
#include <QDebug>

UserSettings::UserSettings(UserInfo user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserSettings)
{
    ui->setupUi(this);
    this->user = user;
    qDebug() << "UserSettings::UserSettings this->user.userID" << this->user.userID;
//    emit sockWrite("main", "edditUserGetPassword");

    showUserInfo();

}

void UserSettings::showUserInfo()
{
    ui->lineEdit_login->setText(user.login);
    ui->lineEdit_name->setText(user.name);
    ui->textEdit_status->setText(user.status);

    ui->lineEdit_login->setReadOnly(true);
    ui->lineEdit_password->setReadOnly(true);
    ui->lineEdit_name->setReadOnly(true);
    ui->textEdit_status->setReadOnly(true);
}

UserSettings::~UserSettings()
{
    delete ui;
}

void UserSettings::on_pushButton_login_clicked()
{
    if(ui->pushButton_login->text() == "ред.") {
        ui->lineEdit_login->setReadOnly(false);
        ui->pushButton_login->setText("Отмена");
    } else if(ui->pushButton_login->text() == "Отмена"){
        ui->lineEdit_login->setText(user.login);
        ui->lineEdit_login->setReadOnly(true);
        ui->pushButton_login->setText("ред.");
    }
}

void UserSettings::on_pushButton_password_clicked()
{
    if(ui->pushButton_password->text() == "ред.") {
        ui->lineEdit_password->clear();
        ui->lineEdit_password->setReadOnly(false);
        ui->pushButton_password->setText("Отмена");
    } else if(ui->pushButton_password->text() == "Отмена"){
        ui->lineEdit_password->setText("**********");
        ui->lineEdit_password->setReadOnly(true);
        ui->pushButton_password->setText("ред.");
    }
}

void UserSettings::on_pushButton_name_clicked()
{
    if(ui->pushButton_name->text() == "ред.") {
        ui->lineEdit_name->setReadOnly(false);
        ui->pushButton_name->setText("Отмена");
    } else if(ui->pushButton_name->text() == "Отмена"){
        ui->lineEdit_name->setText(user.name);
        ui->lineEdit_name->setReadOnly(true);
        ui->pushButton_name->setText("ред.");
    }
}

void UserSettings::on_pushButton_status_clicked()
{
    if(ui->pushButton_status->text() == "ред.") {
        ui->textEdit_status->setReadOnly(false);
        ui->pushButton_status->setText("Отмена");
    } else if(ui->pushButton_status->text() == "Отмена"){
        ui->textEdit_status->setText(user.status);
        ui->textEdit_status->setReadOnly(true);
        ui->pushButton_status->setText("ред.");
    }
}

void UserSettings::on_pushButton_OK_clicked()
{
    bool f = false;
    ui->label_error->clear();
    if(ui->pushButton_login->text() == "Отмена"){
        f = true;
        user.login = ui->lineEdit_login->text();
    }
    if(ui->pushButton_password->text() == "Отмена"){
        f = true;
        user.password =  ui->lineEdit_password->text();
    }
    if(ui->pushButton_name->text() == "Отмена") {
        f = true;
        user.name = ui->lineEdit_name->text();
    }
    if(ui->pushButton_status->text() == "Отмена") {
        f = true;
        user.status = ui->textEdit_status->toPlainText();
    }
    qDebug() << "user.login" << user.login;
    qDebug() << "user.password" << user.password;
    qDebug() << "user.name" << user.name;
    if(user.login == "" || user.password == "" || user.name == "" ){
        ui->label_error->setText("Ошибка в заполнении");
        return;
    }
    if(!f){
        this->close();// ничего не поменялось
        return;
    }
    ConfirmationPassword* confirmationPassword = new ConfirmationPassword();
    connect(confirmationPassword, SIGNAL(confirmationPasswordIsOk(QString)), this, SLOT(sockWrite(QString)));
    confirmationPassword->show();

}
void UserSettings::sockWrite(QString password)
{
    qDebug() << "password = " << password;
    qDebug() << "user.password = " << user.password;
    if(password == user.password) {
        emit sockWrite("main", "edditUser", user);
        this->close();
    } else {
        ui->label_error->setText("Ошибка. Вы неправильно ввели пароль");
    }
}
void UserSettings::on_pushButton_cancel_clicked()
{
//    ui->label_error->clear();
    this->close();
}
