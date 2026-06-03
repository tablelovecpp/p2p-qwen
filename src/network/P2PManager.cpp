#include "P2PManager.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostInfo>

P2PManager::P2PManager(QObject* parent)
    : QObject(parent)
    , m_tcpServer(new QTcpServer(this))
    , m_udpHolePunching(new UDPHolePunching(this))
    , m_dataTransfer(new DataTransfer(this))
    , m_localPeerId(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_tcpPort(0)
    , m_udpPort(0)
    , m_running(false)
{
    connect(m_tcpServer, &QTcpServer::newConnection, this, &P2PManager::onNewConnection);

    connect(m_udpHolePunching, &UDPHolePunching::holePunchReceived,
            this, &P2PManager::onHolePunchReceived);

    connect(m_dataTransfer, &DataTransfer::transferProgress,
            this, &P2PManager::onDataTransferProgress);
    connect(m_dataTransfer, &DataTransfer::transferCompleted,
            this, &P2PManager::onDataTransferCompleted);
    connect(m_dataTransfer, &DataTransfer::transferFailed,
            this, &P2PManager::onDataTransferFailed);

    LOG_INFO(QString("P2PManager created with ID: %1").arg(m_localPeerId));
}

P2PManager::~P2PManager()
{
    stop();
}

bool P2PManager::start(quint16 tcpPort, quint16 udpPort)
{
    if (m_running) {
        LOG_WARNING("P2P service already running");
        return true;
    }

    m_tcpPort = tcpPort;
    m_udpPort = udpPort;

    // 启动TCP服务器
    if (!m_tcpServer->listen(QHostAddress::Any, tcpPort)) {
        LOG_ERROR(QString("Failed to start TCP server on port %1").arg(tcpPort));
        return false;
    }

    // 启动UDP打洞
    if (!m_udpHolePunching->start(udpPort)) {
        LOG_ERROR(QString("Failed to start UDP hole punching on port %1").arg(udpPort));
        m_tcpServer->close();
        return false;
    }

    m_running = true;

    LOG_INFO(QString("P2P service started - TCP: %1, UDP: %2, ID: %3")
                 .arg(tcpPort).arg(udpPort).arg(m_localPeerId));

    return true;
}

void P2PManager::stop()
{
    if (!m_running) return;

    // 断开所有对等连接
    for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it) {
        it.value()->disconnect();
        delete it.value();
    }
    m_peerConnections.clear();
    m_peerNodes.clear();

    m_tcpServer->close();
    m_udpHolePunching->stop();

    m_running = false;

    LOG_INFO("P2P service stopped");
}

bool P2PManager::connectToPeer(const QString& address, quint16 tcpPort)
{
    if (!m_running) {
        LOG_ERROR("P2P service not running");
        return false;
    }

    TCPConnection* connection = new TCPConnection(this);

    connect(connection, &TCPConnection::dataReceived,
            this, &P2PManager::onPeerDataReceived);
    connect(connection, &TCPConnection::connected,
            this, &P2PManager::onPeerConnected);
    connect(connection, &TCPConnection::disconnected,
            this, &P2PManager::onPeerDisconnected);
    connect(connection, &TCPConnection::errorOccurred,
            this, &P2PManager::onPeerError);

    if (!connection->connectToHost(address, tcpPort)) {
        LOG_ERROR(QString("Failed to connect to %1:%2").arg(address).arg(tcpPort));
        delete connection;
        return false;
    }

    // 暂时存储，等待连接成功后再添加到映射
    connection->setProperty("pendingAddress", address);
    connection->setProperty("pendingPort", tcpPort);

    return true;
}

void P2PManager::disconnectFromPeer(const QString& peerId)
{
    if (m_peerConnections.contains(peerId)) {
        TCPConnection* connection = m_peerConnections[peerId];
        connection->disconnect();
        delete connection;
        m_peerConnections.remove(peerId);

        updatePeerStatus(peerId, false);

        LOG_INFO(QString("Disconnected from peer: %1").arg(peerId));
    }
}

QUuid P2PManager::sendFile(const QString& filePath, const QString& peerId)
{
    if (!m_peerConnections.contains(peerId)) {
        LOG_ERROR(QString("Peer not connected: %1").arg(peerId));
        return QUuid();
    }

    TCPConnection* connection = m_peerConnections[peerId];
    QString address = connection->getPeerAddress();
    quint16 port = connection->getPeerPort();

    QUuid transferId = m_dataTransfer->sendFile(filePath, address, port);

    if (!transferId.isNull()) {
        LOG_INFO(QString("Started file transfer to peer %1: %2").arg(peerId).arg(filePath));
    }

    return transferId;
}

QList<PeerNode> P2PManager::getOnlinePeers() const
{
    QList<PeerNode> peers;

    for (auto it = m_peerNodes.begin(); it != m_peerNodes.end(); ++it) {
        if (it->isOnline) {
            peers.append(it.value());
        }
    }

    return peers;
}

void P2PManager::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket* socket = m_tcpServer->nextPendingConnection();

        if (socket) {
            TCPConnection* connection = new TCPConnection(this);
            connection->setSocket(socket);

            connect(connection, &TCPConnection::dataReceived,
                    this, &P2PManager::onPeerDataReceived);
            connect(connection, &TCPConnection::disconnected,
                    this, &P2PManager::onPeerDisconnected);
            connect(connection, &TCPConnection::errorOccurred,
                    this, &P2PManager::onPeerError);

            // 等待握手消息获取peerId
            connection->setProperty("isIncoming", true);

            LOG_INFO("Accepted new incoming connection");
        }
    }
}

void P2PManager::handleIncomingMessage(TCPConnection* connection, const QByteArray& data)
{
    if (data.isEmpty()) return;

    // 解析JSON消息
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        LOG_WARNING("Invalid JSON message received");
        return;
    }

    QJsonObject json = doc.object();
    QString messageType = json["type"].toString();

    if (messageType == "hello") {
        // 握手消息
        QString peerId = json["peerId"].toString();
        QString address = json["address"].toString();
        quint16 tcpPort = json["tcpPort"].toInt();
        quint16 udpPort = json["udpPort"].toInt();

        // 注册对等节点
        m_peerConnections[peerId] = connection;

        PeerNode node;
        node.peerId = peerId;
        node.address = address;
        node.tcpPort = tcpPort;
        node.udpPort = udpPort;
        node.lastSeen = QDateTime::currentDateTime();
        node.isOnline = true;

        m_peerNodes[peerId] = node;

        // 注册到UDP打洞
        m_udpHolePunching->registerPeer(peerId, QHostAddress(address), udpPort);

        // 回复握手
        QJsonObject response;
        response["type"] = "hello_response";
        response["peerId"] = m_localPeerId;
        response["address"] = QHostInfo::localHostName();
        response["tcpPort"] = m_tcpPort;
        response["udpPort"] = m_udpPort;

        QJsonDocument respDoc(response);
        connection->sendData(respDoc.toJson(QJsonDocument::Compact));

        emit peerConnected(peerId, address);

        LOG_INFO(QString("Peer connected: %1 at %2:%3").arg(peerId).arg(address).arg(tcpPort));

    } else if (messageType == "peer_list") {
        // 对等节点列表广播
        QJsonArray peers = json["peers"].toArray();

        for (const QJsonValue& value : peers) {
            QJsonObject peerObj = value.toObject();
            QString peerId = peerObj["peerId"].toString();

            if (peerId != m_localPeerId && !m_peerNodes.contains(peerId)) {
                PeerNode node;
                node.peerId = peerId;
                node.address = peerObj["address"].toString();
                node.tcpPort = peerObj["tcpPort"].toInt();
                node.udpPort = peerObj["udpPort"].toInt();
                node.lastSeen = QDateTime::currentDateTime();
                node.isOnline = true;

                m_peerNodes[peerId] = node;

                LOG_DEBUG(QString("Discovered new peer: %1").arg(peerId));
            }
        }

    } else if (messageType == "file_transfer_request") {
        // 文件传输请求
        QUuid fileId = QUuid::fromString(json["fileId"].toString());
        QString fileName = json["fileName"].toString();
        qint64 fileSize = json["fileSize"].toInteger();

        m_dataTransfer->prepareReceive(fileId, fileName, fileSize);

        LOG_INFO(QString("Received file transfer request: %1 (%2 bytes)").arg(fileName).arg(fileSize));

    } else if (messageType == "chunk_received") {
        // 接收方收到 chunk 后发送确认
        // 发送方收到确认后继续发送下一个 chunk（在 DataTransfer 中处理）
        int chunkIndex = json["chunkIndex"].toInt();
        LOG_DEBUG(QString("Chunk %1 acknowledged").arg(chunkIndex));
    }
}

void P2PManager::broadcastPeerList()
{
    QJsonArray peers;

    for (auto it = m_peerNodes.begin(); it != m_peerNodes.end(); ++it) {
        if (it->isOnline) {
            QJsonObject peerObj;
            peerObj["peerId"] = it->peerId;
            peerObj["address"] = it->address;
            peerObj["tcpPort"] = it->tcpPort;
            peerObj["udpPort"] = it->udpPort;

            peers.append(peerObj);
        }
    }

    QJsonObject message;
    message["type"] = "peer_list";
    message["peers"] = peers;

    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 发送给所有连接的對等節點
    for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it) {
        it.value()->sendData(data);
    }
}

void P2PManager::updatePeerStatus(const QString& peerId, bool online)
{
    if (m_peerNodes.contains(peerId)) {
        m_peerNodes[peerId].isOnline = online;
        m_peerNodes[peerId].lastSeen = QDateTime::currentDateTime();

        if (!online) {
            emit peerDisconnected(peerId);
            LOG_INFO(QString("Peer disconnected: %1").arg(peerId));
        }
    }
}

void P2PManager::onPeerDataReceived(const QByteArray& data)
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    handleIncomingMessage(connection, data);
}

void P2PManager::onPeerConnected()
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    // 发送握手消息
    QJsonObject hello;
    hello["type"] = "hello";
    hello["peerId"] = m_localPeerId;
    hello["address"] = QHostInfo::localHostName();
    hello["tcpPort"] = m_tcpPort;
    hello["udpPort"] = m_udpPort;

    QJsonDocument doc(hello);
    connection->sendData(doc.toJson(QJsonDocument::Compact));

    LOG_INFO("Sent hello message to peer");
}

void P2PManager::onPeerDisconnected()
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    // 查找并移除该连接
    QString peerId;
    for (auto it = m_peerConnections.begin(); it != m_peerConnections.end(); ++it) {
        if (it.value() == connection) {
            peerId = it.key();
            break;
        }
    }

    if (!peerId.isEmpty()) {
        delete connection;
        m_peerConnections.remove(peerId);
        updatePeerStatus(peerId, false);
    }
}

void P2PManager::onPeerError(const QString& error)
{
    LOG_ERROR(QString("Peer connection error: %1").arg(error));
}

void P2PManager::onHolePunchReceived(const QString& peerId, const QHostAddress& address,
                                     quint16 port, const QByteArray& data)
{
    LOG_DEBUG(QString("Hole punch received from %1:%2, peer: %3")
                  .arg(address.toString()).arg(port).arg(peerId));

    // 更新对等节点信息
    if (m_peerNodes.contains(peerId)) {
        m_peerNodes[peerId].address = address.toString();
        m_peerNodes[peerId].udpPort = port;
        m_peerNodes[peerId].lastSeen = QDateTime::currentDateTime();
    }
}

void P2PManager::onDataTransferProgress(const QUuid& transferId, double progress)
{
    emit transferProgress(transferId, progress);
}

void P2PManager::onDataTransferCompleted(const QUuid& transferId, const QString& filePath)
{
    emit transferCompleted(transferId, filePath);
    LOG_INFO(QString("Transfer completed: %1").arg(filePath));
}

void P2PManager::onDataTransferFailed(const QUuid& transferId, const QString& error)
{
    emit transferFailed(transferId, error);
    LOG_ERROR(QString("Transfer failed: %1").arg(error));
}