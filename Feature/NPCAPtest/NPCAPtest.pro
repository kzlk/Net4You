QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\npcap-sdk-1.13\Include
LIBS += -LC:\\npcap-sdk-1.13\\Lib\\x64 -lPacket
LIBS += -LC:\\npcap-sdk-1.13\\Lib\\x64 -lwpcap
LIBS += -lIphlpapi
LIBS += -lWs2_32
LIBS += -lBthprops

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    NetworkDevicesWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

