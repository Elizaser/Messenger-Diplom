#include "confirmationpassword.h"
#include "ui_confirmationpassword.h"
#include<QDebug>

ConfirmationPassword::ConfirmationPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::confirmationPassword)
{
    ui->setupUi(this);
}

ConfirmationPassword::~ConfirmationPassword()
{
    delete ui;
}

void ConfirmationPassword::on_pushButton_cancel_clicked()
{
    this->close();
}

void ConfirmationPassword::on_pushButton_ok_clicked()
{
    QString password = ui->lineEdit->text();
    qDebug() << "in ConfirmationPassword inPassword = " << password;
    if(password != "")
        emit confirmationPasswordIsOk(password);
    this->close();
}
