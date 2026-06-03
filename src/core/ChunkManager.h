#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <QByteArray>
#include <QHash>
#include <QMutex>
#include <QVector>
#include <QUuid>

struct ChunkInfo {
    QUuid fileId;
    int chunkIndex;
    int totalChunks;
    QByteArray data;
    bool isReceived;

    ChunkInfo() : chunkIndex(0), totalChunks(0), isReceived(false) {}
};

class ChunkManager {
public:
    ChunkManager(int chunkSize = 65536);

    // 将数据分割成块
    QVector<QByteArray> splitData(const QByteArray& data);

    // 获取指定块
    QByteArray getChunk(const QUuid& fileId, int index);

    // 添加接收到的块
    bool addChunk(const QUuid& fileId, int index, int totalChunks, const QByteArray& data);

    // 检查文件是否完整
    bool isFileComplete(const QUuid& fileId);

    // 重组完整文件
    QByteArray reassembleFile(const QUuid& fileId);

    // 清理文件数据
    void removeFile(const QUuid& fileId);

    // 获取文件进度
    double getProgress(const QUuid& fileId);

    // 获取总块数
    int getTotalChunks(const QUuid& fileId) const;

private:
    int m_chunkSize;
    QHash<QUuid, QHash<int, ChunkInfo>> m_chunks;
    QMutex m_mutex;
};

#endif // CHUNKMANAGER_H