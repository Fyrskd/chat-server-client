#include "mainwindow.h"
#include "chatbubble.h"
#include <QApplication>

MainWindow::MainWindow(Client *_client, const QString Name, QWidget *parent) : client(_client), QMainWindow(parent) {
    SetupUI();
    Init();
    AddMessage("[system] : Welcome " + Name, false);
    inputEdit->installEventFilter(this);
}

void MainWindow::setClient(Client *client) {
    MainWindow::client = client;
}

void MainWindow::SetupUI() {
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    messageList = new QListWidget;
    messageList->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    messageList->setStyleSheet("QlistWidget { background : #F0F0F0; }");
    inputEdit = new QTextEdit;
    inputEdit->setMaximumHeight(80);
    sendButton = new QPushButton("Send");
    sendButton->setFixedSize(80, 80);
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputEdit);
    inputLayout->addWidget(sendButton);

    onlineLabel = new QLabel("🟢 在线人数：1");
    client->askOnlineCount();
    mainLayout->addWidget(onlineLabel);
    mainLayout->addWidget(messageList);
    mainLayout->addLayout(inputLayout);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);
}

void MainWindow::Init() {
    connect(client, &Client::newMessageReceived, this, &MainWindow::MessageReceived);
    connect(client, &Client::connectionError, this, &MainWindow::ConnectionError);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::SendMessage);
    connect(inputEdit, &QTextEdit::textChanged, this, [this] {
        sendButton->setEnabled(! inputEdit->toPlainText().isEmpty());
    });
}

void MainWindow::SendMessage() {
    QString text = inputEdit->toPlainText().trimmed();
    if(text.isEmpty()) return;
    client->sendMessage("?" + text);
    AddMessage(text, true);
    inputEdit->clear();
}

void MainWindow::updateClientCount() {
    onlineLabel->setText(QString("🟢 在线人数：%1").arg(clientCount));
}

void MainWindow::MessageReceived(const QString &msg) {
    if(msg.size() > 0 && msg[0] == '!') {
        bool ok;
        clientCount = (msg.mid(1)).toInt(&ok);
        if(ok) {
            updateClientCount();
        }
        return;
    }
    AddMessage(msg, false);
}

void MainWindow::ConnectionError(const QString &error) {
    QMessageBox::critical(this, "连接错误", error);
}

void MainWindow::AddMessage(const QString &text, bool isOutgoing) {
    QListWidgetItem *item = new QListWidgetItem(messageList);
    ChatBubble *bubble = new ChatBubble(text, isOutgoing);

    bubble->adjustSize();
    item->setSizeHint(bubble->sizeHint());

    messageList->addItem(item);
    messageList->setItemWidget(item, bubble);
    messageList->scrollToBottom();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == inputEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (!(keyEvent->modifiers() & Qt::ControlModifier)) {
                SendMessage();        
                return true;          
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

MainWindow::~MainWindow() {}
