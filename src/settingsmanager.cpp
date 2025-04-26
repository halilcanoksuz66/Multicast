#include "settingsmanager.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDir>
#include <QDebug>

// Static instance'ı tanımla
SettingsManager* SettingsManager::instance = nullptr;

// Varsayılan değerler
 QString SettingsManager::DEFAULT_BROADCAST_ADDRESS = "239.255.0.1";
 QStringList SettingsManager::DEFAULT_LISTEN_ADDRESSES = {"239.255.0.2", "239.255.0.3"};
 int SettingsManager::DEFAULT_INPUT_DEVICE = 0;
 int SettingsManager::DEFAULT_OUTPUT_DEVICE = 0;
 int SettingsManager::DEFAULT_BITRATE = 96000;
 float SettingsManager::DEFAULT_VOLUME = 1.0f;
 int SettingsManager::DEFAULT_SAMPLERATE = 48000;
 int SettingsManager::DEFAULT_FRAME_SIZE = 480;
 int SettingsManager::DEFAULT_INPUT_CHANNELS = 1; // Mono by default
 int SettingsManager::DEFAULT_OUTPUT_CHANNELS = 1; // Mono by default
 int SettingsManager::DEFAULT_CHANNELS = 1; // Mono by default for codec
 bool SettingsManager::DEFAULT_MICROPHONE_MUTED = false; // Microphone not muted by default

// Singleton instance'ı almak için statik metot
SettingsManager* SettingsManager::getInstance(QObject *parent)
{
    if (instance != nullptr)
    {
        return instance;
    }

    instance = new SettingsManager(parent);

    return instance;
}


SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    broadcastAddress(DEFAULT_BROADCAST_ADDRESS),
    listenAddresses(DEFAULT_LISTEN_ADDRESSES),
    inputDeviceIndex(DEFAULT_INPUT_DEVICE),
    outputDeviceIndex(DEFAULT_OUTPUT_DEVICE),
    bitrate(DEFAULT_BITRATE),
    frameSize(DEFAULT_FRAME_SIZE),
    volume(DEFAULT_VOLUME),
    samplerate(DEFAULT_SAMPLERATE),
    inputChannels(DEFAULT_INPUT_CHANNELS),
    outputChannels(DEFAULT_OUTPUT_CHANNELS),
    channels(DEFAULT_CHANNELS),
    microphoneMuted(DEFAULT_MICROPHONE_MUTED)
{
    loadSettings();
    saveSettings();
}

// xml den veriler mevcut değişkenlere çekiliyor. Eğer yoksa da default değişkenler mevcut değişkenlere atanıyor.
void SettingsManager::loadSettings()
{
    QFile file("settings.xml");
    if (!file.exists()) {
        qDebug() << "Settings file does not exist, using defaults";
        return; // Dosya yoksa varsayılan değerleri kullan
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open settings file for reading";
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qDebug() << "Could not parse settings file";
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.isNull()) {
        qDebug() << "Settings file has no root element";
        return;
    }

    // Multicast ayarları
    QDomElement broadcastElement = root.firstChildElement("broadcastAddress");
    if (!broadcastElement.isNull()) {
        QString addr = broadcastElement.text().trimmed();
        if (!addr.isEmpty()) {
            broadcastAddress = addr;
            qDebug() << "Broadcast address loaded:" << broadcastAddress;
        } else {
            broadcastAddress = DEFAULT_BROADCAST_ADDRESS;
            qDebug() << "Broadcast address empty, using default:" << broadcastAddress;
        }
    } else {
        broadcastAddress = DEFAULT_BROADCAST_ADDRESS;
        qDebug() << "Broadcast element not found, using default:" << broadcastAddress;
    }


    QDomElement listenAddressesElement = root.firstChildElement("listenAddresses");
    if (!listenAddressesElement.isNull()) {
        listenAddresses.clear();
        QDomNodeList addressNodes = listenAddressesElement.elementsByTagName("address");
        for (int i = 0; i < addressNodes.count(); ++i) {
            QDomElement addressElement = addressNodes.at(i).toElement();
            QString addressText = addressElement.text().trimmed();
            if (!addressElement.isNull() && !addressText.isEmpty()) {
                listenAddresses.append(addressText);
            }
        }

        if (listenAddresses.isEmpty()) {
            listenAddresses = DEFAULT_LISTEN_ADDRESSES;
            qDebug() << "No valid listen addresses found, using default:" << listenAddresses;
        } else {
            qDebug() << "Listen addresses loaded:" << listenAddresses;
        }
    } else {
        listenAddresses = DEFAULT_LISTEN_ADDRESSES;
        qDebug() << "Listen addresses element not found, using default:" << listenAddresses;
    }


    // Ses cihazı ayarları
    QDomElement inputDeviceElement = root.firstChildElement("inputDevice");
    if (!inputDeviceElement.isNull()) {
        bool ok = false;
        int deviceIndex = inputDeviceElement.text().trimmed().toInt(&ok);
        if (ok) {
            inputDeviceIndex = deviceIndex;
            qDebug() << "Input device index loaded:" << inputDeviceIndex;
        } else {
            inputDeviceIndex = DEFAULT_INPUT_DEVICE;
            qDebug() << "Failed to convert input device index, using default:" << inputDeviceIndex;
        }
    } else {
        inputDeviceIndex = DEFAULT_INPUT_DEVICE;
        qDebug() << "Input device element not found, using default:" << inputDeviceIndex;
    }


    QDomElement outputDeviceElement = root.firstChildElement("outputDevice");
    if (!outputDeviceElement.isNull()) {
        bool ok = false;
        int deviceIndex = outputDeviceElement.text().trimmed().toInt(&ok);
        if (ok) {
            outputDeviceIndex = deviceIndex;
            qDebug() << "Output device index loaded:" << outputDeviceIndex;
        } else {
            outputDeviceIndex = DEFAULT_OUTPUT_DEVICE;
            qDebug() << "Failed to convert output device index, using default:" << outputDeviceIndex;
        }
    } else {
        outputDeviceIndex = DEFAULT_OUTPUT_DEVICE;
        qDebug() << "Output device element not found, using default:" << outputDeviceIndex;
    }

    // Codec ayarları
    QDomElement bitrateElement = root.firstChildElement("bitrate");
    if (!bitrateElement.isNull()) {
        bool ok = false;
        int value = bitrateElement.text().trimmed().toInt(&ok);
        if (ok) {
            bitrate = value;
            qDebug() << "Bitrate loaded:" << bitrate;
        } else {
            bitrate = DEFAULT_BITRATE;
            qDebug() << "Failed to convert bitrate, using default:" << bitrate;
        }
    } else {
        bitrate = DEFAULT_BITRATE;
        qDebug() << "Bitrate element not found, using default:" << bitrate;
    }

    // Samplerate ayarı
    QDomElement samplerateElement = root.firstChildElement("samplerate");
    if (!samplerateElement.isNull()) {
        bool ok = false;
        int value = samplerateElement.text().trimmed().toInt(&ok);
        if (ok) {
            samplerate = value;
            qDebug() << "Samplerate loaded:" << samplerate;
        } else {
            samplerate = DEFAULT_SAMPLERATE;
            qDebug() << "Failed to convert samplerate, using default:" << samplerate;
        }
    } else {
        samplerate = DEFAULT_SAMPLERATE;
        qDebug() << "Samplerate element not found, using default:" << samplerate;
    }

    // InputChannels ayarı
    QDomElement inputChannelsElement = root.firstChildElement("inputChannels");
    if (!inputChannelsElement.isNull()) {
        bool ok = false;
        int value = inputChannelsElement.text().trimmed().toInt(&ok);
        if (ok) {
            inputChannels = value;
            qDebug() << "InputChannels loaded:" << inputChannels;
        } else {
            inputChannels = DEFAULT_INPUT_CHANNELS;
            qDebug() << "Failed to convert inputChannels, using default:" << inputChannels;
        }
    } else {
        inputChannels = DEFAULT_INPUT_CHANNELS;
        qDebug() << "InputChannels element not found, using default:" << inputChannels;
    }

    // OutputChannels ayarı
    QDomElement outputChannelsElement = root.firstChildElement("outputChannels");
    if (!outputChannelsElement.isNull()) {
        bool ok = false;
        int value = outputChannelsElement.text().trimmed().toInt(&ok);
        if (ok) {
            outputChannels = value;
            qDebug() << "OutputChannels loaded:" << outputChannels;
        } else {
            outputChannels = DEFAULT_OUTPUT_CHANNELS;
            qDebug() << "Failed to convert outputChannels, using default:" << outputChannels;
        }
    } else {
        outputChannels = DEFAULT_OUTPUT_CHANNELS;
        qDebug() << "OutputChannels element not found, using default:" << outputChannels;
    }

    // Codec Channels ayarı
    QDomElement channelsElement = root.firstChildElement("channels");
    if (!channelsElement.isNull()) {
        bool ok = false;
        int value = channelsElement.text().trimmed().toInt(&ok);
        if (ok) {
            channels = value;
            qDebug() << "Codec Channels loaded:" << channels;
        } else {
            channels = DEFAULT_CHANNELS;
            qDebug() << "Failed to convert channels, using default:" << channels;
        }
    } else {
        channels = DEFAULT_CHANNELS;
        qDebug() << "Channels element not found, using default:" << channels;
    }

    // Microphone Muted ayarı
    QDomElement microphoneMutedElement = root.firstChildElement("microphoneMuted");
    if (!microphoneMutedElement.isNull()) {
        QString value = microphoneMutedElement.text().trimmed().toLower();
        microphoneMuted = (value == "true" || value == "1");
        qDebug() << "Microphone Muted loaded:" << microphoneMuted;
    } else {
        microphoneMuted = DEFAULT_MICROPHONE_MUTED;
        qDebug() << "Microphone Muted element not found, using default:" << microphoneMuted;
    }

    QDomElement frameSizeElement = root.firstChildElement("frameSize");
    if (!frameSizeElement.isNull()) {
        bool ok = false;
        int value = frameSizeElement.text().trimmed().toInt(&ok);
        if (ok) {
            frameSize = value;
            qDebug() << "Frame size loaded:" << frameSize;
        } else {
            frameSize = DEFAULT_FRAME_SIZE;
            qDebug() << "Failed to convert frame size, using default:" << frameSize;
        }
    } else {
        frameSize = DEFAULT_FRAME_SIZE;
        qDebug() << "Frame size element not found, using default:" << frameSize;
    }


    // Volume ayarı
    QDomElement volumeElement = root.firstChildElement("volume");
    if (!volumeElement.isNull()) {
        bool ok = false;
        float value = volumeElement.text().trimmed().toFloat(&ok);
        if (ok) {
            volume = value;
            qDebug() << "Volume loaded:" << volume;
        } else {
            volume = DEFAULT_VOLUME;
            qDebug() << "Failed to convert volume, using default:" << volume;
        }
    } else {
        volume = DEFAULT_VOLUME;
        qDebug() << "Volume element not found, using default:" << volume;
    }
}

// mevcut değişkenlerde ki değerler xml ' e yazılıp kaydediliyor.
void SettingsManager::saveSettings()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("settings");
    doc.appendChild(root);

    // Multicast ayarları
    QDomElement broadcastElement = doc.createElement("broadcastAddress");
    QDomText broadcastText = doc.createTextNode(broadcastAddress);
    broadcastElement.appendChild(broadcastText);
    root.appendChild(broadcastElement);

    QDomElement listenAddressesElement = doc.createElement("listenAddresses");
    for (const QString &address : listenAddresses) {
        QDomElement addressElement = doc.createElement("address");
        QDomText addressText = doc.createTextNode(address);
        addressElement.appendChild(addressText);
        listenAddressesElement.appendChild(addressElement);
    }
    root.appendChild(listenAddressesElement);

    // Ses cihazı ayarları
    QDomElement inputDeviceElement = doc.createElement("inputDevice");
    QDomText inputDeviceText = doc.createTextNode(QString::number(inputDeviceIndex));
    inputDeviceElement.appendChild(inputDeviceText);
    root.appendChild(inputDeviceElement);

    QDomElement outputDeviceElement = doc.createElement("outputDevice");
    QDomText outputDeviceText = doc.createTextNode(QString::number(outputDeviceIndex));
    outputDeviceElement.appendChild(outputDeviceText);
    root.appendChild(outputDeviceElement);

    // Codec ayarları
    QDomElement bitrateElement = doc.createElement("bitrate");
    QDomText bitrateText = doc.createTextNode(QString::number(bitrate));
    bitrateElement.appendChild(bitrateText);
    root.appendChild(bitrateElement);

    QDomElement samplerateElement = doc.createElement("samplerate");
    QDomText samplerateText = doc.createTextNode(QString::number(samplerate));
    samplerateElement.appendChild(samplerateText);
    root.appendChild(samplerateElement);

    QDomElement inputChannelsElement = doc.createElement("inputChannels");
    QDomText inputChannelsText = doc.createTextNode(QString::number(inputChannels));
    inputChannelsElement.appendChild(inputChannelsText);
    root.appendChild(inputChannelsElement);

    QDomElement outputChannelsElement = doc.createElement("outputChannels");
    QDomText outputChannelsText = doc.createTextNode(QString::number(outputChannels));
    outputChannelsElement.appendChild(outputChannelsText);
    root.appendChild(outputChannelsElement);

    QDomElement channelsElement = doc.createElement("channels");
    QDomText channelsText = doc.createTextNode(QString::number(channels));
    channelsElement.appendChild(channelsText);
    root.appendChild(channelsElement);

    QDomElement microphoneMutedElement = doc.createElement("microphoneMuted");
    QDomText microphoneMutedText = doc.createTextNode(microphoneMuted ? "true" : "false");
    microphoneMutedElement.appendChild(microphoneMutedText);
    root.appendChild(microphoneMutedElement);

    QDomElement frameSizeElement = doc.createElement("frameSize");
    QDomText frameSizeText = doc.createTextNode(QString::number(frameSize));
    frameSizeElement.appendChild(frameSizeText);
    root.appendChild(frameSizeElement);

    // Volume ayarı
    QDomElement volumeElement = doc.createElement("volume");
    QDomText volumeText = doc.createTextNode(QString::number(volume, 'f', 6)); // 6 decimal places for precision
    volumeElement.appendChild(volumeText);
    root.appendChild(volumeElement);

    // XML dosyasını kaydet
    QFile file("settings.xml");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << doc.toString(4); // 4 spaces for indentation
        file.close();
        qDebug() << "Settings saved to XML, volume:" << volume;
    } else {
        qDebug() << "Failed to open settings file for writing";
    }
}

QString SettingsManager::getBroadcastAddress() const
{
    return broadcastAddress;
}

void SettingsManager::setBroadcastAddress(const QString &address)
{
    broadcastAddress = address;
}

QStringList SettingsManager::getListenAddresses() const
{
    return listenAddresses;
}

void SettingsManager::setListenAddresses(const QStringList &addresses)
{
    listenAddresses = addresses;
}

int SettingsManager::getInputDeviceIndex() const
{
    return inputDeviceIndex;
}

void SettingsManager::setInputDeviceIndex(int index)
{
    inputDeviceIndex = index;
}

int SettingsManager::getOutputDeviceIndex() const
{
    return outputDeviceIndex;
}

void SettingsManager::setOutputDeviceIndex(int index)
{
    outputDeviceIndex = index;
}

int SettingsManager::getBitrate() const
{
    return bitrate;
}

void SettingsManager::setBitrate(int newBitrate)
{
    bitrate = newBitrate;
}

int SettingsManager::getFrameSize() const
{
    return frameSize;
}

void SettingsManager::setFrameSize(int newFrameSize)
{
    frameSize = newFrameSize;
}

float SettingsManager::getVolume() const
{
    return volume;
}

void SettingsManager::setVolume(float newVolume)
{
    volume = newVolume;
    qDebug() << "Volume set to:" << volume;
}

int SettingsManager::getSamplerate() const
{
    return samplerate;
}

void SettingsManager::setSamplerate(int newSamplerate)
{
    samplerate = newSamplerate;
    qDebug() << "Samplerate set to:" << samplerate;
}

int SettingsManager::getInputChannels() const
{
    return inputChannels;
}

void SettingsManager::setInputChannels(int channels)
{
    inputChannels = channels;
    qDebug() << "InputChannels set to:" << inputChannels;
}

int SettingsManager::getOutputChannels() const
{
    return outputChannels;
}

void SettingsManager::setOutputChannels(int channels)
{
    outputChannels = channels;
    qDebug() << "OutputChannels set to:" << outputChannels;
}

int SettingsManager::getChannels() const
{
    return channels;
}

void SettingsManager::setChannels(int newChannels)
{
    channels = newChannels;
    qDebug() << "Codec Channels set to:" << channels;
}

bool SettingsManager::isMicrophoneMuted() const
{
    return microphoneMuted;
}

void SettingsManager::setMicrophoneMuted(bool muted)
{
    microphoneMuted = muted;
    qDebug() << "Microphone Muted set to:" << microphoneMuted;
}
