#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QMessageBox>
#include <QSslSocket>
#include <QString>
#include "entrywindow.h"
#include "registrationwindow.h"
#include "mainwindow.h"
#include "dataparsing.h"

namespace Ui {
class Controller;
}

class Controller : public QWidget
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = nullptr);
    ~Controller();
    QSslSocket* socket;
    void start(QString hostName);

signals:
        void proccesRegistration(DataParsing messageFromServer);
        void proccesEntry(DataParsing messageFromServer);
        void proccesMain(DataParsing messageFromServer);

private:
    Ui::Controller *ui;

    EntryWindow *eWindow;
    RegistrationWindow *rWindow;
    MainWindow *mWindow;

private slots:
    void sockReady();
    void sockDisc();
    void on_pushButton_again_clicked();
    void on_pushButton_quit_clicked();
};

#endif // CONTROLLER_H
