#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QString>
#include <QStandardItem>
#include <QSslSocket>
#include "registrationwindow.h"
#include "mainwindow.h"
#include "dataparsing.h"

namespace Ui {
class EntryWindow;
}

class EntryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EntryWindow(QSslSocket *socket, RegistrationWindow *rWindow, MainWindow* mWindow, QWidget *parent = nullptr);
    ~EntryWindow();

private:
    Ui::EntryWindow *ui;
    RegistrationWindow *rWindow;
    MainWindow *mWindow;
    QSslSocket* socket;
private slots:
    void on_pushButton_register_clicked();
    void on_pushButton_signin_clicked();

public slots:
    void sockReady(DataParsing messageFromServer);
};

#endif // ENTRYWINDOW_H
