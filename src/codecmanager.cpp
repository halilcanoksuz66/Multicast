#include "codecmanager.h"
#include <opus.h>
#include <QDebug>
#include <QMutexLocker>
#include <QMutex>

CodecManager::CodecManager(QObject *parent)
    : QObject(parent),
    encoder(nullptr),
    application(OPUS_APPLICATION_VOIP),
    useFEC(true)
{
    settingsManager = SettingsManager::getInstance(this);
    bitrate = settingsManager->getBitrate();
    frameSize = settingsManager->getFrameSize();
    sampleRate = settingsManager->getSamplerate();
    channels = settingsManager->getChannels();
}

CodecManager::~CodecManager()
{
    terminate();
}

bool CodecManager::initialize()
{
    // Zaten başlatılmış mı kontrol et
    if (encoder) {
        terminate();
    }

    int error;

    // Opus encoder oluştur
    qDebug() << "Creating Opus encoder with rate:" << sampleRate << "channels:" << channels;
    encoder = opus_encoder_create(sampleRate, channels, application, &error);
    if (error != OPUS_OK) {
        qCritical() << "Opus encoder oluşturulamadı:" << opus_strerror(error);
        emit codecError(QString("Opus encoder oluşturulamadı: %1").arg(opus_strerror(error)));
        return false;
    }

    // Opus ayarla
    // Düzeltme: 16kHz için örnek değerlerini optimize et
    if (sampleRate == 16000) {
        frameSize = 160; // 10ms @ 16kHz
        bitrate = 24000; // Düşük bit hızı
    } else if (sampleRate == 24000) {
        frameSize = 240; // 10ms @ 24kHz
        bitrate = 32000; // Orta bit hızı
    } else if (sampleRate == 48000) {
        frameSize = 480; // 10ms @ 48kHz
        bitrate = 64000; // Yüksek bit hızı
    }

    // Kanal sayısına göre bit hızını ayarla
    if (channels == 1) {
        bitrate = bitrate / 2; // Mono için bit hızını yarıya düşür
    }

    // Encoder ayarlarını yapılandır
    opus_encoder_ctl(encoder, OPUS_SET_BITRATE(bitrate)); // Sesin kalitesi..
    opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(8)); // Daha düşük işlemci yükü
    opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(useFEC ? 1 : 0)); // Paket kaybı olsa bile ses daha kaliteli olacak.
    opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(5)); // %5 kayıp varsayımı ile paket kayıplarını telafi etmeye.
    opus_encoder_ctl(encoder, OPUS_SET_DTX(1)); // Sessizlikte veri göndermemesi için kullanılır.
    opus_encoder_ctl(encoder, OPUS_SET_VBR(1)); // Sessizlikte düşük bitrate, karmaşık seslerde yüksek bitrate otomatik olarak ayarlanır.
    opus_encoder_ctl(encoder, OPUS_BANDWIDTH_WIDEBAND); // Konuşma için daha az freakans genişliği kullanıyor. Ağ için iyi.
    opus_encoder_ctl(encoder, OPUS_SET_APPLICATION(OPUS_APPLICATION_VOIP)); // Konuşma anlaşılabilirliği için
    opus_encoder_ctl(encoder, OPUS_SET_LSB_DEPTH(16)); // 16-bit  pcm seslerde




    // Ayarları kaydet
    settingsManager->setSamplerate(sampleRate);
    settingsManager->setChannels(channels);
    settingsManager->setBitrate(bitrate);
    settingsManager->setFrameSize(frameSize);

    qDebug() << "Opus encoder initialized with:"
             << "Sample rate:" << sampleRate
             << "Channels:" << channels
             << "Bitrate:" << bitrate
             << "Frame size:" << frameSize;

    return true;
}

void CodecManager::terminate()
{
    // Encoder'ı serbest bırak
    if (encoder) {
        opus_encoder_destroy(encoder);
        encoder = nullptr;
    }

    // Decoder'ları serbest bırak
    for (auto it = decoders.begin(); it != decoders.end(); ++it) {
        if (it.value()) {
            opus_decoder_destroy(it.value());
        }
    }
    decoders.clear();
}

void CodecManager::configureBitrate(int newBitrate)
{
    if (newBitrate <= 0) {
        emit codecError(QString("Geçersiz bit hızı: %1").arg(newBitrate));
        return;
    }

    bitrate = newBitrate;
    settingsManager->setBitrate(bitrate);

    if (encoder) {
        int error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(bitrate));
        if (error != OPUS_OK) {
            emit codecError(QString("Bit hızı ayarlanamadı: %1").arg(opus_strerror(error)));
        }
    }
}

void CodecManager::configureFrameSize(int newFrameSize)
{
    // İki önemli şartımız var:
    // 1. frameSize, örnek hızı ile tutarlı olmalı (yani 10ms'lik segment olmalı)
    // 2. Opus kodek belirli değerleri destekler

    // Önce 10ms için doğru değeri hesapla
    int correctFrameSize = sampleRate / 100; // 10ms = 1/100 saniye

    // Eğer verilen değer 10ms'ye yaklaşık değilse
    if (abs(newFrameSize - correctFrameSize) > 10) {
        qDebug() << "Frame size adjusted from" << newFrameSize << "to" << correctFrameSize;
        newFrameSize = correctFrameSize;
    }

    frameSize = newFrameSize;
    settingsManager->setFrameSize(frameSize);
    qDebug() << "Frame size set to" << frameSize << "samples ("
             << (frameSize * 1000 / sampleRate) << "ms at" << sampleRate << "Hz)";
}

// Ses örnek hızını ayarla
void CodecManager::setSampleRate(int newSampleRate)
{
    // Opus, sadece belirli örnek hızlarını destekler
    if (newSampleRate != 8000 && newSampleRate != 12000 &&
        newSampleRate != 16000 && newSampleRate != 24000 &&
        newSampleRate != 48000) {
        qWarning() << "Invalid sample rate:" << newSampleRate
                   << "- Using closest valid rate";

        // En yakın geçerli değeri bul
        if (newSampleRate < 12000) {
            newSampleRate = 8000;
        } else if (newSampleRate < 16000) {
            newSampleRate = 12000;
        } else if (newSampleRate < 24000) {
            newSampleRate = 16000;
        } else if (newSampleRate < 48000) {
            newSampleRate = 24000;
        } else {
            newSampleRate = 48000;
        }
    }

    // Eğer değişiklik varsa
    if (sampleRate != newSampleRate) {
        sampleRate = newSampleRate;
        settingsManager->setSamplerate(sampleRate);

        // Frame size'ı güncelle (10ms'lik dilim için)
        configureFrameSize(sampleRate / 100);

        // Encoder varsa yeniden başlat
        if (encoder) {
            initialize();
        }

        qDebug() << "Sample rate set to" << sampleRate << "Hz";
    }
}

// Kanal sayısını ayarla (AudioManager'dan gelen kanal sayısına göre)
void CodecManager::setChannels(int channelCount)
{
    if (channelCount != 1 && channelCount != 2) {
        qWarning() << "Invalid channel count:" << channelCount << "- Using mono";
        channelCount = 1;
    }

    // Kanal sayısı değiştiyse encoder'ı yeniden başlat
    if (channels != channelCount) {
        channels = channelCount;
        settingsManager->setChannels(channels);

        // Kanal sayısına göre bit hızını ayarla
        if (channels == 1) {
            bitrate = bitrate / 2; // Mono için bit hızını yarıya düşür
        } else {
            bitrate = bitrate * 2; // Stereo için bit hızını iki katına çıkar
        }
        settingsManager->setBitrate(bitrate);

        // Aktif encoder varsa yeniden başlat
        if (encoder) {
            initialize();
        }

        // Decoder'ları temizle (yeniden oluşturulacaklar)
        for (auto it = decoders.begin(); it != decoders.end(); ++it) {
            if (it.value()) {
                opus_decoder_destroy(it.value());
            }
        }
        decoders.clear();

        qDebug() << "Codec channels set to:" << channels << "with bitrate:" << bitrate;
    }
}

void CodecManager::encodeAudioData(const QByteArray &rawAudioData)
{
    if (!encoder) {
        if (!initialize()) {
            return;
        }
    }

    // Gelen ses verisinin kanal sayısına göre hesaplama
    int audioSamples = rawAudioData.size() / sizeof(float); // toplam örnek sayısı
    int expectedChannels = audioSamples / frameSize; // toplam örnek sayısı / frame size = kanal sayısı

    // Uyumsuzluk varsa
    if (expectedChannels != channels) {
        qDebug() << "Input audio has" << expectedChannels << "channels, but codec configured for"
                 << channels << "channels - adjusting";
        setChannels(expectedChannels);

        // Eğer encoder tekrar başlatılması başarısız olursa
        if (!encoder) {
            qCritical() << "Failed to reinitialize encoder after channel change";
            return;
        }
    }

    // Float32 formatından Int16 formatına dönüştür (Opus için)
    const float *floatData = reinterpret_cast<const float*>(rawAudioData.constData());
    int numSamples = rawAudioData.size() / sizeof(float);

    // Doğru boyutta olduğunu kontrol et
    if (numSamples != frameSize * channels) {
        qWarning() << "Unexpected sample count:" << numSamples
                   << "expected:" << frameSize * channels
                   << "channels:" << channels
                   << "frame size:" << frameSize;
        return;
    }

    QByteArray pcm(numSamples * sizeof(opus_int16), Qt::Uninitialized);
    opus_int16 *pcmData = reinterpret_cast<opus_int16*>(pcm.data());

    for (int i = 0; i < numSamples; i++) {
        float sample = floatData[i];

        // Limit to [-1.0, 1.0] range
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;

        // Convert to int16
        pcmData[i] = static_cast<opus_int16>(sample * 32767.0f);
    }

    // Encode
    QByteArray encodedData(4000, Qt::Uninitialized); // Max Opus packet size
    int encodedBytes = opus_encode(encoder, pcmData, frameSize,
                                   reinterpret_cast<unsigned char*>(encodedData.data()),
                                   encodedData.size());

    if (encodedBytes < 0) {
        qCritical() << "Kodlama hatası:" << opus_strerror(encodedBytes);
        emit codecError(QString("Kodlama hatası: %1").arg(opus_strerror(encodedBytes)));
        return;
    }

    qDebug() << "Encoded" << numSamples << "samples into" << encodedBytes << "bytes";

    // Resize to actual encoded size
    encodedData.resize(encodedBytes);

    // Emit encoded data
    emit encodedDataReady(encodedData);
}

void CodecManager::decodeAudioData(const QByteArray &encodedAudioData, const QString &sourceAddress)
{
    // Thread güvenliği
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    // Gelen kaynak için decoder var mı kontrol et
    if (!decoders.contains(sourceAddress)) {
        int error;
        OpusDecoder *decoder = opus_decoder_create(sampleRate, channels, &error);

        if (error != OPUS_OK) {
            qCritical() << "Opus decoder oluşturulamadı:" << opus_strerror(error);
            emit codecError(QString("Opus decoder oluşturulamadı: %1").arg(opus_strerror(error)));
            return;
        }

        decoders[sourceAddress] = decoder;
    }

    OpusDecoder *decoder = decoders[sourceAddress];

    // Decode
    QByteArray pcm(frameSize * channels * sizeof(opus_int16), Qt::Uninitialized);
    opus_int16 *pcmData = reinterpret_cast<opus_int16*>(pcm.data());

    int decodedSamples = opus_decode(decoder,
                                     reinterpret_cast<const unsigned char*>(encodedAudioData.constData()),
                                     encodedAudioData.size(),
                                     pcmData,
                                     frameSize,
                                     0); // Use FEC if packet lost (0 = normal decoding)

    if (decodedSamples < 0) {
        qCritical() << "Çözme hatası:" << opus_strerror(decodedSamples);
        emit codecError(QString("Çözme hatası: %1").arg(opus_strerror(decodedSamples)));
        return;
    }

    // Int16 formatından Float32 formatına dönüştür (PortAudio için)
    QByteArray floatData(decodedSamples * channels * sizeof(float), Qt::Uninitialized);
    float *floatPtr = reinterpret_cast<float*>(floatData.data());

    for (int i = 0; i < decodedSamples * channels; i++) {
        floatPtr[i] = pcmData[i] / 32767.0f;
    }

    qDebug() << "Decoded" << encodedAudioData.size() << "bytes into"
             << decodedSamples << "samples (" << decodedSamples * channels << "total values)";

    // Emit decoded data
    emit decodedDataReady(floatData, sourceAddress);
}
