#include "DataTransfer.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"
#include <QFile>
#include <QDataStream>
#include <QJsonDocument>   // [FIX] Qt6 必须显式包含
#include <QJsonObject>     // [FIX] Qt6 必须显式包含
#include <QDir>            // [FIX] Qt6 必须显式包含

// [FIX] 使用初始化列表初始化 m_chunkManager，避免拷贝赋值
DataTransfer::DataTransfer(QObject* parent)
    : QObject(parent)
    , m_chunkManager(Config::instance().getChunkSize())  // 正确初始化方式
{
    m_threadPool.setMaxThreadCount(Config::instance().getMaxThreads());
    LOG_INFO("DataTransfer initialized");
}

DataTransfer::~DataTransfer()
{
    QMutexLocker locker(&m_mutex);

    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        if (it.value()->connection) {
            it.value()->connection->disconnect();
            delete it.value()->connection;
        }
        delete it.value();
    }
    m_activeTransfers.clear();
}

QUuid DataTransfer::sendFile(const QString& filePath, const QString& remoteHost, quint16 remotePort)
{
    FileHandler fileHandler;

    if (!fileHandler.fileExists(filePath)) {
        LOG_ERROR(QString("File not found: %1").arg(filePath));
        emit transferFailed(QUuid(), "File not found");
        return QUuid();
    }

    FileInfo fileInfo = fileHandler.getFileInfo(filePath);
    if (fileInfo.fileSize == 0) {
        LOG_ERROR("Empty file");
        emit transferFailed(QUuid(), "Empty file");
        return QUuid();
    }

    QByteArray fileData = fileHandler.readFile(filePath);
    if (fileData.isEmpty()) {
        LOG_ERROR("Failed to read file");
        emit transferFailed(QUuid(), "Failed to read file");
        return QUuid();
    }

    QVector<QByteArray> chunks = m_chunkManager.splitData(fileData);

    // [FIX] 现在 ActiveTransfer 定义完整，可以正常访问所有成员
    ActiveTransfer* transfer = new ActiveTransfer();
    transfer->info.transferId = QUuid::createUuid();
    transfer->info.fileId = fileInfo.id;
    transfer->info.fileName = fileInfo.fileName;
    transfer->info.fileSize = fileInfo.fileSize;
    transfer->info.remoteAddress = remoteHost;
    transfer->info.remotePort = remotePort;
    transfer->info.progress = 0;
    transfer->info.isSending = true;
    transfer->info.isCompleted = false;
    transfer->info.status = "Initializing";
    transfer->currentChunk = 0;
    transfer->totalChunks = chunks.size();
    transfer->chunks = chunks;

    TCPConnection* connection = new TCPConnection();
    transfer->connection = connection;

    connect(connection, &TCPConnection::dataReceived,
            this, &DataTransfer::onConnectionDataReceived);
    connect(connection, &TCPConnection::connected,
            this, &DataTransfer::onConnectionConnected);
    connect(connection, &TCPConnection::disconnected,
            this, &DataTransfer::onConnectionDisconnected);
    connect(connection, &TCPConnection::errorOccurred,
            this, &DataTransfer::onConnectionError);

    {
        QMutexLocker locker(&m_mutex);
        m_activeTransfers[transfer->info.transferId] = transfer;
    }

    LOG_INFO(QString("Starting file transfer: %1 to %2:%3")
                 .arg(filePath).arg(remoteHost).arg(remotePort));

    connection->connectToHost(remoteHost, remotePort);

    emit transferStarted(transfer->info.transferId);
    return transfer->info.transferId;
}

void DataTransfer::prepareReceive(const QUuid& fileId, const QString& fileName, qint64 fileSize)
{
    ActiveTransfer* transfer = new ActiveTransfer();
    transfer->info.transferId = QUuid::createUuid();
    transfer->info.fileId = fileId;
    transfer->info.fileName = fileName;
    transfer->info.fileSize = fileSize;
    transfer->info.progress = 0;
    transfer->info.isSending = false;
    transfer->info.isCompleted = false;
    transfer->info.status = "Waiting for data";
    transfer->currentChunk = 0;
    transfer->totalChunks = 0;
    transfer->connection = nullptr;

    transfer->savePath = transfer->fileHandler.createTempFilePath(fileName);

    {
        QMutexLocker locker(&m_mutex);
        m_activeTransfers[transfer->info.transferId] = transfer;
    }

    LOG_INFO(QString("Prepared to receive file: %1, size: %2 bytes").arg(fileName).arg(fileSize));
}

void DataTransfer::cancelTransfer(const QUuid& transferId)
{
    QMutexLocker locker(&m_mutex);

    if (!m_activeTransfers.contains(transferId)) {
        return;
    }

    ActiveTransfer* transfer = m_activeTransfers[transferId];

    if (transfer->connection) {
        transfer->connection->disconnect();
        delete transfer->connection;
        transfer->connection = nullptr;
    }

    transfer->info.status = "Cancelled";
    emit transferCancelled(transferId);

    m_activeTransfers.remove(transferId);
    delete transfer;

    LOG_INFO(QString("Transfer cancelled: %1").arg(transferId.toString()));
}

TransferInfo DataTransfer::getTransferInfo(const QUuid& transferId) const
{
    QMutexLocker locker(&m_mutex);

    if (m_activeTransfers.contains(transferId)) {
        return m_activeTransfers[transferId]->info;
    }

    return TransferInfo();
}

QList<TransferInfo> DataTransfer::getAllTransfers() const
{
    QMutexLocker locker(&m_mutex);

    QList<TransferInfo> transfers;
    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        transfers.append(it.value()->info);
    }

    return transfers;
}

void DataTransfer::setEncryption(const QString& key)
{
    QMutexLocker locker(&m_mutex);

    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        it.value()->encryption.setEnabled(!key.isEmpty());
    }
}

void DataTransfer::startSendTransfer(ActiveTransfer* transfer)
{
    if (!transfer || !transfer->connection) return;

    transfer->info.status = "Sending";

    QJsonObject metadata;
    metadata["type"] = "file_transfer";
    metadata["fileId"] = transfer->info.fileId.toString();
    metadata["fileName"] = transfer->info.fileName;
    metadata["fileSize"] = transfer->info.fileSize;
    metadata["totalChunks"] = transfer->totalChunks;
    metadata["chunkSize"] = Config::instance().getChunkSize();

    QJsonDocument doc(metadata);
    QByteArray metadataBytes = doc.toJson(QJsonDocument::Compact);

    QByteArray packet;
    packet.append(static_cast<char>(0x02));
    packet.append(metadataBytes);

    if (!transfer->connection->sendData(packet)) {
        LOG_ERROR("Failed to send metadata");
        emit transferFailed(transfer->info.transferId, "Failed to send metadata");
        return;
    }

    LOG_INFO(QString("Sent metadata for file: %1").arg(transfer->info.fileName));

    // 发送元数据后，开始发送第一个 chunk
    if (transfer->currentChunk < transfer->totalChunks) {
        QByteArray chunk = transfer->chunks[transfer->currentChunk];

        QByteArray chunkPacket;
        QDataStream stream(&chunkPacket, QIODevice::WriteOnly);
        stream << transfer->currentChunk << transfer->totalChunks;
        chunkPacket.append(chunk);

        if (transfer->connection->sendData(chunkPacket)) {
            transfer->currentChunk++;
            transfer->info.progress = (static_cast<double>(transfer->currentChunk) /
                                       transfer->totalChunks) * 100.0;
            transfer->info.status = QString("Sending: %1%").arg(transfer->info.progress, 0, 'f', 1);

            emit transferProgress(transfer->info.transferId, transfer->info.progress);

            LOG_DEBUG(QString("Sent initial chunk %1/%2")
                          .arg(transfer->currentChunk).arg(transfer->totalChunks));
        } else {
            LOG_ERROR("Failed to send first chunk");
            emit transferFailed(transfer->info.transferId, "Failed to send first chunk");
        }
    }
}

void DataTransfer::processReceivedChunk(ActiveTransfer* transfer, const QByteArray& data)
{
    if (!transfer) return;

    if (data.size() < 8) {
        LOG_WARNING("Received chunk data too small");
        return;
    }

    QDataStream stream(data);
    int chunkIndex;
    int totalChunks;
    stream >> chunkIndex >> totalChunks;

    if (chunkIndex < 0 || chunkIndex >= totalChunks) {
        LOG_ERROR(QString("Invalid chunk index: %1/%2").arg(chunkIndex).arg(totalChunks));
        return;
    }

    QByteArray chunkData = data.mid(8);

    m_chunkManager.addChunk(transfer->info.fileId, chunkIndex, totalChunks, chunkData);

    transfer->currentChunk = chunkIndex + 1;
    transfer->totalChunks = totalChunks;

    double progress = m_chunkManager.getProgress(transfer->info.fileId);
    transfer->info.progress = progress;
    transfer->info.status = QString("Receiving: %1%").arg(progress, 0, 'f', 1);

    emit transferProgress(transfer->info.transferId, progress);

    LOG_DEBUG(QString("Received chunk %1/%2, progress: %3%")
                  .arg(chunkIndex + 1).arg(totalChunks).arg(progress, 0, 'f', 1));

    // 发送确认给发送方
    QJsonObject ack;
    ack["type"] = "chunk_received";
    ack["chunkIndex"] = chunkIndex;
    QJsonDocument doc(ack);
    
    if (transfer->connection) {
        QByteArray ackData = doc.toJson(QJsonDocument::Compact);
        transfer->connection->sendData(ackData);
    }

    if (m_chunkManager.isFileComplete(transfer->info.fileId)) {
        finalizeReceive(transfer);
    }
}

void DataTransfer::finalizeReceive(ActiveTransfer* transfer)
{
    if (!transfer) return;

    transfer->info.status = "Finalizing";

    QByteArray fileData = m_chunkManager.reassembleFile(transfer->info.fileId);

    if (fileData.isEmpty()) {
        LOG_ERROR("Failed to reassemble file");
        emit transferFailed(transfer->info.transferId, "Failed to reassemble file");
        return;
    }

    QString finalPath = QDir::homePath() + "/Downloads/" + transfer->info.fileName;

    if (transfer->fileHandler.writeFile(finalPath, fileData)) {
        transfer->info.isCompleted = true;
        transfer->info.progress = 100.0;
        transfer->info.status = "Completed";

        LOG_INFO(QString("File received and saved: %1").arg(finalPath));
        emit transferCompleted(transfer->info.transferId, finalPath);
    } else {
        LOG_ERROR("Failed to save received file");
        emit transferFailed(transfer->info.transferId, "Failed to save file");
    }

    m_chunkManager.removeFile(transfer->info.fileId);
}

void DataTransfer::onConnectionDataReceived(const QByteArray& data)
{
    if (data.isEmpty()) return;

    QMutexLocker locker(&m_mutex);

    TCPConnection* senderConn = qobject_cast<TCPConnection*>(sender());
    if (!senderConn) return;

    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        if (it.value()->connection == senderConn) {
            ActiveTransfer* transfer = it.value();

            // 检查是否是元数据（以 0x02 开头）
            if (!data.isEmpty() && static_cast<unsigned char>(data[0]) == 0x02) {
                // 这是元数据，解析并准备接收
                QByteArray metadataBytes = data.mid(1);
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(metadataBytes, &parseError);

                if (parseError.error == QJsonParseError::NoError) {
                    QJsonObject metadata = doc.object();
                    QString type = metadata["type"].toString();

                    if (type == "file_transfer") {
                        // 发送方：元数据已发送，等待接收方准备好后开始发送 chunks
                        // 接收方：已经通过 prepareReceive 准备好了
                        LOG_INFO("Metadata exchange completed");
                    }
                }
                continue;
            }

            // 尝试解析为 JSON 消息（确认消息等）
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
            if (jsonError.error == QJsonParseError::NoError) {
                QJsonObject jsonObj = jsonDoc.object();
                QString msgType = jsonObj["type"].toString();

                if (msgType == "chunk_received" && transfer->info.isSending) {
                    // 发送方收到 chunk 确认，继续发送下一个 chunk
                    int ackChunkIndex = jsonObj["chunkIndex"].toInt();
                    LOG_DEBUG(QString("Chunk %1 acknowledged").arg(ackChunkIndex));

                    if (transfer->currentChunk < transfer->totalChunks) {
                        QByteArray chunk = transfer->chunks[transfer->currentChunk];

                        QByteArray packet;
                        QDataStream stream(&packet, QIODevice::WriteOnly);
                        stream << transfer->currentChunk << transfer->totalChunks;
                        packet.append(chunk);

                        if (transfer->connection->sendData(packet)) {
                            transfer->currentChunk++;
                            transfer->info.progress = (static_cast<double>(transfer->currentChunk) /
                                                       transfer->totalChunks) * 100.0;
                            transfer->info.status = QString("Sending: %1%").arg(transfer->info.progress, 0, 'f', 1);

                            emit transferProgress(transfer->info.transferId, transfer->info.progress);

                            LOG_DEBUG(QString("Sent chunk %1/%2")
                                          .arg(transfer->currentChunk).arg(transfer->totalChunks));
                        }

                        if (transfer->currentChunk >= transfer->totalChunks) {
                            transfer->info.isCompleted = true;
                            transfer->info.progress = 100.0;
                            transfer->info.status = "Completed";

                            LOG_INFO(QString("File sent successfully: %1").arg(transfer->info.fileName));
                            emit transferCompleted(transfer->info.transferId, transfer->info.fileName);
                        }
                    }
                    continue;
                }
            }

            // 处理普通数据包（chunks）- 仅接收方
            if (!transfer->info.isSending) {
                processReceivedChunk(transfer, data);
            }

            break;
        }
    }
}

void DataTransfer::onConnectionConnected()
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    QMutexLocker locker(&m_mutex);

    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        if (it.value()->connection == connection) {
            ActiveTransfer* transfer = it.value();

            if (transfer->info.isSending) {
                startSendTransfer(transfer);
            }

            break;
        }
    }
}

void DataTransfer::onConnectionDisconnected()
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    LOG_WARNING("Connection disconnected during transfer");
}

void DataTransfer::onConnectionError(const QString& error)
{
    TCPConnection* connection = qobject_cast<TCPConnection*>(sender());
    if (!connection) return;

    QMutexLocker locker(&m_mutex);

    for (auto it = m_activeTransfers.begin(); it != m_activeTransfers.end(); ++it) {
        if (it.value()->connection == connection) {
            ActiveTransfer* transfer = it.value();
            transfer->info.status = "Error: " + error;

            emit transferFailed(transfer->info.transferId, error);
            break;
        }
    }
}