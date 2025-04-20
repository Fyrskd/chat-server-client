#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qstring.h>
#include <QMessageBox>
#include <QKeyEvent>
#include "client.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(Client *_client, const QString Name, QWidget *parent = nullptr);
    void setClient(Client *client);
    ~MainWindow();
private slots:
    void SendMessage();
    void MessageReceived(const QString &msg);
    void ConnectionError(const QString &error);
    void AddMessage(const QString &text, bool insOutgoing);

private:
    void SetupUI();
    void Init();
    void updateClientCount();

    Client *client;
    QListWidget *messageList;
    QTextEdit *inputEdit;
    QPushButton *sendButton;
    QLabel *onlineLabel;
    int clientCount = 0;
    int loginStatus = 0;
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
#endif // MAINWINDOW_H


