#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <QObject>
#include <vector>
#include <opus.h>

class AudioCodec : public QObject
{
    Q_OBJECT

public:
    explicit AudioCodec(QObject *parent = nullptr);
    ~AudioCodec();

    // Ses verisini sıkıştır
    std::vector<char> compress(const std::vector<char>& audioData);
    
    // Sıkıştırılmış veriyi çöz
    std::vector<char> decompress(const std::vector<char>& compressedData);

private:
    OpusEncoder* encoder;
    OpusDecoder* decoder;
    
    static const int SAMPLE_RATE = 48000;  // Opus için standart örnek hızı
    static const int CHANNELS = 1;
    static const int FRAME_SIZE = 480;     // 10ms @ 48kHz
    static const int MAX_FRAME_SIZE = 6 * 960; // Opus için maksimum frame boyutu
};

#endif // AUDIO_CODEC_H 
