#ifndef JITTERBUFFER_H
#define JITTERBUFFER_H

#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include <QVector>
#include <QDebug>

class JitterBuffer {
public:
    // Hedef buffer derinliğini ms cinsinden ayarlar
    explicit JitterBuffer(int targetLatencyMs = 50, int frameSizeSamples = 480, int sampleRate = 48000)
        : m_targetLatencyMs(targetLatencyMs),
        m_frameSizeSamples(frameSizeSamples),
        m_sampleRate(sampleRate),
        m_packetDurationMs((frameSizeSamples * 1000.0) / sampleRate),
        m_maxQueueDepth(qMax(2, (targetLatencyMs * 2) / (int)m_packetDurationMs)) // Hedefin 2 katı kadar en fazla
    {
        m_lastPlayedPacket.resize(frameSizeSamples * sizeof(float));
        memset(m_lastPlayedPacket.data(), 0, m_lastPlayedPacket.size()); // Başlangıçta sessizlik
    }

    // Gelen decode edilmiş ses paketini ekler (float verisi içeren QByteArray)
    // Bu fonksiyon farklı bir thread'den çağrılabilir (örn. Network/Codec thread'i)
    void addPacket(const QByteArray& decodedFloatPacket) {
        QMutexLocker locker(&m_mutex);
        if (m_packetQueue.size() >= m_maxQueueDepth) {
            // Çok fazla birikti, eski paketi atla (gecikmeyi azaltmak için)
            m_packetQueue.dequeue();
            qDebug() << "Jitter buffer overflow, dropping oldest packet.";
        }
        m_packetQueue.enqueue(decodedFloatPacket);
        // İsteğe bağlı: Paket varış zamanını veya sıra numarasını saklayabilirsiniz
    }

    // Ses callback'i tarafından çağrılır. İstenen sayıda sample için ses verisi döndürür.
    // outputBuffer: Doldurulacak float buffer
    // framesRequested: İstenen sample sayısı (genellikle frameSizeSamples ile aynı olmalı)
    // channels: Kanal sayısı (çıktı buffer'ının boyutunu etkiler)
    // return: Veri başarıyla alındıysa true, PLC uygulandıysa false
    bool getNextChunk(float* outputBuffer, int framesRequested, int channels) {
        QMutexLocker locker(&m_mutex);

        // Boyut kontrolü
        int requestedBytes = framesRequested * channels * sizeof(float);
        if (requestedBytes <= 0) return false;


        bool packetAvailable = !m_packetQueue.isEmpty();

        // TODO: Daha gelişmiş logic eklenebilir:
        // - Eğer sıra numarası kullanılıyorsa, sıradaki paketin olup olmadığını kontrol et.
        // - Buffer doluluğunu hedef latency ile karşılaştır. Çok azsa bekle (PLC), çok fazlaysa atla.

        if (packetAvailable) {
            // Sıradaki paketi al
            QByteArray packet = m_packetQueue.dequeue();

            if (packet.size() == requestedBytes) {
                memcpy(outputBuffer, packet.constData(), requestedBytes);
                // Son başarılı paketi sakla (PLC için)
                m_lastPlayedPacket = packet;
                return true;
            } else {
                qWarning() << "Jitter Buffer: Packet size mismatch. Expected:" << requestedBytes << "Got:" << packet.size();
                // Boyut uyuşmuyorsa PLC uygula
                performPLC(outputBuffer, requestedBytes);
                return false;
            }

        } else {
            // Paket yok, PLC uygula
            qDebug() << "Jitter buffer empty, performing PLC.";
            performPLC(outputBuffer, requestedBytes);
            return false;
        }
    }

    int getCurrentQueueDepth() const {
        QMutexLocker locker(&m_mutex);
        return m_packetQueue.size();
    }

private:
    // Paket Kaybı Gizleme (Packet Loss Concealment - PLC)
    // Basit strateji: Son başarılı paketi tekrar oynat veya sessizlik ekle
    void performPLC(float* outputBuffer, int bytesToFill) {
        // Strateji 1: Son başarılı paketi tekrar oynat
        if (m_lastPlayedPacket.size() == bytesToFill) {
            memcpy(outputBuffer, m_lastPlayedPacket.constData(), bytesToFill);
        } else {
            // Strateji 2: Sessizlik ekle (fallback)
            memset(outputBuffer, 0, bytesToFill);
        }
    }

    mutable QMutex m_mutex; // Kuyruk erişimini korumak için
    QQueue<QByteArray> m_packetQueue;
    QByteArray m_lastPlayedPacket; // PLC için son başarılı paket

    int m_targetLatencyMs;   // Hedeflenen buffer derinliği (ms)
    int m_frameSizeSamples;  // Bir paketteki sample sayısı
    int m_sampleRate;        // Örnekleme hızı
    double m_packetDurationMs; // Bir paketin süresi (ms)
    int m_maxQueueDepth;     // Kuyruğun maksimum boyutu (paket sayısı)
};

#endif // JITTERBUFFER_H
