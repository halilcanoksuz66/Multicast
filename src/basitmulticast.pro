QT += core gui widgets network xml
LIBS += -lws2_32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audiomanager.cpp \
    codecmanager.cpp \
    debuglogger.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    settingsmanager.cpp

HEADERS += \
    audiomanager.h \
    codecmanager.h \
    debuglogger.h \
    mainwindow.h \
    networkmanager.h \
    settingsmanager.h \
    JitterBuffer.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# ASIO(non-boost)
INCLUDEPATH += C:/msys64/home/halil/asio/include

# PortAudio
INCLUDEPATH += C:/msys64/home/halil/portaudio/include
DEPENDPATH += C:/msys64/home/halil/portaudio/include
LIBS += -L"C:/msys64/home/halil/portaudio/build" -lportaudio

# Opus
INCLUDEPATH += C:/msys64/home/halil/opus-1.5.2/include
LIBS += -L"C:/msys64/home/halil/opus-1.5.2/build" -lopus


DISTFILES +=

# Add this for ASIO
DEFINES += ASIO_STANDALONE
