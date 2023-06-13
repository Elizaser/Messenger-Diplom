#ifndef USERLOOK_H
#define USERLOOK_H

#include <QWidget>
#include "userinfo.h"

namespace Ui {
class UserLook;
}

class UserLook : public QWidget
{
    Q_OBJECT

public:
    explicit UserLook(UserInfo user, QWidget *parent = nullptr);
    ~UserLook();

private slots:
    void on_pushButton_OK_clicked();

private:
    Ui::UserLook *ui;
};

#endif // USERLOOK_H
