#include "controller.h"
#include "ui_controller.h"

Controller::Controller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
}

Controller::~Controller()
{
    delete ui;
}
void Controller:: start(QString hostName)
{    
    socket = new QSslSocket();
    socket->connectToHost(hostName, 5555);

    while(!socket->waitForConnected(1000)) {
        this->show();
        QCoreApplication:: processEvents();
    }
    connect(socket, SIGNAL(readyRead()), SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), SLOT(sockDisc()));

    rWindow = new RegistrationWindow(socket);
    mWindow = new MainWindow(socket);
    eWindow = new EntryWindow(socket, rWindow, mWindow);
    connect(this, &Controller::proccesEntry, eWindow, &EntryWindow::sockReady);
    connect(this, &Controller::proccesRegistration, rWindow, &RegistrationWindow::sockReady);
    connect(this, &Controller::proccesMain, mWindow, &MainWindow::sockReady);

//    connect(this, &Controller::socketWrite, eWindow, &EntryWindow::socketWrite);
//    connect(this, &Controller::socketWrite, rWindow, &RegistrationWindow::socketWrite);
//    connect(this, &Controller::socketWrite, mWindow, &MainWindow::socketWrite);

    eWindow->show();
}
void Controller:: sockDisc()
{
    socket->deleteLater();
}
void Controller:: sockReady(){
    try {
        QByteArray msocket = socket->readAll();
        qDebug() << "msocket -  " << msocket;
//        QMessageBox::information(this, "Информация(ConnectionToServer)", "msocket\n = " + msocket);
        DataParsing messageFromServer(msocket);
        QString nameProccess = messageFromServer.getProccess();
        if(nameProccess == "entry") {
            emit proccesEntry(messageFromServer);
        }
        else if(nameProccess == "registration") {
            emit proccesRegistration(messageFromServer);
        }
        else if(nameProccess == "main") {
            emit proccesMain(messageFromServer);
        } else {
            qDebug() << "Информация(ConnectionToServer)\n" << "Ошибка с форматом передачи данных:" + nameProccess;
        }
    } catch (QString error_message){
        qDebug() << "Информация(ConnectionToServer)\n" << "Ошибка с jsonDocument " + error_message;
    }
}
void Controller::on_pushButton_again_clicked()
{
    this->close();
    start("127.0.0.1");
}

void Controller::on_pushButton_quit_clicked()
{
//    this->close();
//    qApp->exit();
    exit(0);
}
