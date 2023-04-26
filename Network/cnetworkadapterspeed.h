#ifndef CNETWORKADAPTERSPEED_H
#define CNETWORKADAPTERSPEED_H
#include <QObject>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <Iphlpapi.h>

class CNetworkAdapterSpeed : public QObject
{

  public:
    CNetworkAdapterSpeed();

  private:
    int adapterIndex{};

  signals:
    void networkSpeedChangeg(int download, int upload);
};

#endif // CNETWORKADAPTERSPEED_H
