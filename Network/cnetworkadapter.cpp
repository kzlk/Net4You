#include "cnetworkadapter.h"

CNetworkAdapter::CNetworkAdapter()
{
    if (!updateDeviceList())
        throw std::bad_alloc();
    cachedAddresses_.setMaxCost(10);
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

QMap<int, QString> CNetworkAdapter::getListKeyValueInterface()
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
            allInterfaceKeyValue.insert(pCurrAddresses->IfIndex, QString::fromStdWString(pCurrAddresses->Description));
            pCurrAddresses = pCurrAddresses->Next;
        }
    }
    return allInterfaceKeyValue;
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

CNetworkAdapter::NetworkProperties CNetworkAdapter::getNetworkProperties(int indexAdapter)
{
    getAdapterByIndex(pCurrAddresses, indexAdapter);

    if (pCurrAddresses != nullptr)
    {
        adapterProperties.networkAdapter = QString::fromStdWString(pCurrAddresses->Description);
        adapterProperties.interfaceType = getAdapterType(pCurrAddresses);
        adapterProperties.hardwareAddress = getHardwareAddress(pCurrAddresses);
        adapterProperties.connectionName = QString::fromStdWString(pCurrAddresses->FriendlyName);
        adapterProperties.connectionSpeed = (pCurrAddresses->TransmitLinkSpeed);
        adapterProperties.MTU = pCurrAddresses->Mtu;
    }

    return adapterProperties;
}

CNetworkAdapter::NetworkAdapterAddreses CNetworkAdapter::getNetworkAdapterAddreses(int indexAdapter)
{
    getAdapterByIndex(pCurrAddresses, indexAdapter);
    if (pCurrAddresses != nullptr)
    {
        char ipAddressStr[INET_ADDRSTRLEN]{};
        sockaddr_in *address{};

        if (pCurrAddresses->Ipv4Enabled)
        {
            // retrive ip adress
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != nullptr)
            {
                while (pUnicast)
                {
                    // Check if this address is an IPv4 address
                    if (pUnicast->Address.lpSockaddr->sa_family != AF_INET)
                    {
                        pUnicast = pUnicast->Next;
                        continue;
                    }
                    address = (struct sockaddr_in *)pUnicast->Address.lpSockaddr;
                    inet_ntop(AF_INET, &address->sin_addr, ipAddressStr, INET_ADDRSTRLEN);
                    adapterAddreses.IPAddr = QString::fromLocal8Bit(ipAddressStr);
                    break;
                }
            };

            // retrive subnet mask
            ULONG out{};
            ConvertLengthToIpv4Mask(pUnicast->OnLinkPrefixLength, &out);
            in_addr in{};
            in.S_un.S_addr = out;
            inet_ntop(AF_INET, &in, ipAddressStr, INET_ADDRSTRLEN);
            adapterAddreses.subnetMask = QString::fromLocal8Bit(ipAddressStr);
        }

        // retrive gateway
        if (pCurrAddresses->FirstGatewayAddress != nullptr)
        {
            address = (struct sockaddr_in *)pCurrAddresses->FirstGatewayAddress->Address.lpSockaddr;
            inet_ntop(AF_INET, &address->sin_addr, ipAddressStr, INET_ADDRSTRLEN);
            adapterAddreses.gatewayAddr = QString::fromLocal8Bit(ipAddressStr);
        }

        // retrive DHCP
        if (pCurrAddresses->Dhcpv4Enabled)
        {
            isDHCPEnabled = true;
            address = (struct sockaddr_in *)pCurrAddresses->Dhcpv4Server.lpSockaddr;
            inet_ntop(AF_INET, &address->sin_addr, ipAddressStr, INET_ADDRSTRLEN);
            adapterAddreses.DHCPAddr = QString::fromLocal8Bit(ipAddressStr);

            // Retrieve the DHCP lease obtained and expires time
            adapterAddreses.DHCPLeaseExpires = QDateTime::currentDateTime().addSecs(pUnicast->ValidLifetime);
            adapterAddreses.DHCPLeaseObtained = adapterAddreses.DHCPLeaseExpires.addSecs(0 - (pUnicast->LeaseLifetime));
        }

        // retriveDNS
        if (pCurrAddresses->DdnsEnabled)
        {
            if (pCurrAddresses->DnsSuffix != nullptr)
                adapterAddreses.DNSSuffix = QString::fromWCharArray(pCurrAddresses->DnsSuffix);

            address = (struct sockaddr_in *)pCurrAddresses->FirstDnsServerAddress->Address.lpSockaddr;
            inet_ntop(AF_INET, &address->sin_addr, ipAddressStr, INET_ADDRSTRLEN);
            adapterAddreses.DNSServers = QString::fromLocal8Bit(ipAddressStr);
        }
    }

    return adapterAddreses;
}

void CNetworkAdapter::getAdapterByIndex(PIP_ADAPTER_ADDRESSES &adapter, int &index)
{
    if (adapter->IfIndex != index)
    {
        adapter = pAddresses;
        while (adapter)
        {
            if (adapter->IfIndex == index)
            {
                break;
            }
            adapter = adapter->Next;
        }
    }
}

QString CNetworkAdapter::getAdapterType(PIP_ADAPTER_ADDRESSES &index)
{
    switch (index->IfType)
    {
    case IF_TYPE_OTHER:
        return "Other";
        break;
    case IF_TYPE_ETHERNET_CSMACD:
        if (strstr(QString::fromStdWString(index->Description).toStdString().c_str(), ("bluetooth")) != NULL)
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
        return ("Software Loopback");
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
        return "Unknown type " + QString::number(index->IfType);
        break;
    }
}

QString CNetworkAdapter::getHardwareAddress(PIP_ADAPTER_ADDRESSES &index)
{
    if (index->PhysicalAddressLength == 0)
        return "";

    QString address{};
    char buffer[3]{};

    if (index->PhysicalAddressLength != 0)
    {
        for (int j = 0; j < (int)index->PhysicalAddressLength; j++)
        {
            for (j = 0; j < (int)index->PhysicalAddressLength; j++)
            {
                if (j == ((int)index->PhysicalAddressLength - 1))
                    sprintf(buffer, "%.2X\0", (int)index->PhysicalAddress[j]);
                else
                    sprintf(buffer, "%.2X-", (int)index->PhysicalAddress[j]);
                address.append(buffer);
            }
        }
    }
    return address;
}

// Initialize the cachedAddresses_
QCache<quint16, PIP_ADAPTER_ADDRESSES> CNetworkAdapter::cachedAddresses_{};