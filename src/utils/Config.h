#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>

class Config {
public:
    static Config& instance() {
        static Config config;
        return config;
    }

    // 网络配置
    int getListenPort() const { return m_listenPort; }
    void setListenPort(int port) { m_listenPort = port; }

    int getMaxConnections() const { return m_maxConnections; }
    void setMaxConnections(int max) { m_maxConnections = max; }

    // 传输配置
    int getChunkSize() const { return m_chunkSize; }
    void setChunkSize(int size) { m_chunkSize = size; }

    int getBufferSize() const { return m_bufferSize; }
    void setBufferSize(int size) { m_bufferSize = size; }

    int getMaxThreads() const { return m_maxThreads; }
    void setMaxThreads(int threads) { m_maxThreads = threads; }

    // 安全配置
    bool getEncryptionEnabled() const { return m_encryptionEnabled; }
    void setEncryptionEnabled(bool enabled) { m_encryptionEnabled = enabled; }

    QString getEncryptionKey() const { return m_encryptionKey; }
    void setEncryptionKey(const QString& key) { m_encryptionKey = key; }

    // NAT穿透配置
    bool getHolePunchingEnabled() const { return m_holePunchingEnabled; }
    void setHolePunchingEnabled(bool enabled) { m_holePunchingEnabled = enabled; }

    int getKeepAliveInterval() const { return m_keepAliveInterval; }
    void setKeepAliveInterval(int interval) { m_keepAliveInterval = interval; }

    void loadFromFile(const QString& filename = "config.ini") {
        QSettings settings(filename, QSettings::IniFormat);
        m_listenPort = settings.value("Network/ListenPort", 8888).toInt();
        m_maxConnections = settings.value("Network/MaxConnections", 50).toInt();
        m_chunkSize = settings.value("Transfer/ChunkSize", 65536).toInt();
        m_bufferSize = settings.value("Transfer/BufferSize", 1048576).toInt();
        m_maxThreads = settings.value("Transfer/MaxThreads", 10).toInt();
        m_encryptionEnabled = settings.value("Security/EncryptionEnabled", false).toBool();
        m_encryptionKey = settings.value("Security/EncryptionKey", "").toString();
        m_holePunchingEnabled = settings.value("NAT/HolePunchingEnabled", true).toBool();
        m_keepAliveInterval = settings.value("NAT/KeepAliveInterval", 30).toInt();
    }

    void saveToFile(const QString& filename = "config.ini") {
        QSettings settings(filename, QSettings::IniFormat);
        settings.setValue("Network/ListenPort", m_listenPort);
        settings.setValue("Network/MaxConnections", m_maxConnections);
        settings.setValue("Transfer/ChunkSize", m_chunkSize);
        settings.setValue("Transfer/BufferSize", m_bufferSize);
        settings.setValue("Transfer/MaxThreads", m_maxThreads);
        settings.setValue("Security/EncryptionEnabled", m_encryptionEnabled);
        settings.setValue("Security/EncryptionKey", m_encryptionKey);
        settings.setValue("NAT/HolePunchingEnabled", m_holePunchingEnabled);
        settings.setValue("NAT/KeepAliveInterval", m_keepAliveInterval);
    }

private:
    Config() {
        m_listenPort = 8888;
        m_maxConnections = 50;
        m_chunkSize = 65536;      // 64KB
        m_bufferSize = 1048576;   // 1MB
        m_maxThreads = 10;
        m_encryptionEnabled = false;
        m_holePunchingEnabled = true;
        m_keepAliveInterval = 30;
    }

    int m_listenPort;
    int m_maxConnections;
    int m_chunkSize;
    int m_bufferSize;
    int m_maxThreads;
    bool m_encryptionEnabled;
    QString m_encryptionKey;
    bool m_holePunchingEnabled;
    int m_keepAliveInterval;
};

#endif // CONFIG_H