QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Windows için ASIO yapılandırması
win32 {
    DEFINES += WIN32_LEAN_AND_MEAN
    DEFINES += _WIN32_WINNT=0x0601
    LIBS += -lws2_32
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audiocapture.cpp \
    main.cpp \
    mainwindow.cpp \
    multicast_manager.cpp \
    multicast_receiver.cpp \
    multicast_sender.cpp \
    audio_codec.cpp

HEADERS += \
    audiocapture.h \
    mainwindow.h \
    multicast_manager.h \
    multicast_receiver.h \
    multicast_sender.h \
    audio_codec.h

FORMS += \
    mainwindow.ui

# ASIO(non-boost)
INCLUDEPATH += C:/msys64/home/halil/asio/include

# PortAudio
INCLUDEPATH += C:/msys64/home/halil/portaudio/include
DEPENDPATH += C:/msys64/home/halil/portaudio/include
LIBS += -L"C:/msys64/home/halil/portaudio/build" -lportaudio

# Opus
INCLUDEPATH += C:/msys64/home/halil/opus-1.5.2/include
LIBS += -L"C:/msys64/home/halil/opus-1.5.2/build" -lopus

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
