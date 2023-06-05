QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


LIBS += -lIphlpapi
LIBS += -lws2_32
LIBS += -lwlanapi



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000   # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GUI/aboutdialog.cpp \
    GUI/helpwidget.cpp \
    GUI/speedwidget.cpp \
    Graph/qchartimage.cpp \
    Network/cmaincontrolblock.cpp \
    Network/cnetworkadapter.cpp \
    Network/cnetworkadapterspeed.cpp \
    Network/cpaintnetworkgraphic.cpp \
    Network/croutetable.cpp \
    Network/cviewcontextmenu.cpp \
    Network/cwirelessnetworkadapter.cpp \
    SystemTray/capptrayicon.cpp \
   main.cpp \
     GUI/mainwindow.cpp\
     Translator/ctranslateapp.cpp

HEADERS += \
     GUI/CustomGrip.h \
     GUI/Settings.h \
     GUI/Widget.h \
     GUI/aboutdialog.h \
     GUI/helpwidget.h \
     GUI/mainwindow.h \
     GUI/speedwidget.h \
     Graph/qchartimage.h \
     Network/cmaincontrolblock.h \
     Network/cnetworkadapter.h \
     Network/cnetworkadapterspeed.h \
     Network/cpaintnetworkgraphic.h \
     Network/croutetable.h \
     Network/cviewcontextmenu.h \
     Network/cwirelessnetworkadapter.h \
     SystemTray/capptrayicon.h \
     Translator/ctranslateapp.h

FORMS += \
     GUI/aboutdialog.ui \
     GUI/helpwidget.ui \
     GUI/main.ui \
     GUI/speedwidget.ui

DEFINES += CHARTDIR_HIDE_OBSOLETE _CRT_SECURE_NO_WARNINGS

CONFIG += warn_off

INCLUDEPATH += ../KURSOVA_SPZ/GraphLibrary/include

TRANSLATIONS += languages/Ukrainian_ua.ts

win32 {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += ../KURSOVA_SPZ/GraphLibrary/lib64/chartdir70.lib
        QMAKE_POST_LINK = copy /Y ..\\KURSOVA_SPZ\\GraphLibrary\\lib64\\chartdir70.dll $(DESTDIR)
    } else {
        LIBS += ../KURSOVA_SPZ/GraphLibrary/lib32/chartdir70.lib
        QMAKE_POST_LINK = copy /Y ..\\KURSOVA_SPZ\\GraphLibrary\\lib32\\chartdir70.dll $(DESTDIR)
    }
}

RESOURCES += \
    resources.qrc

RC_ICONS = appicon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/icons/logo4 (Custom).png


