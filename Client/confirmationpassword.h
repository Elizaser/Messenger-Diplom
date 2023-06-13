#ifndef CONFIRMATIONPASSWORD_H
#define CONFIRMATIONPASSWORD_H

#include <QDialog>

namespace Ui {
class confirmationPassword;
}

class ConfirmationPassword : public QDialog
{
    Q_OBJECT
signals:
    void confirmationPasswordIsOk(QString password);
public:
    explicit ConfirmationPassword(QWidget *parent = nullptr);
    ~ConfirmationPassword();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_ok_clicked();

private:
    Ui::confirmationPassword *ui;
};

#endif // CONFIRMATIONPASSWORD_H
