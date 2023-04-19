#ifndef CMAINCONTROLBLOCK_H
#define CMAINCONTROLBLOCK_H
#include "ui_main.h"
#include <QMainWindow>
#include <QObject>
#include  "cnetworkadapter.h"
class CMainControlBlock
{
public:
    CMainControlBlock(Ui::MainWindow* main);

private:
    void setupInterfaceInfo();
    Ui::MainWindow* ui;
   // CNetworkAdapter* adapter = new CNetworkAdapter();


};

#endif // CMAINCONTROLBLOCK_H
