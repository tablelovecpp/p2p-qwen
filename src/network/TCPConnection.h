#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QUuid>

class TCPConnection : public QObject {
    Q_OBJECT

public:
    enum ConnectionType {
        Incoming,
        Outgoing
    };

    explicit TCPConnection(QObject* parent = nullptr);
    ~TCPConnection();

    // 作为客户端连接
    bool connectToHost(const QString& host, quint16 port);

    // 作为服务器接受连接
    void setSocket(QTcpSocket* socket);

    // 发送数据
    bool sendData(const QByteArray& data);

    // 断开连接
    void disconnect();

    // 获取对端信息
    QString getPeerAddress() const;
    quint16 getPeerPort() const;
    QUuid getConnectionId() const { return m_connectionId; }

    // 连接状态
    bool isConnected() const;
    ConnectionType getType() const { return m_type; }

signals:
    void dataReceived(const QByteArray& data);
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onKeepAlive();

private:
    void setupSocket();
    void sendKeepAlive();

    QTcpSocket* m_socket;
    ConnectionType m_type;
    QUuid m_connectionId;
    QTimer* m_keepAliveTimer;
    bool m_owned;  // 是否拥有socket所有权
};

#endif // TCPCONNECTION_H