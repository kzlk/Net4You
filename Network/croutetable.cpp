#include "croutetable.h"
#include <QHeaderView>
#include <QStyledItemDelegate>
CRouteTable::CRouteTable(QTableView *tableView, CNetworkAdapter *adapter)
{
    if (!updateRoutingTable())
        throw std::bad_alloc();

    myTableView = tableView;
    myAdapter = adapter;
    contextMenu = new CViewContextMenu(myTableView, myAdapter);

    tableView->verticalHeader()->hide();

    // Enable sorting on the table view
    tableView->setSortingEnabled(true);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    tableView->verticalHeader()->setDefaultSectionSize(40);

    model = new QStandardItemModel(this);

    model->setHorizontalHeaderLabels({"Type", "Net Destination", "Netmask", "Gateway", "Metric", "Interface"});

    myTableView->setModel(model);

    connect(contextMenu, &CViewContextMenu::refreshClicked, [=]() {
        if (adapter->updateDeviceList())
        {
            updateRoutingTable();
            fillTable();
        }
        else
        {
            qDebug() << "Error at adapter->updateDeviceList() in CRouteTable::CRouteTable(QTableView *tableView, "
                        "CNetworkAdapter *adapter) \n";
        }
    });
}

CRouteTable::~CRouteTable()
{
    if (pIpForwardTable != NULL)
        free(pIpForwardTable);
}

bool CRouteTable::updateRoutingTable()
{
    if (pIpForwardTable != NULL)
        free(pIpForwardTable);

    pIpForwardTable = (MIB_IPFORWARDTABLE *)malloc(sizeof(MIB_IPFORWARDTABLE));
    if (pIpForwardTable == NULL)
    {
        printf("Error allocating memory MIB_IPFORWARDTABLE\n");
        return false;
    }
    DWORD s{};
    if (GetIpForwardTable(pIpForwardTable, &s, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(pIpForwardTable);
        pIpForwardTable = (MIB_IPFORWARDTABLE *)malloc(s);
        if (pIpForwardTable == NULL)
        {
            printf("Error allocating memory MIB_IPFORWARDTABLE\n");
            return false;
        }
    }

    return dwRetVal = GetIpForwardTable(pIpForwardTable, &s, 0) == NO_ERROR;
}

void CRouteTable::fillTable()
{
    model->removeRows(0, model->rowCount());
    QString forwardType{};
    for (i = 0; i < (int)pIpForwardTable->dwNumEntries; i++)
    {
        QList<QStandardItem *> row;

        /* Convert IPv4 addresses to strings */
        IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardDest;
        strcpy_s(szDestIp, sizeof(szDestIp), inet_ntoa(IpAddr));

        IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardMask;
        strcpy_s(szMaskIp, sizeof(szMaskIp), inet_ntoa(IpAddr));

        IpAddr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardNextHop;
        strcpy_s(szGatewayIp, sizeof(szGatewayIp), inet_ntoa(IpAddr));

        switch (pIpForwardTable->table[i].dwForwardType)
        {
        case MIB_IPROUTE_TYPE_OTHER:
            forwardType = ("Other");
            break;
        case MIB_IPROUTE_TYPE_INVALID:
            forwardType = ("Invalid route");
            break;
        case MIB_IPROUTE_TYPE_DIRECT:
            forwardType = ("Direct");
            break;
        case MIB_IPROUTE_TYPE_INDIRECT:
            forwardType = ("Indirect");
            break;
        default:
            forwardType = ("Unknown type");
            break;
        }

        // printf("\tRoute[%d] Metric1: %ld\n", i, pIpForwardTable->table[i].dwForwardMetric1);

        row.append(new QStandardItem("Active (" + forwardType + ")"));
        row.append(new QStandardItem(szDestIp));
        row.append(new QStandardItem(szMaskIp));
        row.append(new QStandardItem(szGatewayIp));
        row.append(new QStandardItem(QString::number(pIpForwardTable->table[i].dwForwardMetric1)));

        auto formatAdapterName =
            myAdapter->getIPAddress(pIpForwardTable->table[i].dwForwardIfIndex)
                .append(" (")
                .append(myAdapter->getFriendlyNameAdapter(pIpForwardTable->table[i].dwForwardIfIndex))
                .append(")");

        row.append(new QStandardItem(formatAdapterName));

        model->appendRow(row);
    }
}
