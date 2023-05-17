

#include "NetworkDevicesWidget.h"
#include <QApplication>
#include "pcap.h"
#include <iomanip>
#include <iphlpapi.h>
#include <map>
#include <psapi.h>
#include <iostream>

#define MAX_STR_LEN 256
#define ETHERNET_HEADER_SIZE 14
using namespace std;
QMap<std::string, std::string> tab{};

typedef struct
{
    DWORD processId;
    ULONGLONG bytesIn;
    ULONGLONG bytesOut;
} ProcessData;

typedef struct
{
    DWORD ipSrc;
    DWORD ipDst;
    USHORT portSrc;
    USHORT portDst;
    ULONGLONG length;
} PacketData;

static pcap_t *pcapHandle;
static HANDLE mutex;
static std::unordered_map<DWORD, ProcessData> processData;

static void ProcessPacket(u_char *user, const struct pcap_pkthdr *header, const u_char *packet)
{
    PacketData packetData;
    const u_char *data = packet + ETHERNET_HEADER_SIZE;

    // Extract packet data
    packetData.ipSrc = *(DWORD *)(data + 12);
    packetData.ipDst = *(DWORD *)(data + 16);
    packetData.portSrc = *(USHORT *)(data + 20);
    packetData.portDst = *(USHORT *)(data + 22);
    packetData.length = header->len - ETHERNET_HEADER_SIZE;

    // Look up process ID for source IP address and port
    MIB_TCPROW_OWNER_MODULE tcpRow = {0};
    tcpRow.dwLocalAddr = packetData.ipSrc;
    tcpRow.dwLocalPort = htons(packetData.portSrc);
    GetOwnerModuleFromTcpEntry(&tcpRow, TCPIP_OWNER_MODULE_INFO_BASIC, 0, &tcpRow.dwOwningPid);

    // Lock mutex to avoid race conditions
    WaitForSingleObject(mutex, INFINITE);

    // Update process data with packet information
    ProcessData &pd = processData[tcpRow.dwOwningPid];
    pd.processId = tcpRow.dwOwningPid;
    if (tcpRow.dwLocalAddr == packetData.ipSrc && tcpRow.dwLocalPort == htons(packetData.portSrc))
        pd.bytesOut += packetData.length;
    else
        pd.bytesIn += packetData.length;

    // Release mutex
    ReleaseMutex(mutex);
}
static DWORD WINAPI MonitorThread(LPVOID param)
{
    pcap_loop(pcapHandle, -1, ProcessPacket, NULL);
    return 0;
}

void DisplayConnectedDevices()
{
    PMIB_IPNETTABLE pIpNetTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Retrieve the ARP table
    if (GetIpNetTable(NULL, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
    {
        pIpNetTable = (PMIB_IPNETTABLE)malloc(dwSize);
        if (pIpNetTable == NULL)
        {
            printf("Error allocating memory\n");
            return;
        }
        if (GetIpNetTable(pIpNetTable, &dwSize, FALSE) != NO_ERROR)
        {
            printf("Error retrieving ARP table\n");
            free(pIpNetTable);
            return;
        }

        // Display the list of connected devices
        printf("List of connected devices:\n");
        qDebug() << "Num entries is " << pIpNetTable->dwNumEntries << '\n';
        for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++)
        {
            if (pIpNetTable->table[i].dwPhysAddrLen != 0)
            {
                int s = pIpNetTable->table[i].dwPhysAddrLen;
                std::stringstream macAddressStream;
                for (int j = 0; j < s; j++)
                {
                    macAddressStream << std::hex << std::setw(2) << std::setfill('0')
                                     << (int)pIpNetTable->table[i].bPhysAddr[j];
                    if (j < pIpNetTable->table[i].dwPhysAddrLen - 1)
                    {
                        macAddressStream << ":";
                    }
                }

                std::string ip = std::string(inet_ntoa(*(struct in_addr *)&pIpNetTable->table[i].dwAddr));
                std::string macAddress = macAddressStream.str();
                // qDebug() << QString::fromStdString(ip) << '\t' << QString::fromStdString(macAddress) << '\n';
                if (macAddress != "00:00:00:00:00:00" && macAddress != "ff:ff:ff:ff:ff:ff")
                {
                    // qDebug() << QString::fromStdString(ip) << '\t' << QString::fromStdString(macAddress) << '\n';
                    tab.insert(macAddress, ip);
                }

                // printf("IP Address: %s, MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
                //        inet_ntoa(*(struct in_addr *)&pIpNetTable->table[i].dwAddr),
                //        pIpNetTable->table[i].bPhysAddr[0], pIpNetTable->table[i].bPhysAddr[1],
                //        pIpNetTable->table[i].bPhysAddr[2], pIpNetTable->table[i].bPhysAddr[3],
                //        pIpNetTable->table[i].bPhysAddr[4], pIpNetTable->table[i].bPhysAddr[5]);
            }
        }
        QMap<std::string, std::string>::Iterator end = tab.end();
        QMap<std::string, std::string>::Iterator begin = tab.begin();

        QMap<std::string, std::string>::const_iterator iter;
        for (iter = tab.constBegin(); iter != tab.constEnd(); ++iter)
        {
            qDebug() << QString::fromStdString(iter.key()) << '\t' << QString::fromStdString(iter.value()) << '\n';
        }

        free(pIpNetTable);
    }
    else
    {
        printf("Error retrieving ARP table size\n");
        return;
    }
}
// Function to convert MAC address to a string
std::string macToString(const u_char *mac)
{
    std::stringstream ss;
    ss << std::hex << static_cast<int>(mac[0]) << ":" << std::hex << static_cast<int>(mac[1]) << ":" << std::hex
       << static_cast<int>(mac[2]) << ":" << std::hex << static_cast<int>(mac[3]) << ":" << std::hex
       << static_cast<int>(mac[4]) << ":" << std::hex << static_cast<int>(mac[5]);
    return ss.str();
}
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    map<string, vector<string>> *devices = reinterpret_cast<map<string, vector<string>> *>(userData);
    // Extract the MAC and IP addresses from the packet
    const u_char *arp_payload = packet + 14;

    // Only process ARP replies
    uint32_t ip = *(uint32_t *)(arp_payload + 14);
    uint8_t mac[6];
    memcpy(mac, arp_payload + 8, 6);
    // Convert the MAC address to a string
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Convert the IP address to a string
    char ipStr[16];
    snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
    // Add the MAC and IP addresses to the list of devices
    (*devices)[ipStr].push_back(macStr);

    std::cout << macStr << '\t' << ipStr << '\n';
}
void getDevices()
{
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get the list of available devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        cerr << "Error finding devices: " << errbuf << endl;
        return;
    }

    // Loop through the list of devices
    for (pcap_if_t *d = alldevs; d != NULL; d = d->next)
    {
        // Open the device for sniffing
        pcap_t *handle = pcap_open_live(d->name, BUFSIZ, 1, 1000, errbuf);
        if (handle == NULL)
        {
            cerr << "Error opening device: " << errbuf << endl;
            continue;
        }

        // Compile and apply the filter
        struct bpf_program fp;
        char filter_exp[] = "arp";
        if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1)
        {
            cerr << "Error compiling filter: " << pcap_geterr(handle) << endl;
            continue;
        }
        if (pcap_setfilter(handle, &fp) == -1)
        {
            cerr << "Error setting filter: " << pcap_geterr(handle) << endl;
            continue;
        }

        // Start capturing packets
        map<string, vector<string>> devices;

        cout << "Sniffing on " << d->name << "...\n";

        // Capture packets for 10 seconds
        struct pcap_pkthdr *header;
        const u_char *packet;
        int timeout = 10000; // 10 seconds
        int res;
        while ((res = pcap_next_ex(handle, &header, &packet)) >= 0 && timeout > 0)
        {
            if (res == 0)
            {
                // Timeout expired
                break;
            }
            else if (res == -1)
            {
                // An error occurred
                cerr << "Error while capturing packets: " << pcap_geterr(handle) << endl;
                pcap_close(handle);
                return;
            }
            packetHandler((u_char *)&devices, header, packet);
            timeout -= header->len;
        }

        // Close the handle and print the list of devices
        pcap_close(handle);

        cout << "Devices on " << d->name << ":\n";
        for (auto &entry : devices)
        {
            cout << "  " << entry.first << ": ";
            for (auto &mac : entry.second)
            {
                cout << mac << " ";
            }
            cout << endl;
        }
    }

    // Free the list of devices
    pcap_freealldevs(alldevs);
}
void get_mac_address(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    // // Check if the packet is a DHCP packet
    // if (packet[12] == 0x08 && packet[13] == 0x00 && packet[23] == 0x11 && packet[236] == 0x35 && packet[237] == 0x01
    // &&
    //     packet[242] == 0x32 && packet[243] == 0x04)
    // {

    // Get the source MAC address from the packet
    const u_char *mac_address = packet + 6;

    // Convert the MAC address to a string
    stringstream stream;
    stream << setfill('0') << hex;
    for (int i = 0; i < 6; i++)
    {
        stream << setw(2) << (int)mac_address[i];
        if (i < 5)
            stream << ":";
    }
    string mac_str = stream.str();

    // Print the MAC address
    cout << "MAC address: " << mac_str << endl;

    // Get the IP address from the packet
    const u_char *ip_address = packet + 26;

    // Convert the IP address to a string
    stream.str("");
    for (int i = 0; i < 4; i++)
    {
        stream << (int)ip_address[i];
        if (i < 3)
            stream << ".";
    }
    string ip_str = stream.str();

    // Print the IP address
    cout << "IP address: " << ip_str << endl;
    // }
}

#define ETH_HEADER_LEN 14

struct eth_header
{
    u_char dest_addr[6];
    u_char src_addr[6];
    u_short ether_type;
};

map<string, string> devices; // to store MAC and IP addresses of devices

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct eth_header *ethhdr = (struct eth_header *)packet;
    const u_char *ip_hdr = packet + ETH_HEADER_LEN;

    if (ntohs(ethhdr->ether_type) == 0x0800)
    { // IPv4 packet
        char mac_addr[18];
        sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", ethhdr->src_addr[0], ethhdr->src_addr[1],
                ethhdr->src_addr[2], ethhdr->src_addr[3], ethhdr->src_addr[4], ethhdr->src_addr[5]);

        struct in_addr addr;
        memcpy(&addr, ip_hdr + 12, 4);
        string ip_addr = inet_ntoa(addr);

        devices[mac_addr] = ip_addr;
    }
}

void print_devices()
{
    printf("MAC address\tIP address\n");
    for (map<string, string>::iterator it = devices.begin(); it != devices.end(); it++)
    {
        printf("%s\t%s\n", it->first.c_str(), it->second.c_str());
    }
}

int main(int argc, char *argv[])
{

    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get a list of all available network adapters
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        cerr << "Error finding devices: " << errbuf << endl;
        return 1;
    }
    pcap_t *handle;
    struct bpf_program fp;
    bpf_u_int32 net, mask;
    // Print the name and description of each adapter
    //  for (pcap_if_t *dev = alldevs; dev != NULL; dev = dev->next)
    //  {
    //      cout << "Adapter name: " << dev->name << endl;
    //      cout << "Description: " << dev->description << endl;
    //  }
    if (pcap_lookupnet("\\Device\\NPF_{70C741BB-B108-4302-87A7-425DBD1FB3FB}", &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Can't get netmask for device %s\n", argv[1]);
        net = 0;
        mask = 0;
    }
    char filter_exp[] = "ether proto 0x0800"; // filter for IPv4 packets

    // if (pcap_lookupnet("Realtek Gaming GbE Family Controller", &net, &mask, errbuf) == -1)
    // {
    //     fprintf(stderr, "Can't get netmask for device %s\n", argv[1]);
    //     net = 0;
    //     mask = 0;
    // }

    handle = pcap_open_live("\\Device\\NPF_{70C741BB-B108-4302-87A7-425DBD1FB3FB}", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL)
    {
        fprintf(stderr, "Couldn't open device %s: %s\n", argv[1], errbuf);
        return -1;
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
    {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

    if (pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

    pcap_loop(handle, -1, process_packet, NULL);

    pcap_close(handle);

    print_devices();

    return 0;
}
