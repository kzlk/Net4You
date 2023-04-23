#ifndef CNETWORKADAPTER_H
#define CNETWORKADAPTER_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <Windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDateTime>
// For caching
#include <QCache>
// #pragma comment(lib, "ws2_32.lib")
// #pragma comment(lib, "IPHLPAPI.lib")

class CNetworkAdapter : public QWidget
{
    Q_OBJECT

  private:
    struct NetworkProperties
    {
        QString networkAdapter{};
        QString interfaceType{};
        QString hardwareAddress{};
        QString connectionName{};
        uint connectionSpeed{};
        uint MTU{};
        //
        QString bytesReceived{};
        QString bytesSent{};
    };

    struct NetworkAdapterAddreses
    {
        QString IPAddr{};
        QString subnetMask{};
        QString gatewayAddr{};
        QString DHCPAddr{};
        QDateTime DHCPLeaseObtained{};
        QDateTime DHCPLeaseExpires{};
        QString DNSSuffix{};
        QString DNSServers{};
    };

  public:
    CNetworkAdapter();
    bool updateDeviceList();
    QMap<int, QString> getListKeyValueInterface();
    QStringList getListOfInterface();
    NetworkProperties getNetworkProperties(int indexAdapter);
    NetworkAdapterAddreses getNetworkAdapterAddreses(int indexAdapter);

  private:
    static QCache<quint16, PIP_ADAPTER_ADDRESSES> cachedAddresses_;
    QMap<int, QString> allInterfaceKeyValue{};
    QList<QString> allInterfaceList{};
    PIP_ADAPTER_INFO adapterInfoList{};
    PIP_ADAPTER_INFO adapterInfo{};

    PIP_ADAPTER_ADDRESSES pAddresses{};
    PIP_ADAPTER_ADDRESSES pCurrAddresses{};

    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer{};

    PIP_ADAPTER_UNICAST_ADDRESS pUnicast{};

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;
    ULONG outBufLen{};
    DWORD dwRetVal = 0;

    // unsigned __int64 inBytes{};
    // unsigned __int64 outBytes{};
    // unsigned __int64 preInBytes{};
    // unsigned __int64 preOutBytes{};
    NetworkProperties adapterProperties{};
    NetworkAdapterAddreses adapterAddreses{};

    // A set of flags specifying various settings for the adapter.
    bool isDNSEnabled{false};
    bool isRegisterDNSSuffix{false};
    bool isDHCPEnabled{false};
    bool isAdapterIPv4Enabled{false};

  private:
    void getAdapterByIndex(PIP_ADAPTER_ADDRESSES &adapter, int &index);
    QString getAdapterType(PIP_ADAPTER_ADDRESSES &index);
    QString getHardwareAddress(PIP_ADAPTER_ADDRESSES &index);
};

#endif // CNETWORKADAPTER_H
