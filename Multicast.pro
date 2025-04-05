QT       += core gui

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
    main.cpp \
    mainwindow.cpp \
    multicast_receiver.cpp \
    multicast_sender.cpp

HEADERS += \
    mainwindow.h \
    multicast_receiver.h \
    multicast_sender.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += C:/msys64/home/halil/asio/include  # ASIO başlık dosyalarının bulunduğu dizin

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
