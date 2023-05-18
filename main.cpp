
#include <QMainWindow>
#include <QHeaderView>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "GUI/mainwindow.h"
#include "QDir"
int main(int argc, char *argv[])
{
    QByteArray envVarValue = "96";
    qputenv("QT_FONT_DPI", envVarValue);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/images/images/logo4.png"));

    QTranslator translator;
    if (translator.load("../KURSOVA_SPZ/languages/Ukrainian_ua"))
    {
        a.installTranslator(&translator);
    }
    else
    {
        qDebug() << "Не вдалося завантажити мову";
        qDebug() << "Translation file path:" << QDir::current().absoluteFilePath("languages/Ukrainian_ua.qm");
    }
    MainWindow w;

    w.show();
    return a.exec();
}
