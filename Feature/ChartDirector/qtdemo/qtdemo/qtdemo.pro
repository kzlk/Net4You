# -------------------------------------------------
# QT Project File
# -------------------------------------------------
TARGET = qtdemo

TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
} 

SOURCES += qchartviewer.cpp \
    contourcrosssection.cpp \
    contourzoomscroll.cpp \
    crosshair.cpp \
    financedemo.cpp \
    hotspotdialog.cpp \
    megarealtimezoomscroll.cpp \
    megazoomscroll.cpp \
    qtdemo.cpp \
    democharts.cpp \
    realtimedemo.cpp \
    realtimemultichart.cpp \
    realtimetrack.cpp \
    realtimezoomscroll.cpp \
    simplezoomscroll.cpp \
    threedchartrotation.cpp \
    trackaxis.cpp \
    trackbox.cpp \
    trackfinance.cpp \
    tracklabel.cpp \
    tracklegend.cpp \
    viewportcontroldemo.cpp \
    xyzoomscroll.cpp \
    zoomscrollpdf.cpp \
    zoomscrolltrack.cpp \
    zoomscrolltrack2.cpp
    
HEADERS += qchartviewer.h \
    contourcrosssection.h \
    contourzoomscroll.h \
    crosshair.h \
    financedemo.h \
    hotspotdialog.h \
    megarealtimezoomscroll.h \
    megazoomscroll.h \
    qtdemo.h \
    democharts.h \
    realtimedemo.h \
    realtimemultichart.h \
    realtimetrack.h \
    realtimezoomscroll.h \
    simplezoomscroll.h \
    threedchartrotation.h \
    trackaxis.h \
    trackbox.h \
    trackfinance.h \
    tracklabel.h \
    tracklegend.h \
    viewportcontroldemo.h \
    xyzoomscroll.h \
    zoomscrollpdf.h \
    zoomscrolltrack.h \
    zoomscrolltrack2.h

RESOURCES += qtdemo.qrc
    
INCLUDEPATH += ../../include

DEFINES += CHARTDIR_HIDE_OBSOLETE _CRT_SECURE_NO_WARNINGS

CONFIG += warn_off

win32 {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += ../../lib64/chartdir70.lib
        QMAKE_POST_LINK = copy /Y ..\\..\\lib64\\chartdir70.dll $(DESTDIR)
    } else {
        LIBS += ../../lib32/chartdir70.lib
        QMAKE_POST_LINK = copy /Y ..\\..\\lib32\\chartdir70.dll $(DESTDIR)
    }
}

macx:LIBS += -L../../lib -lchartdir
macx:QMAKE_RPATHDIR += @executable_path/../Frameworks
macx:QMAKE_POST_LINK += mkdir -p \"`dirname $(TARGET)`/../Frameworks\";
macx:QMAKE_POST_LINK += cp ../../lib/libchartdir.7.dylib \"`dirname $(TARGET)`/../Frameworks\";

unix:!macx:LIBS += -L../../lib -lchartdir
unix:!macx:QMAKE_RPATHDIR += ../../lib
