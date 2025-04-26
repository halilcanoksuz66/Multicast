#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    // Singleton instance'ı almak için statik metot
    static SettingsManager* getInstance(QObject *parent = nullptr);
    
    // Ayarları yükleme ve kaydetme
    void loadSettings();
    void saveSettings();

    // Multicast ayarları
    QString getBroadcastAddress() const;
    void setBroadcastAddress(const QString &address);

    QStringList getListenAddresses() const;
    void setListenAddresses(const QStringList &addresses);

    // Ses cihazı ayarları
    int getInputDeviceIndex() const;
    void setInputDeviceIndex(int index);

    int getOutputDeviceIndex() const;
    void setOutputDeviceIndex(int index);

    // Codec ayarları
    int getBitrate() const;
    void setBitrate(int bitrate);

    int getFrameSize() const;
    void setFrameSize(int frameSize);

    // Volume ayarları
    float getVolume() const;
    void setVolume(float volume);

    // Samplerate ayarları
    int getSamplerate() const;
    void setSamplerate(int samplerate);

    // InputChannels ayarları
    int getInputChannels() const;
    void setInputChannels(int channels);

    // OutputChannels ayarları
    int getOutputChannels() const;
    void setOutputChannels(int channels);

    // Codec Channels ayarları
    int getChannels() const;
    void setChannels(int channels);

    // Microphone Muted ayarları
    bool isMicrophoneMuted() const;
    void setMicrophoneMuted(bool muted);

private:
    // Singleton için private constructor
    explicit SettingsManager(QObject *parent = nullptr);
    
    // Singleton için copy constructor ve assignment operator'ı engelleme
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;
    
    // Singleton instance
    static SettingsManager* instance;

    QSettings settings;

    // Geçici saklama için değişkenler
    QString broadcastAddress;
    QStringList listenAddresses;
    int inputDeviceIndex;
    int outputDeviceIndex;
    int bitrate;
    int frameSize;
    float volume;
    int samplerate;
    int inputChannels;
    int outputChannels;
    int channels;
    bool microphoneMuted;

    // Varsayılan değerler
    static QString DEFAULT_BROADCAST_ADDRESS;
    static QStringList DEFAULT_LISTEN_ADDRESSES;
    static int DEFAULT_INPUT_DEVICE;
    static int DEFAULT_OUTPUT_DEVICE;
    static int DEFAULT_BITRATE;
    static int DEFAULT_FRAME_SIZE;
    static float DEFAULT_VOLUME;
    static int DEFAULT_SAMPLERATE;
    static int DEFAULT_INPUT_CHANNELS;
    static int DEFAULT_OUTPUT_CHANNELS;
    static int DEFAULT_CHANNELS;
    static bool DEFAULT_MICROPHONE_MUTED;
};

#endif // SETTINGSMANAGER_H
