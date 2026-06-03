#include "FileHandler.h"
#include "../utils/Logger.h"
#include <QCryptographicHash>
#include <QDir>
#include <QStandardPaths>

FileHandler::FileHandler()
{
}

QByteArray FileHandler::readFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_ERROR(QString("Failed to open file for reading: %1").arg(filePath));
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();

    LOG_DEBUG(QString("Read file: %1, size: %2 bytes").arg(filePath).arg(data.size()));
    return data;
}

bool FileHandler::writeFile(const QString& filePath, const QByteArray& data)
{
    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            LOG_ERROR(QString("Failed to create directory: %1").arg(dir.absolutePath()));
            return false;
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_ERROR(QString("Failed to open file for writing: %1").arg(filePath));
        return false;
    }

    qint64 written = file.write(data);
    file.close();

    if (written != data.size()) {
        LOG_ERROR(QString("Failed to write complete file: %1").arg(filePath));
        return false;
    }

    LOG_DEBUG(QString("Wrote file: %1, size: %2 bytes").arg(filePath).arg(written));
    return true;
}

FileInfo FileHandler::getFileInfo(const QString& filePath)
{
    FileInfo info;
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists()) {
        LOG_ERROR(QString("File does not exist: %1").arg(filePath));
        return info;
    }

    info.id = QUuid::createUuid();
    info.fileName = fileInfo.fileName();
    info.filePath = filePath;
    info.fileSize = fileInfo.size();
    info.modifiedTime = fileInfo.lastModified();

    // 计算校验和
    QByteArray data = readFile(filePath);
    if (!data.isEmpty()) {
        info.checksum = calculateChecksum(data);
    }

    return info;
}

QString FileHandler::calculateChecksum(const QByteArray& data)
{
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex();
}

bool FileHandler::verifyFile(const QString& filePath, const QString& expectedChecksum)
{
    QByteArray data = readFile(filePath);
    if (data.isEmpty()) {
        return false;
    }

    QString actualChecksum = calculateChecksum(data);
    bool valid = (actualChecksum == expectedChecksum);

    if (!valid) {
        LOG_WARNING(QString("File verification failed: %1").arg(filePath));
    }

    return valid;
}

QString FileHandler::createTempFilePath(const QString& fileName)
{
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString uniqueName = QUuid::createUuid().toString(QUuid::WithoutBraces) + "_" + fileName;
    return QDir(tempDir).filePath(uniqueName);
}

bool FileHandler::deleteFile(const QString& filePath)
{
    QFile file(filePath);
    if (file.exists()) {
        bool result = file.remove();
        if (result) {
            LOG_DEBUG(QString("Deleted file: %1").arg(filePath));
        } else {
            LOG_ERROR(QString("Failed to delete file: %1").arg(filePath));
        }
        return result;
    }
    return false;
}

bool FileHandler::fileExists(const QString& filePath)
{
    return QFile::exists(filePath);
}

qint64 FileHandler::getFileSize(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() ? fileInfo.size() : 0;
}