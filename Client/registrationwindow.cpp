#include "registrationwindow.h"
#include "ui_registrationwindow.h"

RegistrationWindow::RegistrationWindow(QTcpSocket* socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationWindow)
{
    ui->setupUi(this);
    this->socket = socket;
}

RegistrationWindow::~RegistrationWindow()
{
    delete ui;
}

void RegistrationWindow::on_pushButton_reg_clicked()
{
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();
    QString name = ui->lineEdit_name->text();

    if(login == "" || password == "" || password == "" || name == "") {
        ui->label_error->setText("Заполните пустые поля");
    } else if(login.size() > 20){
        ui->label_errorLogin->setText("Длина логина не должен превышать 20 символов");
    } else if(password.size() > 20){
        ui->label_errorLogin->setText("Длина пароля не должен превышать 20 символов");
    } else if(name.size() > 20){
        ui->label_errorLogin->setText("Длина имени не должен превышать 20 символов");
    } else {
        socket->write("{\"process\":\"registration\", \"login\":\"" + login.toLocal8Bit() + "\", \"password\":\"" + password.toLocal8Bit() + "\", \"name\":\"" + name.toLocal8Bit()+ "\"}");
    }
}

void RegistrationWindow::sockReady(DataParsing messageFromServer)
{
    try {
        QString statusRegisrtation = messageFromServer.getSignal();

        if(statusRegisrtation == "welcome") {
            QMessageBox::information(this, "Информация(RegWindow)", "Вы успешно зарегистрировались!");

            ui->label_error->clear();
            ui->label_errorLogin->clear();
            ui->label_errorPassword->clear();
            ui->lineEdit_login->clear();
            ui->lineEdit_password->clear();
            ui->lineEdit_name->clear();

            this->close();
            emit openWindow();

        } else if (statusRegisrtation == "errorLogin") {
            ui->label_error->setText("Такой логин уже существует");
        } else {
            qDebug() << "Информация(RegWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(RegWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}

void RegistrationWindow::on_pushButton_exit_clicked()
{
    ui->label_error->clear();
    ui->label_errorLogin->clear();
    ui->label_errorPassword->clear();
    ui->lineEdit_login->clear();
    ui->lineEdit_password->clear();
    ui->lineEdit_name->clear();

    this->close();
    emit openWindow();
}
