#include "mainwindow.h"
#include "debuglogger.h"
#include <QApplication>
#include <QDateTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("MulticastVoiceApp");
    QCoreApplication::setApplicationName("MulticastVoiceApp");

    // Debug logger'ı başlat
    QString logFileName = QString("debug_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    DebugLogger::getInstance()->startLogging(logFileName);
    qDebug() << "Debug logger başlatıldı. Log dosyası:" << logFileName;

    MainWindow w;
    w.show();

    return a.exec();
}
