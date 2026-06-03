#include "TCPConnection.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"
#include <QDataStream>

TCPConnection::TCPConnection(QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_type(Outgoing)
    , m_connectionId(QUuid::createUuid())
    , m_keepAliveTimer(new QTimer(this))
    , m_owned(true)
{
    m_keepAliveTimer->setInterval(Config::instance().getKeepAliveInterval() * 1000);
    connect(m_keepAliveTimer, &QTimer::timeout, this, &TCPConnection::onKeepAlive);
}

TCPConnection::~TCPConnection()
{
    disconnect();
}

bool TCPConnection::connectToHost(const QString& host, quint16 port)
{
    if (m_socket) {
        delete m_socket;
    }

    m_socket = new QTcpSocket(this);
    m_type = Outgoing;
    m_owned = true;

    setupSocket();

    LOG_INFO(QString("Connecting to %1:%2").arg(host).arg(port));
    m_socket->connectToHost(host, port);

    return true;
}

void TCPConnection::setSocket(QTcpSocket* socket)
{
    if (m_socket && m_owned) {
        delete m_socket;
    }

    m_socket = socket;
    m_type = Incoming;
    m_owned = false;

    setupSocket();

    LOG_INFO(QString("Accepted incoming connection from %1:%2")
                 .arg(m_socket->peerAddress().toString())
                 .arg(m_socket->peerPort()));

    emit connected();
    m_keepAliveTimer->start();
}

void TCPConnection::setupSocket()
{
    if (!m_socket) return;

    // 优化TCP性能
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    // 设置读写缓冲区
    int bufferSize = Config::instance().getBufferSize();
    m_socket->setReadBufferSize(bufferSize);

    connect(m_socket, &QTcpSocket::readyRead, this, &TCPConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &TCPConnection::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TCPConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        onError(error);
    });
}

bool TCPConnection::sendData(const QByteArray& data)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        LOG_ERROR("Cannot send data: not connected");
        return false;
    }

    // 添加数据长度前缀（4字节）
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream << static_cast<quint32>(data.size());
    packet.append(data);

    qint64 written = m_socket->write(packet);
    if (written == -1) {
        LOG_ERROR("Failed to send data");
        return false;
    }

    LOG_DEBUG(QString("Sent %1 bytes").arg(written));
    return true;
}

void TCPConnection::disconnect()
{
    if (m_socket) {
        m_keepAliveTimer->stop();

        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->disconnectFromHost();
            if (m_socket->state() != QAbstractSocket::UnconnectedState) {
                m_socket->waitForDisconnected(1000);
            }
        }

        if (m_owned) {
            delete m_socket;
            m_socket = nullptr;
        }

        LOG_INFO("Connection disconnected");
    }
}

QString TCPConnection::getPeerAddress() const
{
    if (m_socket) {
        return m_socket->peerAddress().toString();
    }
    return QString();
}

quint16 TCPConnection::getPeerPort() const
{
    if (m_socket) {
        return m_socket->peerPort();
    }
    return 0;
}

bool TCPConnection::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void TCPConnection::onReadyRead()
{
    if (!m_socket) return;

    while (m_socket->bytesAvailable() >= 4) {
        // 读取数据长度
        QByteArray lengthBytes = m_socket->peek(4);
        QDataStream lengthStream(lengthBytes);
        quint32 dataLength;
        lengthStream >> dataLength;

        // 检查是否有足够的数据
        if (m_socket->bytesAvailable() < static_cast<qint64>(4 + dataLength)) {
            break;
        }

        // 跳过长度字段，读取实际数据
        m_socket->read(4);
        QByteArray data = m_socket->read(dataLength);

        if (!data.isEmpty()) {
            emit dataReceived(data);
        }
    }
}

void TCPConnection::onConnected()
{
    LOG_INFO(QString("Connected to %1:%2").arg(getPeerAddress()).arg(getPeerPort()));
    emit connected();
    m_keepAliveTimer->start();
}

void TCPConnection::onDisconnected()
{
    LOG_INFO("Connection closed by peer");
    m_keepAliveTimer->stop();
    emit disconnected();
}

void TCPConnection::onError(QAbstractSocket::SocketError error)
{
    QString errorStr = m_socket ? m_socket->errorString() : "Unknown error";
    LOG_ERROR(QString("Socket error: %1 (code: %2)").arg(errorStr).arg(error));
    emit errorOccurred(errorStr);
}

void TCPConnection::onKeepAlive()
{
    sendKeepAlive();
}

void TCPConnection::sendKeepAlive()
{
    if (isConnected()) {
        QByteArray keepAliveData = QByteArray::fromHex("KEEPALIVE");
        sendData(keepAliveData);
        LOG_DEBUG("Sent keep-alive");
    }
}