#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "multicast_receiver.h"
#include "multicast_sender.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void onMessageReceived(const QString& message);
    void onSendButtonClicked();

private:
    Ui::MainWindow *ui;
    MulticastReceiver *multicastReceiver;
    MulticastSender* multicastSender;
    QThread *receiverThread;

};
#endif // MAINWINDOW_H
