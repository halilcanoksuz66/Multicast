#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "multicast_manager.h"
#include "audiocapture.h"

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

private slots:
    void onMessageReceived(const QString& message);
    void onSendButtonClicked();
    void onSaveButtonClicked();

private:
    Ui::MainWindow *ui;
    MulticastManager *multicastManager;
    AudioCapture *audioCapture;
};

#endif // MAINWINDOW_H
