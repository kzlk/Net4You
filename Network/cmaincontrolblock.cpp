#include "cmaincontrolblock.h"
#include "qstandarditemmodel.h"

CMainControlBlock::CMainControlBlock(Ui::MainWindow *qMain)
{
    ui = qMain;
    setupInterfaceInfo();
}

void CMainControlBlock::setupInterfaceInfo()
{


    //Devide::getDevideType(!=DEVICE::WLAN)

    // Create a QStandardItemModel to hold the data
    QStandardItemModel* model = new QStandardItemModel();

    // Set the header data for the model
    model->setHorizontalHeaderLabels({"Field", "Value"});

    // Create the top-level item for "Network Adapter Properties"
    QList<QStandardItem*> topLevelItems;
    QStandardItem* networkAdapterPropertiesItem = new QStandardItem("Network Adapter Properties");
    topLevelItems.append(networkAdapterPropertiesItem);

    // Create child items for "Network Adapter Properties"
    QList<QStandardItem*> networkAdapterItems;
    networkAdapterItems.append(new QStandardItem("Network Adapter"));
    networkAdapterItems.append(new QStandardItem("Realtek RTL8822CE 802.1 lac PCIe Adapter"));
    networkAdapterPropertiesItem->appendRow(networkAdapterItems);

    QList<QStandardItem*> interfaceTypeItems;
    interfaceTypeItems.append(new QStandardItem(QIcon(":/icons/images/icons/cil-save.png"), "Interface Type"));
    interfaceTypeItems.append(new QStandardItem("802.11 Wireless Ethernet"));
    networkAdapterPropertiesItem->appendRow(interfaceTypeItems);

    QList<QStandardItem*> hardwareAddressItems;
    hardwareAddressItems.append(new QStandardItem("Hardware Address"));
    hardwareAddressItems.append(new QStandardItem("5C-3A-45-92-F6-2B"));
    networkAdapterPropertiesItem->appendRow(hardwareAddressItems);

    QList<QStandardItem*> connectionNameItems;
    connectionNameItems.append(new QStandardItem("Connection Name"));
    connectionNameItems.append(new QStandardItem("Wi-Fi"));
    networkAdapterPropertiesItem->appendRow(connectionNameItems);

    QList<QStandardItem*> connectionSpeedItems;
    connectionSpeedItems.append(new QStandardItem("Connection Speed"));
    connectionSpeedItems.append(new QStandardItem("300 Mbps"));
    networkAdapterPropertiesItem->appendRow(connectionSpeedItems);

    model->appendRow(networkAdapterPropertiesItem);

     ui->treeView_interfaces->setModel(model);
     ui->treeView_interfaces->expandAll();
}
