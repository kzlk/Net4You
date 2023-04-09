

#include "NetworkDevicesWidget.h"
#include <QApplication>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    NetworkDevicesWidget networkDevicesWidget;
    networkDevicesWidget.show();
    return app.exec();}
