#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <iostream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void setLogFile(const QString& filename) {
        QMutexLocker locker(&m_mutex);
        if (m_file.isOpen()) {
            m_file.close();
        }
        m_file.setFileName(filename);
        if (m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            m_stream.setDevice(&m_file);
        }
    }

    void log(LogLevel level, const QString& message, const QString& context = "") {
        QMutexLocker locker(&m_mutex);

        QString levelStr;
        switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::CRITICAL: levelStr = "CRITICAL"; break;
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString logMessage = QString("[%1] [%2] %3").arg(timestamp, levelStr, message);

        if (!context.isEmpty()) {
            logMessage += QString(" (%1)").arg(context);
        }

        std::cout << logMessage.toStdString() << std::endl;

        if (m_stream.device()) {
            m_stream << logMessage << "\n";
            m_stream.flush();
        }
    }

private:
    Logger() {}
    QMutex m_mutex;
    QFile m_file;
    QTextStream m_stream;
};

#define LOG_DEBUG(msg) Logger::instance().log(LogLevel::DEBUG, msg, __FUNCTION__)
#define LOG_INFO(msg) Logger::instance().log(LogLevel::INFO, msg, __FUNCTION__)
#define LOG_WARNING(msg) Logger::instance().log(LogLevel::WARNING, msg, __FUNCTION__)
#define LOG_ERROR(msg) Logger::instance().log(LogLevel::ERROR, msg, __FUNCTION__)
#define LOG_CRITICAL(msg) Logger::instance().log(LogLevel::CRITICAL, msg, __FUNCTION__)

#endif // LOGGER_H