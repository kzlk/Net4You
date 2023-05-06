#ifndef CROUTETABLE_H
#define CROUTETABLE_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "Network/cnetworkadapter.h"
#include "qstandarditemmodel.h"
#include <QObject>
#include <QTableView>
#include <iprtrmib.h>
#include "cviewcontextmenu.h"
class CRouteTable : public QObject
{
    Q_OBJECT
  public:
    CRouteTable(QTableView *tableView, CNetworkAdapter *adapter);
    ~CRouteTable();

    bool updateRoutingTable();

    void fillTable();

  private:
    QTableView *myTableView{};
    CNetworkAdapter *myAdapter{};

    CViewContextMenu *contextMenu{};

    PMIB_IPFORWARDTABLE pIpForwardTable{};

    QStandardItemModel *model{};

    DWORD dwSize{};
    DWORD dwRetVal{};

    char szDestIp[128]{};
    char szMaskIp[128]{};
    char szGatewayIp[128]{};

    struct in_addr IpAddr
    {
    };

    int i{};
};

#endif // CROUTETABLE_H
