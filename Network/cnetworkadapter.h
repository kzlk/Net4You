#ifndef CNETWORKADAPTER_H
#define CNETWORKADAPTER_H
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

class CNetworkAdapter : public QWidget {
  Q_OBJECT

private:
  struct NetworkProperties {
    QString networkAdapter{};
    QString interfaceType{};
    QString hardwareAddress{};
    QString connectionName{};
    QString connectionSpeed{};
    QString MTU{};
    QString bytesReceived{};
    QString bytesSent{};
  };

public:
  CNetworkAdapter();
  bool updateDeviceList();
  QStringList getListOfInterface();
  NetworkProperties getNetworkProperties(int index);

private:
  QList<QString> allInterfaceList{};
  PIP_ADAPTER_INFO adapterInfoList{};
  PIP_ADAPTER_INFO adapterInfo{};

  PIP_ADAPTER_ADDRESSES pAddresses{};
  PIP_ADAPTER_ADDRESSES pCurrAddresses{};
  IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer{};
  // default to unspecified address family (both)
  ULONG family = AF_UNSPEC;
  ULONG outBufLen{};
  DWORD dwRetVal = 0;

  //unsigned __int64 inBytes{};
  //unsigned __int64 outBytes{};
  //unsigned __int64 preInBytes{};
  //unsigned __int64 preOutBytes{};
  NetworkProperties adapterProperties{};

private:
  QString getAdapterType(PIP_ADAPTER_INFO &index);
  QString getHardwareAddress(PIP_ADAPTER_INFO &index);
};

#endif // CNETWORKADAPTER_H
