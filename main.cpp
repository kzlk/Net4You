#include "mainwindow.h"
#include <QMainWindow>
#include <QHeaderView>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QByteArray envVarValue = "96";
        qputenv("QT_FONT_DPI", envVarValue);


    w.show();
    return a.exec();
}
