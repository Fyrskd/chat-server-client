#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "client.h"
#include <QDialog>

class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(Client *client, QWidget *parent = nullptr);
    QString getUserName() const;
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoginClicked();

private:
    QLineEdit *userEdit;
    QPushButton *loginBtn;
    Client *client;
    QString name;
};

#endif // LOGINDIALOG_H
