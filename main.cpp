
#include <QMainWindow>
#include <QHeaderView>
#include <QApplication>
#include "GUI/mainwindow.h"

int main(int argc, char *argv[])
{
    QByteArray envVarValue = "96";
        qputenv("QT_FONT_DPI", envVarValue);
    QApplication a(argc, argv);
    MainWindow w;




    w.show();
    return a.exec();
}
