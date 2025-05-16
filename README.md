
<h1 align="center">🎙️ Multicast Voice Communication App</h1>
<p align="center">
  <b>C++ • Qt • PortAudio • Opus • ASIO</b><br>
  Gerçek zamanlı multicast üzerinden sesli iletişim uygulaması
</p>

---

## 📸 Ekran Görüntüsü

> (Buraya GUI ekran görüntüsü eklenebilir - placeholder)

---

## ✨ Özellikler

- 🎧 Gerçek zamanlı sesli iletişim (low-latency)
- 🖼️ Kullanıcı dostu arayüz (Qt GUI)
- 📡 Multicast UDP yayın desteği
- 🧠 Jitter Buffer ile gecikme önleyici algoritma
- 🔊 Opus codec ile yüksek kaliteli ses sıkıştırma
- 📜 Basit ve özelleştirilebilir log sistemi

---

## 🔧 Kullanılan Teknolojiler

| Teknoloji     | Açıklama                                  |
|---------------|-------------------------------------------|
| `C++17`       | Temel programlama dili                    |
| `Qt Widgets`  | GUI oluşturmak için                      |
| `PortAudio`   | Cihazdan gerçek zamanlı ses alımı         |
| `Opus Codec`  | Ses sıkıştırma ve codec yönetimi         |
| `ASIO`        | Düşük gecikmeli soket haberleşmesi       |

---

## 🚀 Kurulum Adımları

### 1. Gerekli Kütüphaneleri Kur

- Qt (5 veya 6)
- PortAudio
- Opus Codec
- ASIO SDK

### 2. Projeyi Qt Creator ile Aç

```bash
src/basitmulticast.pro  # Qt Creator üzerinden açılır
```

### 3. Derleme Ayarları

`.pro` dosyasına aşağıdaki dizinler uygun şekilde eklenmeli:

```pro
INCLUDEPATH += path/to/portaudio path/to/opus
LIBS += -lportaudio -lopus -lwinmm -lole32 -lws2_32
```

---

## 🎮 Kullanım Senaryosu

1. Uygulama başlatılır.
2. Gönderici multicast adresine ses verisi yollar.
3. Alıcı, bu adresi dinleyerek sesi çözümler.
4. GUI üzerinden ses başlat/durdur, IP ayarı gibi işlemler yapılır.

---

## 🗂️ Proje Yapısı

```
Multicast-main/
├── docs/                # Önceki dökümantasyonlar
├── src/                 # Ana kaynak kodlar
│   ├── audiomanager.*     # PortAudio üzerinden ses yönetimi
│   ├── codecmanager.*     # Opus ile codec işlemleri
│   ├── debuglogger.*      # Log sistemi
│   ├── JitterBuffer.h     # Gecikme önleyici buffer
│   ├── mainwindow.*       # Qt GUI dosyaları
│   ├── main.cpp           # Uygulama girişi
│   └── basitmulticast.pro # Qt Proje Dosyası
```

---

## 🧪 Örnek Kod Parçası

```cpp
// Opus ile kodlama
int encoded_bytes = opus_encode(encoder, input_buffer, FRAME_SIZE, output_buffer, MAX_PACKET_SIZE);

// Multicast gönderimi
socket.send_to(boost::asio::buffer(output_buffer, encoded_bytes), multicast_endpoint);
```

---
