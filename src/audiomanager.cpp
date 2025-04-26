#include "audiomanager.h"
#include <QDebug>
#include <QMutexLocker>
#include <QMutex>
#include <cmath>

static QMutex audioBufferMutex;

AudioManager::AudioManager(QObject *parent)
    : QObject(parent),
    stream(nullptr)
{
    settingsManager = SettingsManager::getInstance(this);
    selectedInputDevice = settingsManager->getInputDeviceIndex();
    selectedOutputDevice = settingsManager->getOutputDeviceIndex();
    sampleRate = settingsManager->getSamplerate();
    framesPerBuffer = settingsManager->getFrameSize();
    inputChannels = settingsManager->getInputChannels();
    outputChannels = settingsManager->getOutputChannels();
    microphoneMuted = settingsManager->isMicrophoneMuted();
    volume = settingsManager->getVolume();


    PaError err = Pa_Initialize();
    if (err != paNoError) {
        qCritical() << "PortAudio başlatılamadı:" << Pa_GetErrorText(err);
        emit audioError(QString("PortAudio başlatılamadı: %1").arg(Pa_GetErrorText(err)));
        return;
    }

    refreshDeviceLists();

    setDefaultDevices();
}

void AudioManager::setDefaultDevices()
{
    int defaultInputIndex = Pa_GetDefaultInputDevice();
    int defaultOutputIndex = Pa_GetDefaultOutputDevice();

    qDebug() << "Varsayılan giriş cihazı indeksi:" << defaultInputIndex;
    qDebug() << "Varsayılan çıkış cihazı indeksi:" << defaultOutputIndex;

    bool foundInput = false;
    for (int i = 0; i < inputDeviceList.size(); i++) {
        qDebug() << "Checking input device" << i << "name:" << inputDeviceList[i].info->name
                 << "PortAudio index:" << inputDeviceList[i].paDeviceIndex;

        if (inputDeviceList[i].paDeviceIndex == defaultInputIndex) {
            qDebug() << "Found default input device:" << inputDeviceList[i].info->name;
            setInputDevice(i);
            foundInput = true;
            break;
        }
    }

    if (!foundInput && inputDeviceList.size() > 0) {
        qDebug() << "Default input device not found, using first available";
        setInputDevice(0);
    }

    bool foundOutput = false;
    for (int i = 0; i < outputDeviceList.size(); i++) {
        qDebug() << "Checking output device" << i << "name:" << outputDeviceList[i].info->name
                 << "PortAudio index:" << outputDeviceList[i].paDeviceIndex;

        if (outputDeviceList[i].paDeviceIndex == defaultOutputIndex) {
            qDebug() << "Found default output device:" << outputDeviceList[i].info->name;
            setOutputDevice(i);
            foundOutput = true;
            break;
        }
    }

    if (!foundOutput && outputDeviceList.size() > 0) {
        qDebug() << "Default output device not found, using first available";
        setOutputDevice(0);
    }

    qDebug() << "Varsayılan ses cihazları seçildi.";
}

AudioManager::~AudioManager()
{
    if (stream) {
        stopAudioStream();
    }

    PaError err = Pa_Terminate();
    if (err != paNoError) {
        qCritical() << "PortAudio kapatılamadı:" << Pa_GetErrorText(err);
    }
}

bool AudioManager::initialize()
{
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        emit audioError(QString("PortAudio başlatılamadı: %1").arg(Pa_GetErrorText(err)));
        return false;
    }

    refreshDeviceLists();

    settingsManager->setInputDeviceIndex(selectedInputDevice);
    settingsManager->setOutputDeviceIndex(selectedOutputDevice);
    settingsManager->setSamplerate(sampleRate);
    settingsManager->setFrameSize(framesPerBuffer);
    settingsManager->setInputChannels(inputChannels);
    settingsManager->setOutputChannels(outputChannels);
    settingsManager->setMicrophoneMuted(microphoneMuted);

    return true;
}

void AudioManager::terminate()
{
    if (stream) {
        stopAudioStream();
    }

    PaError err = Pa_Terminate();
    if (err != paNoError) {
        qWarning() << "PortAudio kapatılamadı:" << Pa_GetErrorText(err);
    }
}

void AudioManager::refreshDeviceLists()
{
    inputDeviceList.clear();
    outputDeviceList.clear();

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        qCritical() << "Ses cihazları listelenemedi:" << Pa_GetErrorText(numDevices);
        emit audioError(QString("Ses cihazları listelenemedi: %1").arg(Pa_GetErrorText(numDevices)));
        return;
    }

    qDebug() << "Toplam" << numDevices << "ses cihazı bulundu.";

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        qDebug() << "Cihaz" << i << ":" << deviceInfo->name
                 << "- Giriş Kanalları:" << deviceInfo->maxInputChannels
                 << "- Çıkış Kanalları:" << deviceInfo->maxOutputChannels
                 << "- Örnek Hızı:" << deviceInfo->defaultSampleRate;

        if (deviceInfo->maxInputChannels > 0) {
            DeviceInfo device;
            device.info = deviceInfo;
            device.paDeviceIndex = i;
            inputDeviceList.append(device);
        }

        if (deviceInfo->maxOutputChannels > 0) {
            DeviceInfo device;
            device.info = deviceInfo;
            device.paDeviceIndex = i;
            outputDeviceList.append(device);
        }
    }
}

QList<QString> AudioManager::getInputDevices()
{
    QList<QString> devices;
    for (const DeviceInfo &device : inputDeviceList) {
        devices.append(QString(device.info->name));
    }
    return devices;
}

QList<QString> AudioManager::getOutputDevices()
{
    QList<QString> devices;
    for (const DeviceInfo &device : outputDeviceList) {
        devices.append(QString(device.info->name));
    }
    return devices;
}

bool AudioManager::setInputDevice(int deviceIndex)
{
    if (deviceIndex < 0 || deviceIndex >= inputDeviceList.size()) {
        emit audioError("Geçersiz giriş cihazı indeksi");
        return false;
    }

    selectedInputDevice = deviceIndex;
    settingsManager->setInputDeviceIndex(selectedInputDevice);

    inputChannels = inputDeviceList[deviceIndex].info->maxInputChannels;
    settingsManager->setInputChannels(inputChannels);

    qDebug() << "Input device set to:" << inputDeviceList[deviceIndex].info->name
             << "with" << inputChannels << "channels"
             << "at" << sampleRate << "Hz";

    return true;
}

bool AudioManager::setOutputDevice(int deviceIndex)
{
    if (deviceIndex < 0 || deviceIndex >= outputDeviceList.size()) {
        emit audioError("Geçersiz çıkış cihazı indeksi");
        return false;
    }

    selectedOutputDevice = deviceIndex;
    settingsManager->setOutputDeviceIndex(selectedOutputDevice);

    outputChannels = outputDeviceList[deviceIndex].info->maxOutputChannels;
    settingsManager->setOutputChannels(outputChannels);

    qDebug() << "Output device set to:" << outputDeviceList[deviceIndex].info->name
             << "with" << outputChannels << "channels"
             << "at" << sampleRate << "Hz";

    return true;
}

bool AudioManager::startAudioStream()
{
    if (stream) {
        emit audioError("Ses akışı zaten çalışıyor");
        return false;
    }

    if (selectedInputDevice < 0 || selectedOutputDevice < 0) {
        emit audioError("Giriş ve çıkış cihazları seçilmelidir");
        return false;
    }

    int inputDeviceIndex = inputDeviceList[selectedInputDevice].paDeviceIndex;
    int outputDeviceIndex = outputDeviceList[selectedOutputDevice].paDeviceIndex;

    qDebug() << "Using input device index:" << inputDeviceIndex;
    qDebug() << "Using output device index:" << outputDeviceIndex;

    PaStreamParameters inputParameters = {};
    inputParameters.device = inputDeviceIndex;
    inputParameters.channelCount = inputChannels;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = inputDeviceList[selectedInputDevice].info->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParameters = {};
    outputParameters.device = outputDeviceIndex;
    outputParameters.channelCount = outputChannels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = outputDeviceList[selectedOutputDevice].info->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    qDebug() << "Starting audio stream with:"
             << "Input device:" << inputDeviceList[selectedInputDevice].info->name
             << "(" << inputChannels << "channels)"
             << "Output device:" << outputDeviceList[selectedOutputDevice].info->name
             << "(" << outputChannels << "channels)"
             << "Sample rate:" << sampleRate
             << "Frames per buffer:" << framesPerBuffer;

    PaError formatErr = Pa_IsFormatSupported(&inputParameters, &outputParameters, sampleRate);
    if (formatErr != paFormatIsSupported) {
        qCritical() << "Ses formatı desteklenmiyor:" << Pa_GetErrorText(formatErr);

        bool success = false;

        if (inputChannels > 1 || outputChannels > 1) {
            PaStreamParameters monoInput = inputParameters;
            PaStreamParameters monoOutput = outputParameters;

            monoInput.channelCount = 1;
            monoOutput.channelCount = 1;

            formatErr = Pa_IsFormatSupported(&monoInput, &monoOutput, sampleRate);
            if (formatErr == paFormatIsSupported) {
                inputChannels = 1;
                outputChannels = 1;
                inputParameters.channelCount = 1;
                outputParameters.channelCount = 1;
                success = true;
                qDebug() << "Switched to mono channels for compatibility";
            } else {
                qDebug() << "Mono channels not supported either";
            }
        }

        if (!success) {
            emit audioError(QString("Ses formatı desteklenmiyor: %1").arg(Pa_GetErrorText(formatErr)));
            return false;
        }
    }

    PaError err = Pa_OpenStream(
        &stream,
        &inputParameters,
        &outputParameters,
        sampleRate,
        framesPerBuffer,
        paNoFlag,
        &AudioManager::audioCallback,
        this
        );

    if (err != paNoError) {
        qCritical() << "Ses akışı açılamadı:" << Pa_GetErrorText(err);
        emit audioError(QString("Ses akışı açılamadı: %1").arg(Pa_GetErrorText(err)));
        return false;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        Pa_CloseStream(stream);
        stream = nullptr;
        qCritical() << "Ses akışı başlatılamadı:" << Pa_GetErrorText(err);
        emit audioError(QString("Ses akışı başlatılamadı: %1").arg(Pa_GetErrorText(err)));
        return false;
    }

    emit audioStreamStarted();
    return true;
}

void AudioManager::stopAudioStream()
{
    if (!stream) {
        return;
    }

    PaError err = Pa_StopStream(stream);
    if (err != paNoError) {
        qWarning() << "Ses akışı düzgün durdurulamadı:" << Pa_GetErrorText(err);
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        qWarning() << "Ses akışı düzgün kapatılamadı:" << Pa_GetErrorText(err);
    }

    stream = nullptr;
    emit audioStreamStopped();
}

void AudioManager::setMicrophoneMuted(bool muted)
{
    microphoneMuted = muted;
    settingsManager->setMicrophoneMuted(muted);
}

bool AudioManager::isMicrophoneMuted() const
{
    return microphoneMuted;
}

int AudioManager::audioCallback(const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo * /* timeInfo */,
                                PaStreamCallbackFlags /* statusFlags */,
                                void *userData)
{
    AudioManager *self = static_cast<AudioManager*>(userData);
    float *output = static_cast<float*>(outputBuffer);
    int numOutputSamples = framesPerBuffer * self->outputChannels;

    if (inputBuffer && !self->microphoneMuted) {
        const float *input = static_cast<const float*>(inputBuffer);
        int bufferSize = framesPerBuffer * self->inputChannels * sizeof(float);

        QByteArray audioData(reinterpret_cast<const char*>(input), bufferSize);

        QMetaObject::invokeMethod(self, "emitInputAudioReady", Qt::QueuedConnection,
                                  Q_ARG(QByteArray, audioData));
    }

    if (output) {
        memset(output, 0, numOutputSamples * sizeof(float));

        std::vector<float> sourceAudioChunk(numOutputSamples);

        QMutexLocker mapLocker(&self->jitterBufferMapMutex);
        for (auto it = self->jitterBuffers.begin(); it != self->jitterBuffers.end(); ++it) {
            std::shared_ptr<JitterBuffer> jitterBuffer = it.value();
            QString sourceAddress = it.key();

            mapLocker.unlock();

            bool success = jitterBuffer->getNextChunk(sourceAudioChunk.data(), framesPerBuffer, self->outputChannels);

            if (!success) {
                qDebug() << "PLC applied for source:" << sourceAddress;
            }

            for (int i = 0; i < numOutputSamples; ++i) {
                output[i] += sourceAudioChunk[i] * self->volume;
            }

            mapLocker.relock();
        }

        for (int i = 0; i < numOutputSamples; i++) {
            if (output[i] > 1.0f) output[i] = 1.0f;
            if (output[i] < -1.0f) output[i] = -1.0f;
        }
    }

    return paContinue;
}

void AudioManager::emitInputAudioReady(const QByteArray &audioData)
{
    emit inputAudioReady(audioData);
}



void AudioManager::handleOutputAudio(const QByteArray &audioData, const QString &sourceAddress)
{
    std::shared_ptr<JitterBuffer> jitterBuffer;

    {
        QMutexLocker mapLocker(&jitterBufferMapMutex);
        if (!jitterBuffers.contains(sourceAddress)) {
            jitterBuffers[sourceAddress] = std::make_shared<JitterBuffer>(
                50,
                framesPerBuffer,
                sampleRate
                );
            qDebug() << "Created Jitter Buffer for source:" << sourceAddress;
        }
        jitterBuffer = jitterBuffers.value(sourceAddress);
    }

    if (jitterBuffer) {
        jitterBuffer->addPacket(audioData);
    }
}

void AudioManager::setVolume(float newVolume) {
    volume = newVolume;
    settingsManager->setVolume(newVolume);
}
