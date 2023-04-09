#ifndef CNETWORKADAPTER_H
#define CNETWORKADAPTER_H
#include <iphlpapi.h>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#pragma comment(lib, "IPHLPAPI.lib")

class CNetworkAdapter : public QWidget
{
    Q_OBJECT
public:
    CNetworkAdapter();

};

#endif // CNETWORKADAPTER_H
