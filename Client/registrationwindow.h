#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QSslSocket>
#include "dataparsing.h"

namespace Ui {
class RegistrationWindow;
}

class RegistrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationWindow(QSslSocket *socket, QWidget *parent = nullptr);
    ~RegistrationWindow();
signals:
    void openWindow();

private slots:
    void on_pushButton_reg_clicked();

    void on_pushButton_exit_clicked();

public slots:
    void sockReady(DataParsing messageFromServer);

private:
    Ui::RegistrationWindow *ui;
    QSslSocket* socket;
};

#endif // REGISTRATIONWINDOW_H
