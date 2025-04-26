#include "debuglogger.h"
#include <QDir>
#include <QStandardPaths>

DebugLogger* DebugLogger::instance = nullptr;

DebugLogger* DebugLogger::getInstance()
{
    if (!instance) {
        instance = new DebugLogger();
    }
    return instance;
}

DebugLogger::DebugLogger(QObject *parent) : QObject(parent), isLogging(false)
{
    // QDebug mesajlarını yakalamak için handler'ı ayarla
    qInstallMessageHandler(messageHandler);
}

DebugLogger::~DebugLogger()
{
    stopLogging();
}

void DebugLogger::startLogging(const QString& filename)
{
    QMutexLocker locker(&mutex);
    
    if (isLogging) {
        return;
    }
    
    // Log dosyasını aç
    logFile.setFileName(filename);
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Log dosyası açılamadı:" << filename;
        return;
    }
    
    logStream.setDevice(&logFile);
    isLogging = true;
    
    // Başlangıç mesajını yaz
    logStream << "\n\n=== Log başladı: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " ===\n";
    logStream.flush();
}

void DebugLogger::stopLogging()
{
    QMutexLocker locker(&mutex);
    
    if (!isLogging) {
        return;
    }
    
    // Bitiş mesajını yaz
    logStream << "\n=== Log bitti: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " ===\n\n";
    logStream.flush();
    
    logFile.close();
    isLogging = false;
}

void DebugLogger::logMessage(const QString& message, QtMsgType type)
{
    QMutexLocker locker(&mutex);
    
    if (!isLogging) {
        return;
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString typeStr = getMessageTypeString(type);
    
    logStream << timestamp << " [" << typeStr << "] " << message << "\n";
    logStream.flush();
}

void DebugLogger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Orijinal mesajı konsola yazdır
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }
    
    // Mesajı dosyaya kaydet
    QString fullMessage = QString("%1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    getInstance()->logMessage(fullMessage, type);
}

QString DebugLogger::getMessageTypeString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return "DEBUG";
    case QtInfoMsg:
        return "INFO";
    case QtWarningMsg:
        return "WARNING";
    case QtCriticalMsg:
        return "CRITICAL";
    case QtFatalMsg:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
} 