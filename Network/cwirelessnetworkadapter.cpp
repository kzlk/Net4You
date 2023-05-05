#include "cwirelessnetworkadapter.h"
#include "qtimer.h"

CWirelessNetworkAdapter::CWirelessNetworkAdapter()
{
    if (!updateWlanProperties())
        throw std::bad_alloc();

    timer = new QTimer();
}

CWirelessNetworkAdapter::~CWirelessNetworkAdapter()
{
    if (pConnectInfo != NULL)
    {
        WlanFreeMemory(pConnectInfo);
        pConnectInfo = NULL;
    }

    if (pIfList != NULL)
    {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    if (pBssList != NULL)
    {
        WlanFreeMemory(pBssList);
        pBssList = NULL;
    }

    if (pBssEntry != NULL)
    {
        WlanFreeMemory(pBssEntry);
        pBssEntry = NULL;
    }
    if (pConnectInfo != NULL)
    {
        WlanFreeMemory(pConnectInfo);
        pConnectInfo = NULL;
    }

    timer->deleteLater();
}

bool CWirelessNetworkAdapter::updateWlanProperties()
{
    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS)
    {
        qDebug("WlanOpenHandle failed with error: %lu\n", dwResult);
        return false;
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS)
    {
        qDebug("WlanEnumInterfaces failed with error: %lu\n", dwResult);
        return false;
    }

    return true;
}

CWirelessNetworkAdapter::WlanProperties CWirelessNetworkAdapter::getWlanProperties(QString interfaceName)
{
    if (pIfList == nullptr)
        return wlanProperties;

    searchInterface(pIfInfo, interfaceName);

    if (pIfInfo != nullptr)
    {
        if (isWlanStateConnected(pIfInfo))
        {

            wlanProperties.networkType = getNetrworkBSSType(pConnectInfo);
            wlanProperties.ssid = getSSID(pConnectInfo);
            wlanProperties.bssid = getMacAddress(pConnectInfo);
            wlanProperties.authAlgorithm = getAuthAlgo(pConnectInfo);
            wlanProperties.cipherAlgo = getCipherAlgo(pConnectInfo);
            wlanProperties.channelMHz = pBssEntry->ulChCenterFrequency / 1000;
            wlanProperties.transmitRate = pConnectInfo->wlanAssociationAttributes.ulTxRate / 1000;
            wlanProperties.receieveRate = pConnectInfo->wlanAssociationAttributes.ulRxRate / 1000;
        }
    }

    return wlanProperties;
}

bool CWirelessNetworkAdapter::isInterfaceConnectedToWifi(QString &interfaceName)
{
    searchInterface(pIfInfo, interfaceName);
    if (pIfInfo != nullptr)
        return pIfInfo->isState == wlan_interface_state_connected;
    return false;
}

void CWirelessNetworkAdapter::setWlanInterfaceForUpdating(QString &intefaceName)
{
    this->interfaceName = intefaceName;
    disconnect(timer, &QTimer::timeout, this, &CWirelessNetworkAdapter::updateRealTimeValues);
    connect(timer, &QTimer::timeout, this, &CWirelessNetworkAdapter::updateRealTimeValues);
}

void CWirelessNetworkAdapter::startTimer(int interval)
{
    timer->start(interval);
}

void CWirelessNetworkAdapter::stopTimer()
{
    timer->stop();
}

QString CWirelessNetworkAdapter::getSignalLvlFromDBM(int dBm)
{
    if (dBm <= -30 && dBm >= -50)
    {
        return "Excellent";
    }
    else if (dBm <= -51 && dBm >= -60)
    {
        return "Good";
    }
    else if (dBm <= -61 && dBm >= -70)
    {
        return "Fair";
    }
    else if (dBm <= -71 && dBm >= -85)
    {
        return "Poor";
    }
    else
    {
        return "Unreliable";
    }
}

int CWirelessNetworkAdapter::getWifiChannelNumFromMhz(int channel)
{
    if (channel == 2484)
    {
        return 14;
    }
    else if (channel < 2412 || channel > 2484)
    {
        return -1; // Frequency outside 2.4 GHz band
    }
    else
    {
        return (channel - 2407) / 5; // Calculate channel number based on frequency
    }
}

void CWirelessNetworkAdapter::searchInterface(PWLAN_INTERFACE_INFO &info, QString &interfaceName)
{
    if (pIfList == nullptr)
    {
        info = nullptr;
        return;
    }

    for (i = 0; i < pIfList->dwNumberOfItems; i++)
    {
        info = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
        if (QString::fromStdWString(info->strInterfaceDescription) == interfaceName)
        {
            return;
        }
    }

    info = nullptr;
}

bool CWirelessNetworkAdapter::isWlanStateConnected(PWLAN_INTERFACE_INFO &info)
{
    // If interface state is connected, call WlanQueryInterface
    // to get current connection attributes
    if (info->isState == wlan_interface_state_connected)
    {
        dwResult = WlanQueryInterface(hClient, &info->InterfaceGuid, wlan_intf_opcode_current_connection, NULL,
                                      &connectInfoSize, (PVOID *)&pConnectInfo, &opCode);
        if (dwResult != ERROR_SUCCESS)
        {
            qDebug("WlanQueryInterface failed with error: %lu\n", dwResult);
            dwRetVal = 1;
            return false;
        }
        else
        {
            qDebug("WLAN_CONNECTION_ATTRIBUTES getting successfully\n");

            // Get the BSS Entry
            dwResult = WlanGetNetworkBssList(hClient, &info->InterfaceGuid, NULL, dot11_BSS_type_infrastructure, FALSE,
                                             NULL, &pBssList);

            if (dwResult != ERROR_SUCCESS)
            {
                qDebug("WlanGetNetworkBssList getting failed\n");
                return false;
            }

            pBssEntry = &pBssList->wlanBssEntries[i];

            return true;
        }
    };

    return false;
}

QString CWirelessNetworkAdapter::getSSID(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo)
{
    if (pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength == 0)
        return "";
    else
    {
        QString result{};
        for (k = 0; k < pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength; k++)
        {
            result.append((char)pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID[k]);
        }
        return result;
    }
}

QString CWirelessNetworkAdapter::getNetrworkBSSType(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo)
{
    switch (pConnectInfo->wlanAssociationAttributes.dot11BssType)
    {
    case dot11_BSS_type_infrastructure:
        return "Infrastructure";
        break;
    case dot11_BSS_type_independent:
        return "Infrastructure";
        break;
    default:
        return "Other = " + QString::number(pConnectInfo->wlanAssociationAttributes.dot11BssType);
        break;
    }
}

QString CWirelessNetworkAdapter::getMacAddress(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo)
{
    QString result{};
    char buffer[6]{};

    for (k = 0; k < sizeof(pConnectInfo->wlanAssociationAttributes.dot11Bssid); k++)
    {
        if (k == 5)
            sprintf(buffer, "%.2X\0", (int)pConnectInfo->wlanAssociationAttributes.dot11Bssid[k]);
        else
            sprintf(buffer, "%.2X-", (int)pConnectInfo->wlanAssociationAttributes.dot11Bssid[k]);
        result.append(buffer);
    }
    return result;
}

QString CWirelessNetworkAdapter::getAuthAlgo(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo)
{
    switch (pConnectInfo->wlanSecurityAttributes.dot11AuthAlgorithm)
    {
    case DOT11_AUTH_ALGO_80211_OPEN:
        return ("802.11 Open");
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        return ("802.11 Shared");
        break;
    case DOT11_AUTH_ALGO_WPA:
        return ("WPA");
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        return ("WPA-PSK");
        break;
    case DOT11_AUTH_ALGO_WPA_NONE:
        return ("WPA-None");
        break;
    case DOT11_AUTH_ALGO_RSNA:
        return ("RSNA");
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        return ("WPA2 with PSK");
        break;
    default:
        return ("Unknown");
        break;
    }
}

QString CWirelessNetworkAdapter::getCipherAlgo(PWLAN_CONNECTION_ATTRIBUTES &pConnectInfo)
{
    switch (pConnectInfo->wlanSecurityAttributes.dot11CipherAlgorithm)
    {
    case DOT11_CIPHER_ALGO_NONE:
        return ("None");
        break;
    case DOT11_CIPHER_ALGO_WEP40:
        return ("WEP-40");
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        return ("TKIP");
        break;
    case DOT11_CIPHER_ALGO_CCMP:
        return ("CCMP");
        break;
    case DOT11_CIPHER_ALGO_WEP104:
        return ("WEP-104");
        break;
    case DOT11_CIPHER_ALGO_WEP:
        return ("WEP");
        break;
    default:
        return ("Unknown");
        break;
    }
}

void CWirelessNetworkAdapter::updateRealTimeValues()
{
    // if (!this->updateWlanProperties())
    //     throw std::bad_alloc();

    if (pIfList == nullptr)
        return;

    searchInterface(pIfInfo, interfaceName);
    if (pIfInfo != nullptr)
    {
        if (isWlanStateConnected(pIfInfo))
        {
            // Get the BSS Entry
            dwResult = WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid, NULL, dot11_BSS_type_infrastructure,
                                             NULL, NULL, &pBssList);

            if (dwResult != ERROR_SUCCESS)
            {
                qDebug("WlanGetNetworkBssList from updateRealTimeValues() getting failed\n");
                return;
            }

            pBssEntry = &pBssList->wlanBssEntries[i];

            emit updateSignalStrength(pBssEntry->lRssi);
            emit updateChannel(pBssEntry->ulChCenterFrequency);
            emit updateRate(pConnectInfo->wlanAssociationAttributes.ulTxRate,
                            pConnectInfo->wlanAssociationAttributes.ulRxRate);
        }
    }
};
