#ifndef DATATRANSFER_H
#define DATATRANSFER_H

#include <QObject>
#include <QThreadPool>
#include <QUuid>
#include <QMutex>
#include <QHash>
#include "TCPConnection.h"
#include "../core/ChunkManager.h"
#include "../core/FileHandler.h"
#include "../core/Encryption.h"

struct TransferInfo {
    QUuid transferId;
    QUuid fileId;
    QString fileName;
    qint64 fileSize;
    QString remoteAddress;
    quint16 remotePort;
    double progress;
    bool isSending;
    bool isCompleted;
    QString status;

    TransferInfo() : fileSize(0), progress(0), isSending(false), isCompleted(false) {}
};

class DataTransfer : public QObject {
    Q_OBJECT

public:
    explicit DataTransfer(QObject* parent = nullptr);
    ~DataTransfer();

    // 发送文件
    QUuid sendFile(const QString& filePath, const QString& remoteHost, quint16 remotePort);

    // 接收文件准备
    void prepareReceive(const QUuid& fileId, const QString& fileName, qint64 fileSize);

    // 取消传输
    void cancelTransfer(const QUuid& transferId);

    // 获取传输信息
    TransferInfo getTransferInfo(const QUuid& transferId) const;

    // 获取所有传输
    QList<TransferInfo> getAllTransfers() const;

    // 设置加密
    void setEncryption(const QString& key);

signals:
    void transferStarted(const QUuid& transferId);
    void transferProgress(const QUuid& transferId, double progress);
    void transferCompleted(const QUuid& transferId, const QString& filePath);
    void transferFailed(const QUuid& transferId, const QString& error);
    void transferCancelled(const QUuid& transferId);

private slots:
    void onConnectionDataReceived(const QByteArray& data);
    void onConnectionConnected();
    void onConnectionDisconnected();
    void onConnectionError(const QString& error);

private:
    // [FIX] 完整的 ActiveTransfer 结构体定义
    struct ActiveTransfer {
        TransferInfo info;          // 包含 transferId, fileId, fileName 等所有字段
        TCPConnection* connection;
        int currentChunk;
        int totalChunks;
        QVector<QByteArray> chunks;
        QString savePath;
        FileHandler fileHandler;
        Encryption encryption;

        ActiveTransfer() : connection(nullptr), currentChunk(0), totalChunks(0) {}
    };

    void startSendTransfer(ActiveTransfer* transfer);
    void processReceivedChunk(ActiveTransfer* transfer, const QByteArray& data);
    void finalizeReceive(ActiveTransfer* transfer);

    QHash<QUuid, ActiveTransfer*> m_activeTransfers;
    ChunkManager m_chunkManager;    // 作为成员变量持有
    QThreadPool m_threadPool;
    mutable QMutex m_mutex;
};

#endif // DATATRANSFER_H