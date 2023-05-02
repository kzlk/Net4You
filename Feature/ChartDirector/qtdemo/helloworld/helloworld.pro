# -------------------------------------------------
# QT Project File
# -------------------------------------------------
TARGET = helloworld

TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
} 

SOURCES += qchartviewer.cpp \
    helloworld.cpp

HEADERS += qchartviewer.h

INCLUDEPATH += ../../include

DEFINES += CHARTDIR_HIDE_OBSOLETE _CRT_SECURE_NO_WARNINGS

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
