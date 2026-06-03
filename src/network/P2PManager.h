#ifndef P2PMANAGER_H
#define P2PMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QList>
#include "TCPConnection.h"
#include "UDPHolePunching.h"
#include "DataTransfer.h"

struct PeerNode {
    QString peerId;
    QString address;
    quint16 tcpPort;
    quint16 udpPort;
    QDateTime lastSeen;
    bool isOnline;
};

class P2PManager : public QObject {
    Q_OBJECT

public:
    explicit P2PManager(QObject* parent = nullptr);
    ~P2PManager();

    // 启动P2P服务
    bool start(quint16 tcpPort, quint16 udpPort);

    // 停止服务
    void stop();

    // 连接到对等节点
    bool connectToPeer(const QString& address, quint16 tcpPort);

    // 断开与对等节点的连接
    void disconnectFromPeer(const QString& peerId);

    // 发送文件
    QUuid sendFile(const QString& filePath, const QString& peerId);

    // 获取在线对等节点列表
    QList<PeerNode> getOnlinePeers() const;

    // 获取本地ID
    QString getLocalPeerId() const { return m_localPeerId; }

    // 检查是否正在运行
    bool isRunning() const { return m_running; }

signals:
    void peerConnected(const QString& peerId, const QString& address);
    void peerDisconnected(const QString& peerId);
    void fileReceived(const QString& filePath, const QString& fromPeer);
    void transferProgress(const QUuid& transferId, double progress);
    void transferCompleted(const QUuid& transferId, const QString& filePath);
    void transferFailed(const QUuid& transferId, const QString& error);

private slots:
    void onNewConnection();
    void onPeerDataReceived(const QByteArray& data);
    void onPeerConnected();
    void onPeerDisconnected();
    void onPeerError(const QString& error);
    void onHolePunchReceived(const QString& peerId, const QHostAddress& address,
                             quint16 port, const QByteArray& data);
    void onDataTransferProgress(const QUuid& transferId, double progress);
    void onDataTransferCompleted(const QUuid& transferId, const QString& filePath);
    void onDataTransferFailed(const QUuid& transferId, const QString& error);

private:
    void handleIncomingMessage(TCPConnection* connection, const QByteArray& data);
    void broadcastPeerList();
    void updatePeerStatus(const QString& peerId, bool online);

    QTcpServer* m_tcpServer;
    UDPHolePunching* m_udpHolePunching;
    DataTransfer* m_dataTransfer;

    QHash<QString, TCPConnection*> m_peerConnections;
    QHash<QString, PeerNode> m_peerNodes;

    QString m_localPeerId;
    quint16 m_tcpPort;
    quint16 m_udpPort;
    bool m_running;
};

#endif // P2PMANAGER_H