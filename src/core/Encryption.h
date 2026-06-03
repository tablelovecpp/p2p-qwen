#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QByteArray>
#include <QString>

class Encryption {
public:
    Encryption();

    // 加密数据
    QByteArray encrypt(const QByteArray& data, const QString& key);

    // 解密数据
    QByteArray decrypt(const QByteArray& data, const QString& key);

    // 生成随机密钥
    static QString generateKey();

    // 设置是否启用加密
    void setEnabled(bool enabled);
    bool isEnabled() const;

private:
    bool m_enabled;

    // 简单的XOR加密（生产环境应使用AES等更强算法）
    QByteArray xorEncrypt(const QByteArray& data, const QByteArray& key);
};

#endif // ENCRYPTION_H