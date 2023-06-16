#include "registrationwindow.h"
#include "ui_registrationwindow.h"

RegistrationWindow::RegistrationWindow(QSslSocket* socket, QWidget *parent) :
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
    ui->label_errorName->clear();
    ui->label_errorLogin->clear();
    ui->label_errorPassword->clear();
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();
    QString name = ui->lineEdit_name->text();

    if(login == "" || password == "" || password == "" || name == "") {
        ui->label_errorPassword->setText("Заполните пустые поля");
    } else if(login.size() > 20){
        ui->label_errorLogin->setText("Длина логина не должен превышать 20 символов");
    } else if(password.size() > 20){
        ui->label_errorLogin->setText("Длина пароля не должен превышать 20 символов");
    } else if(name.size() > 20){
        ui->label_errorName->setText("Длина имени не должен превышать 20 символов");
    } else {
        socket->write("{\"process\":\"registration\", \"login\":\"" + login.toLocal8Bit() + "\", \"password\":\"" + password.toLocal8Bit() + "\", \"name\":\"" + name.toLocal8Bit()+ "\"}");
    }
}

void RegistrationWindow::sockReady(DataParsing messageFromServer)
{
    try {
        QString statusRegisrtation = messageFromServer.getSignal();

        if(statusRegisrtation == "welcome") {
            QMessageBox msgBox;
            msgBox.setText("Внимание");
            msgBox.setInformativeText("Вы успешно зарегистрировались!");
            msgBox.setIcon(QMessageBox::NoIcon);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setStyleSheet("background-color: #3A6B35;\n font: 75 10pt \"Ubuntu Mono\";\n color-font: #FFFF;");
            msgBox.exec();
            msgBox.show();

            ui->label_errorName->clear();
            ui->label_errorLogin->clear();
            ui->label_errorPassword->clear();
            ui->lineEdit_login->clear();
            ui->lineEdit_password->clear();
            ui->lineEdit_name->clear();

            this->close();
            emit openWindow();

        } else if (statusRegisrtation == "errorLogin") {
            ui->label_errorLogin->setText("Такой логин уже существует");
        } else {
            qDebug() << "Информация(RegWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(RegWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}

void RegistrationWindow::on_pushButton_exit_clicked()
{
    ui->label_errorLogin->clear();
    ui->label_errorPassword->clear();
    ui->label_errorName->clear();
    ui->lineEdit_login->clear();
    ui->lineEdit_password->clear();
    ui->lineEdit_name->clear();

    this->close();
    emit openWindow();
}
