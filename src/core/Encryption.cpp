#include "Encryption.h"
#include "../utils/Logger.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

Encryption::Encryption()
    : m_enabled(false)
{
}

QByteArray Encryption::encrypt(const QByteArray& data, const QString& key)
{
    if (!m_enabled || key.isEmpty()) {
        return data;
    }

    QByteArray keyBytes = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
    QByteArray encrypted = xorEncrypt(data, keyBytes);

    LOG_DEBUG(QString("Encrypted %1 bytes").arg(data.size()));
    return encrypted;
}

QByteArray Encryption::decrypt(const QByteArray& data, const QString& key)
{
    if (!m_enabled || key.isEmpty()) {
        return data;
    }

    QByteArray keyBytes = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256);
    QByteArray decrypted = xorEncrypt(data, keyBytes);  // XOR是对称的

    LOG_DEBUG(QString("Decrypted %1 bytes").arg(data.size()));
    return decrypted;
}

QString Encryption::generateKey()
{
    QByteArray randomBytes(32, 0);
    QRandomGenerator::global()->fillRange(reinterpret_cast<uint*>(randomBytes.data()), 8);
    return randomBytes.toHex();
}

void Encryption::setEnabled(bool enabled)
{
    m_enabled = enabled;
    LOG_INFO(QString("Encryption %1").arg(enabled ? "enabled" : "disabled"));
}

bool Encryption::isEnabled() const
{
    return m_enabled;
}

QByteArray Encryption::xorEncrypt(const QByteArray& data, const QByteArray& key)
{
    if (key.isEmpty()) {
        return data;
    }

    QByteArray result(data.size(), 0);
    int keyLen = key.size();

    for (int i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % keyLen];
    }

    return result;
}