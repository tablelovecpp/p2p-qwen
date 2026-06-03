#include "ChunkManager.h"
#include "../utils/Logger.h"

ChunkManager::ChunkManager(int chunkSize)
    : m_chunkSize(chunkSize)
{
}

QVector<QByteArray> ChunkManager::splitData(const QByteArray& data)
{
    QVector<QByteArray> chunks;
    int totalSize = data.size();
    int numChunks = (totalSize + m_chunkSize - 1) / m_chunkSize;

    for (int i = 0; i < numChunks; ++i) {
        int offset = i * m_chunkSize;
        int size = qMin(m_chunkSize, totalSize - offset);
        chunks.append(data.mid(offset, size));
    }

    LOG_DEBUG(QString("Split data into %1 chunks").arg(numChunks));
    return chunks;
}

QByteArray ChunkManager::getChunk(const QUuid& fileId, int index)
{
    QMutexLocker locker(&m_mutex);

    if (!m_chunks.contains(fileId)) {
        return QByteArray();
    }

    auto& fileChunks = m_chunks[fileId];
    if (!fileChunks.contains(index)) {
        return QByteArray();
    }

    return fileChunks[index].data;
}

bool ChunkManager::addChunk(const QUuid& fileId, int index, int totalChunks, const QByteArray& data)
{
    QMutexLocker locker(&m_mutex);

    if (!m_chunks.contains(fileId)) {
        m_chunks[fileId] = QHash<int, ChunkInfo>();
    }

    auto& fileChunks = m_chunks[fileId];

    ChunkInfo info;
    info.fileId = fileId;
    info.chunkIndex = index;
    info.totalChunks = totalChunks;
    info.data = data;
    info.isReceived = true;

    fileChunks[index] = info;

    LOG_DEBUG(QString("Added chunk %1/%2 for file %3")
                  .arg(index + 1).arg(totalChunks).arg(fileId.toString()));

    return true;
}

bool ChunkManager::isFileComplete(const QUuid& fileId)
{
    QMutexLocker locker(&m_mutex);

    if (!m_chunks.contains(fileId)) {
        return false;
    }

    auto& fileChunks = m_chunks[fileId];
    if (fileChunks.isEmpty()) {
        return false;
    }

    // 获取总块数（从任意一个块中获取）
    int totalChunks = fileChunks.begin()->totalChunks;

    // 检查是否所有块都已接收
    for (int i = 0; i < totalChunks; ++i) {
        if (!fileChunks.contains(i) || !fileChunks[i].isReceived) {
            return false;
        }
    }

    return true;
}

QByteArray ChunkManager::reassembleFile(const QUuid& fileId)
{
    QMutexLocker locker(&m_mutex);

    if (!m_chunks.contains(fileId)) {
        LOG_ERROR("File not found for reassembly");
        return QByteArray();
    }

    auto& fileChunks = m_chunks[fileId];
    if (fileChunks.isEmpty()) {
        return QByteArray();
    }

    int totalChunks = fileChunks.begin()->totalChunks;
    QByteArray result;

    // 预估总大小
    qint64 totalSize = 0;
    for (int i = 0; i < totalChunks; ++i) {
        if (fileChunks.contains(i)) {
            totalSize += fileChunks[i].data.size();
        }
    }

    result.reserve(totalSize);

    // 按顺序拼接
    for (int i = 0; i < totalChunks; ++i) {
        if (fileChunks.contains(i)) {
            result.append(fileChunks[i].data);
        } else {
            LOG_ERROR(QString("Missing chunk %1 for file %2").arg(i).arg(fileId.toString()));
            return QByteArray();
        }
    }

    LOG_INFO(QString("Reassembled file %1, size: %2 bytes").arg(fileId.toString()).arg(result.size()));
    return result;
}

void ChunkManager::removeFile(const QUuid& fileId)
{
    QMutexLocker locker(&m_mutex);
    m_chunks.remove(fileId);
    LOG_DEBUG(QString("Removed file chunks for %1").arg(fileId.toString()));
}

double ChunkManager::getProgress(const QUuid& fileId)
{
    QMutexLocker locker(&m_mutex);

    if (!m_chunks.contains(fileId)) {
        return 0.0;
    }

    auto& fileChunks = m_chunks[fileId];
    if (fileChunks.isEmpty()) {
        return 0.0;
    }

    int totalChunks = fileChunks.begin()->totalChunks;
    int receivedChunks = 0;

    for (int i = 0; i < totalChunks; ++i) {
        if (fileChunks.contains(i) && fileChunks[i].isReceived) {
            receivedChunks++;
        }
    }

    return (totalChunks > 0) ? (static_cast<double>(receivedChunks) / totalChunks * 100.0) : 0.0;
}

int ChunkManager::getTotalChunks(const QUuid& fileId) const
{
    if (!m_chunks.contains(fileId)) {
        return 0;
    }

    auto& fileChunks = m_chunks[fileId];
    if (fileChunks.isEmpty()) {
        return 0;
    }

    return fileChunks.begin()->totalChunks;
}