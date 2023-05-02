#include "cwirelessnetworkadapter.h"

CWirelessNetworkAdapter::CWirelessNetworkAdapter()
{
    if (!updateWlanProperties())
        throw std::bad_alloc();
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
            dwResult =
                WlanGetNetworkBssList(hClient, &info->InterfaceGuid, &pConnectInfo->wlanAssociationAttributes.dot11Ssid,
                                      dot11_BSS_type_infrastructure, TRUE, NULL, &pBssList);

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
};
