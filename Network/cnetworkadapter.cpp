#include "cnetworkadapter.h"

CNetworkAdapter::CNetworkAdapter()
{
    if (!updateDeviceList())
        throw std::bad_alloc();
}

bool CNetworkAdapter::updateDeviceList()
{
    if (pAddresses != nullptr)
        free(pAddresses);

    outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
    pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);

    if (GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAddresses);
        pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
    }

    // Make an initial call to GetAdaptersAddresses to get the
    // size needed into the outBufLen variable

    if (GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAddresses);
        pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
    }

    qDebug("Memory allocated for GetAdapterAddresses = %lu bytes\n", outBufLen);

    if (pAddresses == NULL)
    {
        qDebug("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
        return false;
    }

    dwRetVal = GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);

    return dwRetVal == NO_ERROR;
}

QStringList CNetworkAdapter::getListOfInterface()
{
    pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {
        if (pCurrAddresses->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
        {
            pCurrAddresses = pCurrAddresses->Next;
            continue;
        }
        else
        {
            allInterfaceList.emplace_back(QString::fromStdWString(pCurrAddresses->Description));
            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    return allInterfaceList;
}

CNetworkAdapter::NetworkProperties CNetworkAdapter::getNetworkProperties(int index)
{
}

QString CNetworkAdapter::getAdapterType(PIP_ADAPTER_INFO &index)
{
    switch (index->Type)
    {
    case IF_TYPE_OTHER:
        return "Other";
        break;
    case IF_TYPE_ETHERNET_CSMACD:
        if (strstr(index->Description, "Bluetooth") != NULL)
            return "Bluetooth Ethernet";
        return "Ethernet";
        break;
    case IF_TYPE_ISO88025_TOKENRING:
        return "Token Ring";
        break;
    case IF_TYPE_PPP:
        return "PPP";
        break;
    case IF_TYPE_SOFTWARE_LOOPBACK:
        return ("Software Lookback");
        break;
    case IF_TYPE_ATM:
        return ("ATM");
        break;
    case IF_TYPE_IEEE80211:
        return ("IEEE 802.11 Wireless");
        break;
    case IF_TYPE_TUNNEL:
        return ("Tunnel type encapsulation");
        break;
    case IF_TYPE_IEEE1394:
        return ("IEEE 1394 Firewire");
        break;
    default:
        return "Unknown type " + QString::number(index->Type);
        break;
    }
}

QString CNetworkAdapter::getHardwareAddress(PIP_ADAPTER_INFO &index)
{
    if (index->AddressLength == 0)
        return "";

    QString address{};
    char buffer[3]{};

    for (int j = 0; j < (int)index->AddressLength; j++)
    {
        for (j = 0; j < (int)index->AddressLength; j++)
        {
            if (j == ((int)index->AddressLength - 1))
                sprintf(buffer, "%.2X\0", (int)index->Address[j]);
            else
                sprintf(buffer, "%.2X-", (int)index->Address[j]);
            address.append(buffer);
        }
    }
}
