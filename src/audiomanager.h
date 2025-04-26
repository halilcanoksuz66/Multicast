#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QList>
#include <QPair>
#include "settingsmanager.h"
#include "JitterBuffer.h"
#include <portaudio.h>

class AudioManager : public QObject
{
    Q_OBJECT
public:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();

    bool initialize();
    void terminate();

    QList<QString> getInputDevices();
    QList<QString> getOutputDevices();

    bool setInputDevice(int deviceIndex);
    bool setOutputDevice(int deviceIndex);

    bool startAudioStream();
    void stopAudioStream();

    void setMicrophoneMuted(bool muted);
    bool isMicrophoneMuted() const;

    int getSampleRate() const { return sampleRate; }
    int getInputChannels() const { return inputChannels; }
    int getOutputChannels() const { return outputChannels; }
    void setVolume(float newVolume);
    float getVolume() const { return volume; }

signals:
    void inputAudioReady(const QByteArray &audioData);
    void audioStreamStarted();
    void audioStreamStopped();
    void audioError(const QString &errorMessage);

public slots:
    void handleOutputAudio(const QByteArray &audioData, const QString &sourceAddress);

private slots:
    void emitInputAudioReady(const QByteArray &audioData);

private:
    PaStream *stream;

    static int audioCallback(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo *timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData);

    void refreshDeviceLists();
    void setDefaultDevices();

    int sampleRate;
    int framesPerBuffer;
    int inputChannels;
    int outputChannels;
    int selectedInputDevice;
    int selectedOutputDevice;
    bool microphoneMuted;
    float volume;

    struct DeviceInfo {
        const PaDeviceInfo *info;
        int paDeviceIndex;
    };

    QList<DeviceInfo> inputDeviceList;
    QList<DeviceInfo> outputDeviceList;

    QByteArray inputBuffer;
    QMap<QString, QByteArray> outputBuffers;
    SettingsManager *settingsManager;
    QMap<QString, std::shared_ptr<JitterBuffer>> jitterBuffers;
    QMutex jitterBufferMapMutex;
};

#endif // AUDIOMANAGER_H
