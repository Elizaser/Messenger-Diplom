#include "userlook.h"
#include "ui_userlook.h"
#include <QDebug>

UserLook::UserLook(UserInfo user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLook)
{
    ui->setupUi(this);
    ui->label_name->setText(user.name);
    ui->label_status->setText(user.status);
    qDebug() << "in UserLook";
}

UserLook::~UserLook()
{
    delete ui;
}

void UserLook::on_pushButton_OK_clicked()
{
    this->close();
}
