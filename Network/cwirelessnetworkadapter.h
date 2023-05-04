#ifndef CWIRELESSNETWORKADAPTER_H
#define CWIRELESSNETWORKADAPTER_H
#include "Network/cnetworkadapter.h"
#include <wlanapi.h>
#include <Windot11.h> // for DOT11_SSID struct

// Need to link with Wlanapi.lib and Ole32.lib
// #pragma comment(lib, "wlanapi.lib")
// #pragma comment(lib, "ole32.lib")

class CWirelessNetworkAdapter : public QObject /*: public CNetworkAdapter*/
{
    Q_OBJECT
    struct WlanProperties
    {
        QString networkType{};
        QString ssid{};
        QString bssid{};
        QString authAlgorithm{};
        QString cipherAlgo{};
        int receieveRate{};
        int transmitRate{};
        int signalStrength{};
        int channelNum{};
        int channelMHz{};
        bool isSecurityEnabled{};
    };

  public:
    CWirelessNetworkAdapter();
    ~CWirelessNetworkAdapter();
    bool updateWlanProperties();
    WlanProperties getWlanProperties(QString interfaceName);
    bool isInterfaceConnectedToWifi(QString &interfaceName);
    char *convertGuidToString(GUID interfaceGUID);

    void setWlanInterfaceForUpdating(QString &intefaceName);

    void startTimer(int interval);
    void stopTimer();

    QString getSignalLvlFromDBM(int dBm);
    int getWifiChannelNumFromMhz(int channel);

  private:
    QString interfaceName{};
    QTimer *timer{};

    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;
    unsigned int i, k;

    // variables used for WlanEnumInterfaces

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    PWLAN_BSS_LIST pBssList = NULL;
    PWLAN_BSS_ENTRY pBssEntry = NULL;

    // variables used for WlanQueryInterfaces for opcode = wlan_intf_opcode_current_connection
    PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
    DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
    WLAN_OPCODE_VALUE_TYPE opCode = wlan_opcode_value_type_invalid;

    WlanProperties wlanProperties{};
    void searchInterface(PWLAN_INTERFACE_INFO &info, QString &interfaceName);
    bool isWlanStateConnected(PWLAN_INTERFACE_INFO &info);

    QString getSSID(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo);
    QString getNetrworkBSSType(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo);
    QString getMacAddress(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo);
    QString getPhyNetworkType(PWLAN_CONNECTION_ATTRIBUTES pConnectInfo);
    QString getAuthAlgo(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo);
    QString getCipherAlgo(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo);

  signals:
    void updateRate(int transmitRate, int receiveRate);
    void updateSignalStrength(int signalStrength);
    void updateChannel(int channelMhz);

  public slots:
    void updateRealTimeValues();
};

#endif // CWIRELESSNETWORKADAPTER_H
