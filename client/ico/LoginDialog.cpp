#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(Client *client, QWidget *parent) : client(client), QDialog(parent) {
    setWindowTitle("Login");

    QLabel *userLabel = new QLabel("UserName:");
    userEdit = new QLineEdit;
    loginBtn = new QPushButton("Login");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(userEdit);
    mainLayout->addWidget(loginBtn);

    setLayout(mainLayout);
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(client, &Client::loginResult, this, [this](bool success) {
        if(success) {
            accept();
        } else {
            QMessageBox::warning(this, "Login Failed", "Username has been used!");
        }
    });
}

void LoginDialog::onLoginClicked() {
    QString user = userEdit->text();
    if(user.isEmpty()) {
        return;
    }
    if(user == "system") {
        QMessageBox::warning(this, "Login Failed", "Cannot use system as username!");
        return;
    }
    name = user;
    client->sendMessage("!" + user);
}

QString LoginDialog::getUserName() const {
    return name;
}

void LoginDialog::closeEvent(QCloseEvent *event) {
    reject();
}
