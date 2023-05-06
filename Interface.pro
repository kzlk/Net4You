QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
LIBS += -lIphlpapi
LIBS += -lws2_32
LIBS += -lwlanapi
LIBS += -lole32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Network/cmaincontrolblock.cpp \
    Network/cnetworkadapter.cpp \
    Network/cnetworkadapterspeed.cpp \
    Network/croutetable.cpp \
    Network/cviewcontextmenu.cpp \
    Network/cwirelessnetworkadapter.cpp \
   main.cpp \
     GUI/mainwindow.cpp

HEADERS += \
     GUI/CustomGrip.h \
     GUI/Settings.h \
     GUI/Widget.h \
     GUI/mainwindow.h \
     Network/cmaincontrolblock.h \
     Network/cnetworkadapter.h \
     Network/cnetworkadapterspeed.h \
     Network/croutetable.h \
     Network/cviewcontextmenu.h \
     Network/cwirelessnetworkadapter.h

FORMS += \
     GUI/main.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
