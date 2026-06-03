#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QString>
#include <QByteArray>
#include <QFileInfo>
#include <QUuid>

struct FileInfo {
    QUuid id;
    QString fileName;
    QString filePath;
    qint64 fileSize;
    QString checksum;
    QDateTime modifiedTime;

    FileInfo() : fileSize(0) {}
};

class FileHandler {
public:
    FileHandler();

    // 读取文件
    QByteArray readFile(const QString& filePath);

    // 写入文件
    bool writeFile(const QString& filePath, const QByteArray& data);

    // 获取文件信息
    FileInfo getFileInfo(const QString& filePath);

    // 计算文件校验和（SHA-256简化版）
    QString calculateChecksum(const QByteArray& data);

    // 验证文件完整性
    bool verifyFile(const QString& filePath, const QString& expectedChecksum);

    // 创建临时文件路径
    QString createTempFilePath(const QString& fileName);

    // 删除文件
    bool deleteFile(const QString& filePath);

    // 检查文件是否存在
    bool fileExists(const QString& filePath);

    // 获取文件大小
    qint64 getFileSize(const QString& filePath);
};

#endif // FILEHANDLER_H