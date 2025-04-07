#include "audio_codec.h"
#include <QDebug>

AudioCodec::AudioCodec(QObject *parent)
    : QObject(parent)
    , encoder(nullptr)
    , decoder(nullptr)
{
    int error;
    
    // Encoder oluştur
    encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK) {
        qDebug() << "Opus encoder oluşturulamadı:" << opus_strerror(error);
        return;
    }

    // Decoder oluştur
    decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &error);
    if (error != OPUS_OK) {
        qDebug() << "Opus decoder oluşturulamadı:" << opus_strerror(error);
        return;
    }

    qDebug() << "Opus codec başarıyla başlatıldı";
}

AudioCodec::~AudioCodec()
{
    if (encoder) {
        opus_encoder_destroy(encoder);
    }
    if (decoder) {
        opus_decoder_destroy(decoder);
    }
}

std::vector<char> AudioCodec::compress(const std::vector<char>& audioData)
{
    if (!encoder || audioData.empty()) {
        return std::vector<char>();
    }

    // Giriş verisini float32'den opus formatına dönüştür
    std::vector<opus_int16> pcmData(audioData.size() / sizeof(float));
    const float* floatData = reinterpret_cast<const float*>(audioData.data());
    
    for (size_t i = 0; i < pcmData.size(); ++i) {
        // float32'den int16'ya dönüştür
        float sample = floatData[i] * 32768.0f;
        if (sample > 32767.0f) sample = 32767.0f;
        if (sample < -32768.0f) sample = -32768.0f;
        pcmData[i] = static_cast<opus_int16>(sample);
    }

    // Sıkıştırılmış veri için buffer
    std::vector<unsigned char> compressedData(MAX_FRAME_SIZE);
    
    // Sıkıştır
    int compressedSize = opus_encode(encoder,
                                   pcmData.data(),
                                   FRAME_SIZE,
                                   compressedData.data(),
                                   MAX_FRAME_SIZE);

    if (compressedSize < 0) {
        qDebug() << "Sıkıştırma hatası:" << opus_strerror(compressedSize);
        return std::vector<char>();
    }

    // Sıkıştırılmış veriyi döndür
    return std::vector<char>(compressedData.begin(),
                           compressedData.begin() + compressedSize);
}

std::vector<char> AudioCodec::decompress(const std::vector<char>& compressedData)
{
    if (!decoder || compressedData.empty()) {
        return std::vector<char>();
    }

    // Çözülmüş veri için buffer
    std::vector<opus_int16> pcmData(FRAME_SIZE * CHANNELS);
    
    // Çöz
    int frameSize = opus_decode(decoder,
                              reinterpret_cast<const unsigned char*>(compressedData.data()),
                              compressedData.size(),
                              pcmData.data(),
                              FRAME_SIZE,
                              0);

    if (frameSize < 0) {
        qDebug() << "Çözme hatası:" << opus_strerror(frameSize);
        return std::vector<char>();
    }

    // int16'dan float32'ye dönüştür
    std::vector<float> floatData(pcmData.size());
    for (size_t i = 0; i < pcmData.size(); ++i) {
        floatData[i] = pcmData[i] / 32768.0f;
    }

    qDebug() << "Çözülen frame boyutu:" << frameSize << ", toplam örnek:" << frameSize * CHANNELS;

    // float32 veriyi döndür
    return std::vector<char>(
        reinterpret_cast<char*>(floatData.data()),
        reinterpret_cast<char*>(floatData.data() + floatData.size())
        );

} 
