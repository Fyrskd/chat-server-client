#include "client.h"

Client::Client(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this)), m_stream(m_socket) {

    m_stream.setVersion(QDataStream::Qt_6_0);

    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::onErrorOccurred);
}

void Client::sendMessage(const QString &message) {
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        emit connectionError("Not connected to server");
        return;
    }

    QByteArray data = message.toUtf8();
    quint32 length = data.size();

    m_stream << length;
    m_stream.writeRawData(data.constData(), data.size());
}

void Client::onReadyRead() {
    static quint32 expectedLength = 0;

    while (m_socket->bytesAvailable() > 0) {
        if (expectedLength == 0) {
            if (m_socket->bytesAvailable() < sizeof(quint32)) return;
            m_stream >> expectedLength;
        }

        if (m_socket->bytesAvailable() < expectedLength) return;

        QByteArray data;
        data.resize(expectedLength);
        m_stream.readRawData(data.data(), expectedLength);

        QString msg = QString::fromUtf8(data);
        if(msg.size() > 0 && msg[0] == '?') {
            if(msg.size() > 1 && msg[1] == '1') {
                emit loginResult(true);
            } else {
                emit loginResult(false);
            }
        } else {
            emit newMessageReceived(msg);
        }
        expectedLength = 0;
    }
}

void Client::connectToServer(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port);
    if (!m_socket->waitForConnected(5000)) {
        emit connectionError("Connection timeout");
    }
}

void Client::askOnlineCount() {
    sendMessage("@");
}

void Client::onErrorOccurred(QAbstractSocket::SocketError error) {
    QString errorMsg = m_socket->errorString();
    emit connectionError(errorMsg);
}

Client::~Client() {
    m_socket->close();
    delete m_socket;
}
