#include "UDPHolePunching.h"
#include "../utils/Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime> // [FIX] Qt6 必须显式包含 QDateTime

// [FIX] Qt6 移除了 QHostAddress::isPrivate()，使用此辅助函数替代
static bool isPrivateAddress(const QHostAddress& addr) {
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        // 检查 RFC1918 私有地址段及回环地址
        return addr.isInSubnet(QHostAddress("10.0.0.0"), 8) ||
               addr.isInSubnet(QHostAddress("172.16.0.0"), 12) ||
               addr.isInSubnet(QHostAddress("192.168.0.0"), 16) ||
               addr.isInSubnet(QHostAddress("127.0.0.0"), 8);
    } else if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        // 检查 IPv6 本地回环、空地址及唯一本地地址(ULA)
        return addr.isLoopback() || addr.isNull() ||
               addr.isInSubnet(QHostAddress("::1"), 128) ||
               addr.isInSubnet(QHostAddress("fc00::"), 7);
    }
    return false;
}

UDPHolePunching::UDPHolePunching(QObject* parent)
    : QObject(parent)
    , m_udpSocket(nullptr)
    , m_cleanupTimer(new QTimer(this))
    , m_running(false)
    , m_port(0)
{
    m_cleanupTimer->setInterval(60000);  // 每分钟清理一次
    connect(m_cleanupTimer, &QTimer::timeout, this, &UDPHolePunching::onCleanupTimer);
}

UDPHolePunching::~UDPHolePunching()
{
    stop();
}

bool UDPHolePunching::start(quint16 port)
{
    if (m_running) {
        LOG_WARNING("UDP Hole Punching already running");
        return true;
    }

    m_udpSocket = new QUdpSocket(this);

    // Qt6 中 ShareAddress 和 ReuseAddressHint 仍然可用
    if (!m_udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        LOG_ERROR(QString("Failed to bind UDP socket to port %1").arg(port));
        delete m_udpSocket;
        m_udpSocket = nullptr;
        return false;
    }

    m_port = port;
    m_running = true;

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UDPHolePunching::onReadyRead);
    m_cleanupTimer->start();

    LOG_INFO(QString("UDP Hole Punching started on port %1").arg(port));
    return true;
}

void UDPHolePunching::stop()
{
    if (m_udpSocket) {
        m_udpSocket->close();
        delete m_udpSocket;
        m_udpSocket = nullptr;
    }

    m_cleanupTimer->stop();
    m_running = false;
    m_peers.clear();

    LOG_INFO("UDP Hole Punching stopped");
}

void UDPHolePunching::registerPeer(const QString& peerId, const QHostAddress& address, quint16 port)
{
    PeerInfo info;
    info.address = address;
    info.port = port;
    info.lastSeen = QDateTime::currentDateTime();

    // [FIX] 使用自定义辅助函数替代已移除的 isPrivate()
    info.isPublic = !isPrivateAddress(address);

    m_peers[peerId] = info;

    LOG_DEBUG(QString("Registered peer: %1 at %2:%3 (Public: %4)")
                  .arg(peerId).arg(address.toString()).arg(port).arg(info.isPublic ? "Yes" : "No"));
}

void UDPHolePunching::removePeer(const QString& peerId)
{
    m_peers.remove(peerId);
    LOG_DEBUG(QString("Removed peer: %1").arg(peerId));
}

bool UDPHolePunching::sendHolePunch(const QString& peerId, const QByteArray& data)
{
    if (!m_udpSocket || !m_running) {
        LOG_ERROR("UDP socket not ready");
        return false;
    }

    if (!m_peers.contains(peerId)) {
        LOG_ERROR(QString("Peer not found: %1").arg(peerId));
        return false;
    }

    const PeerInfo& peer = m_peers[peerId];

    // 构建数据包：类型 + peerId + 数据
    QByteArray packet;
    packet.append(static_cast<char>(0x01));  // 打洞消息类型
    packet.append(peerId.toUtf8());
    packet.append('\0');
    packet.append(data);

    qint64 sent = m_udpSocket->writeDatagram(packet, peer.address, peer.port);

    if (sent == -1) {
        LOG_ERROR("Failed to send hole punch message");
        return false;
    }

    LOG_DEBUG(QString("Sent hole punch to %1:%2, %3 bytes")
                  .arg(peer.address.toString()).arg(peer.port).arg(sent));
    return true;
}

PeerInfo UDPHolePunching::getPeerInfo(const QString& peerId) const
{
    if (m_peers.contains(peerId)) {
        return m_peers[peerId];
    }
    return PeerInfo();
}

void UDPHolePunching::onReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(m_udpSocket->pendingDatagramSize()));

        QHostAddress sender;
        quint16 senderPort;

        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        processIncomingData(datagram, sender, senderPort);
    }
}

void UDPHolePunching::processIncomingData(const QByteArray& data, const QHostAddress& sender, quint16 port)
{
    if (data.isEmpty()) return;

    char messageType = data[0];

    if (messageType == 0x01) {  // 打洞消息
        // 解析peerId
        int nullPos = data.indexOf('\0', 1);
        if (nullPos == -1) return;

        QString peerId = QString::fromUtf8(data.mid(1, nullPos - 1));
        QByteArray payload = data.mid(nullPos + 1);

        // 更新或注册对等节点
        registerPeer(peerId, sender, port);

        LOG_DEBUG(QString("Received hole punch from %1:%2, peer: %3")
                      .arg(sender.toString()).arg(port).arg(peerId));

        emit holePunchReceived(peerId, sender, port, payload);
    }
}

void UDPHolePunching::onCleanupTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    QStringList expiredPeers;

    // 清理超过5分钟未活动的对等节点
    for (auto it = m_peers.begin(); it != m_peers.end(); ++it) {
        if (it->lastSeen.secsTo(now) > 300) {
            expiredPeers.append(it.key());
        }
    }

    for (const QString& peerId : expiredPeers) {
        LOG_DEBUG(QString("Cleaning up expired peer: %1").arg(peerId));
        m_peers.remove(peerId);
    }
}