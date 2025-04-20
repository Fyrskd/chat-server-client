#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void connectToServer(const QString &host, quint16 port);
    void sendMessage(const QString &message);
    void askOnlineCount();
signals:
    void newMessageReceived(const QString &message);
    void connectionError(const QString &error);
    void loginResult(bool success);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket *m_socket;
    QDataStream m_stream;
};

#endif // CLIENT_H
