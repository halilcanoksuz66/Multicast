#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QDebug>

class DebugLogger : public QObject
{
    Q_OBJECT
public:
    static DebugLogger* getInstance();
    
    void startLogging(const QString& filename = "debug_log.txt");
    void stopLogging();
    void logMessage(const QString& message, QtMsgType type = QtDebugMsg);
    
    // QDebug mesajlarını yakalamak için
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    explicit DebugLogger(QObject *parent = nullptr);
    ~DebugLogger();
    
    static DebugLogger* instance;
    QFile logFile;
    QTextStream logStream;
    QMutex mutex;
    bool isLogging;
    
    QString getMessageTypeString(QtMsgType type);
};

// Debug mesajlarını kolayca loglamak için makro
#define LOG_DEBUG(msg) DebugLogger::getInstance()->logMessage(msg, QtDebugMsg)
#define LOG_INFO(msg) DebugLogger::getInstance()->logMessage(msg, QtInfoMsg)
#define LOG_WARNING(msg) DebugLogger::getInstance()->logMessage(msg, QtWarningMsg)
#define LOG_CRITICAL(msg) DebugLogger::getInstance()->logMessage(msg, QtCriticalMsg)
#define LOG_FATAL(msg) DebugLogger::getInstance()->logMessage(msg, QtFatalMsg)

#endif // DEBUGLOGGER_H 