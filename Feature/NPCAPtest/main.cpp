

#include "NetworkDevicesWidget.h"
#include <QApplication>
#include "pcap.h"
#include <iphlpapi.h>
#include <psapi.h>
#define MAX_STR_LEN 256
#define ETHERNET_HEADER_SIZE 14

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
        for (DWORD i = 0; i < pIpNetTable->dwNumEntries; i++)
        {

            printf("IP Address: %s, MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
                   inet_ntoa(*(struct in_addr *)&pIpNetTable->table[i].dwAddr), pIpNetTable->table[i].bPhysAddr[0],
                   pIpNetTable->table[i].bPhysAddr[1], pIpNetTable->table[i].bPhysAddr[2],
                   pIpNetTable->table[i].bPhysAddr[3], pIpNetTable->table[i].bPhysAddr[4],
                   pIpNetTable->table[i].bPhysAddr[5]);
        }

        free(pIpNetTable);
    }
    else
    {
        printf("Error retrieving ARP table size\n");
        return;
    }
}

int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);

    DisplayConnectedDevices();
    // return app.exec();
}
