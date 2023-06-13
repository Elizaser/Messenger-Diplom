#include "userlook.h"
#include "ui_user.h"

UserLook::UserLook(UserInfo user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLook)
{
    ui->setupUi(this);
    ui->label_name->setText(user.name);
    ui->label_status->setText(user.status);

}

UserLook::~UserLook()
{
    delete ui;
}

void UserLook::on_pushButton_OK_clicked()
{
    this->close();
}
