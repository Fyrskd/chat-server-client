#include "mainwindow.h"
#include "LoginDialog.h"

#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <QFile>

#define IP
#define PORT

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));

    Client client;
    client.connectToServer(IP, PORT);
    LoginDialog login(&client);
    if(login.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow w(&client, login.getUserName());
    w.setWindowIcon(QIcon(":/icon.ico"));
    w.resize(600, 400);
    w.show();
    return a.exec();
}
