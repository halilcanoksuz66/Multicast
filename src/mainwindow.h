#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "audiocapture.h"
#include "multicast_manager.h"
#include "audio_codec.h"

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
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onSaveRequested();
    void onAudioReceived(std::vector<char> data);
    void onMessageReceived(const QString &message);

private:
    Ui::MainWindow *ui;
    AudioCapture *audioCapture;
    MulticastManager *multicastManager;
    AudioCodec *audioCodec;
};

#endif // MAINWINDOW_H
