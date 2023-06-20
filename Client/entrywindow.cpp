#include "entrywindow.h"
#include "ui_entrywindow.h"
#include <QTimer>

EntryWindow::EntryWindow(QSslSocket* socket, RegistrationWindow *rWindow, MainWindow* mWindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntryWindow)
{
    ui->setupUi(this);

    ui->label_error->clear();
    ui->lineEdit_login->clear();
    ui->lineEdit_password->clear();

    this->socket = socket;

    this->rWindow = rWindow;
    this->mWindow = mWindow;
    connect( this->rWindow, &RegistrationWindow::openWindow, this, &EntryWindow::show);
    connect(mWindow, &MainWindow::openWindow, this, &EntryWindow::show);
}

EntryWindow::~EntryWindow()
{
    delete ui;
}

void EntryWindow::on_pushButton_register_clicked()
{
    rWindow->show();
    this->close();
}
void EntryWindow::on_pushButton_signin_clicked()
{
    QString login = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();

    if(login == "" || password == "") {
        ui->label_error->setText("Заполните пустые поля");
    } else {
         if(socket->isOpen())
        socket->write("{\"process\":\"entry\", \"signal\":\"getStatus\", \"login\":\"" + login.toLocal8Bit() + "\", \"password\":\"" + password.toLocal8Bit() + "\"}");
    }
}

void EntryWindow::sockReady(DataParsing messageFromServer)
{
    try {
        QString statusEntry = messageFromServer.getSignal();
        if(statusEntry == "welcome") {
            if(socket->isOpen()) {
                QString login = ui->lineEdit_login->text();
                QString password = ui->lineEdit_password->text();
                socket->write("{\"process\":\"entry\", \"signal\":\"setUserInfo\", \"login\":\"" + login.toLocal8Bit() + "\", \"password\":\"" + password.toLocal8Bit() + "\"}");
                mWindow->show();
                this->close();

                ui->label_error->clear();
                ui->lineEdit_login->clear();
                ui->lineEdit_password->clear();
            }
        } else if (statusEntry == "error") {
            ui->label_error->setText("Неверный логин или пароль");
        } else {
            qDebug() << "Информация(EntryWindow)\n" <<  "Ошибка с форматом передачи данных";
        }
    } catch (QString error_message){
        qDebug() << "Информация(EntryWindow)\n" <<  "Ошибка с jsonDocument " + error_message;
    }
}
