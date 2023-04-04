#include "mainwindow.h"
#include <QMainWindow>
#include <QHeaderView>
#include <QApplication>

int main(int argc, char *argv[])
{
    QByteArray envVarValue = "96";
        qputenv("QT_FONT_DPI", envVarValue);
    QApplication a(argc, argv);
    MainWindow w;




    w.show();
    return a.exec();
}
