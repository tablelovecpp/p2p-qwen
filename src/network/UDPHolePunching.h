#ifndef UDPHOLEPUNCHING_H
#define UDPHOLEPUNCHING_H

#include <QUdpSocket>
#include <QHostAddress>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QDateTime>

struct PeerInfo {
    QHostAddress address;
    quint16 port;
    QDateTime lastSeen;
    bool isPublic;
};

class UDPHolePunching : public QObject {
    Q_OBJECT

public:
    explicit UDPHolePunching(QObject* parent = nullptr);
    ~UDPHolePunching();

    // 启动UDP监听
    bool start(quint16 port);

    // 停止监听
    void stop();

    // 注册对等节点
    void registerPeer(const QString& peerId, const QHostAddress& address, quint16 port);

    // 移除对等节点
    void removePeer(const QString& peerId);

    // 发送打洞消息
    bool sendHolePunch(const QString& peerId, const QByteArray& data);

    // 获取对等节点信息
    PeerInfo getPeerInfo(const QString& peerId) const;

    // 检查是否正在运行
    bool isRunning() const { return m_running; }

signals:
    void holePunchReceived(const QString& peerId, const QHostAddress& address,
                           quint16 port, const QByteArray& data);
    void peerDiscovered(const QString& peerId, const QHostAddress& address, quint16 port);

private slots:
    void onReadyRead();
    void onCleanupTimer();

private:
    void processIncomingData(const QByteArray& data, const QHostAddress& sender, quint16 port);

    QUdpSocket* m_udpSocket;
    QHash<QString, PeerInfo> m_peers;
    QTimer* m_cleanupTimer;
    bool m_running;
    quint16 m_port;
};

#endif // UDPHOLEPUNCHING_H