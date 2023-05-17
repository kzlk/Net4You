#include "cmaincontrolblock.h"

CMainControlBlock::CMainControlBlock(Ui::MainWindow *qMain)
{
    try
    {
        ui = qMain;

        speed = new CNetworkAdapterSpeed();

        adapter = new CNetworkAdapter();

        wirelessAdapter = new CWirelessNetworkAdapter();

        graph = new CPaintNetworkGraphic(qMain, speed, adapter);

        // Create a QStandardItemModel to hold the data
        model = new QStandardItemModel();
        // Set the header data for the model
        model->setHorizontalHeaderLabels({"Field", "Value"});

        // Set model for treeView
        ui->treeView_interfaces->setModel(model);
        // Set column width for first (0) column
        ui->treeView_interfaces->setColumnWidth(0, 300);
        // Enable sorting for treeView
        ui->treeView_interfaces->setSortingEnabled(true);

        // Add item to comboBox for main window
        setupComboBox();

        // Setup route table window
        setupRouteTable();

        connect(ui->comboBox_interface, &QComboBox::currentIndexChanged, this, &CMainControlBlock::setupInterfaceInfo);
        setupInterfaceInfo(ui->comboBox_interface->currentIndex());

        contextTreeMenu = new CViewContextMenu(ui->treeView_interfaces, adapter);

        // Refresh in context menu in main window
        connect(contextTreeMenu, &CViewContextMenu::refreshClicked, [=]() {
            if (adapter->updateDeviceList())
            {
                setupComboBox();
            }
            else
            {
                qDebug() << "Error at adapter->updateDeviceList() in CMainControlBlock \n";
            }
        });

        // Connect the networkSpeedChangeg signal to a lambda function
        connect(speed, &CNetworkAdapterSpeed::networkBytesReceivedChanged,
                [=](uint received, uint sent, float download, float upload) {
                    // Find the "Download Speed" item in the model
                    QModelIndex downloadIndex = model->indexFromItem(downloadedItem.at(1));
                    QModelIndex uploadIndex = model->indexFromItem(uploadedItem.at(1));

                    // Update the "Value" field with the new download speed
                    model->setData(
                        downloadIndex,
                        speedStr.arg(received).arg(speed->convertSpeed(received)).arg(speed->convertSpeed(download)));

                    model->setData(uploadIndex,
                                   speedStr.arg(sent).arg(speed->convertSpeed(sent)).arg(speed->convertSpeed(upload)));
                });

        speed->setIntervalForUpdatingSpeed(1000);

        // Realtime updating dBm (signal WiFi strength)
        connect(wirelessAdapter, &CWirelessNetworkAdapter::updateSignalStrength, [=](int signal) {
            QModelIndex signalIndex = model->indexFromItem(signalStreghtItem.at(1));
            model->setData(signalIndex,
                           QString::number(signal) + " dBm (" + wirelessAdapter->getSignalLvlFromDBM(signal) + ")");
        });
    }
    catch (std::bad_alloc &bed)
    {
        qDebug() << bed.what();
    }
}

void CMainControlBlock::setupRouteTable()
{
    routeTable = new CRouteTable(ui->tableView_routes, adapter);

    routeTable->fillTable();

    // routeTable->deleteLater();
}

void CMainControlBlock::setupInterfaceInfo(int index)
{
    if (ui->comboBox_interface->count() == 0)
        return;

    // delete rows for displaying new info
    model->removeRows(0, model->rowCount());

    // get index of interf. from combobox
    auto value = ui->comboBox_interface->itemData(index).toInt();
    qDebug() << "Value index from combobox is " << value;

    // get network adapter properties description
    auto interfaceDescription = adapter->getNetworkProperties(value);

    // get network adapter addresses description
    auto intrfaceDescriptionAddreses = adapter->getNetworkAdapterAddreses(value);

    // Create the top-level item for "Network Adapter Properties"
    QList<QStandardItem *> topLevelItems;

    QStandardItem *networkAdapterPropertiesItem = (new QStandardItem("Network Adapter Properties"));

    topLevelItems.append(networkAdapterPropertiesItem);

    // Create child items for "Network Adapter Properties"
    QList<QStandardItem *> networkAdapterItems =
        createStandardItemList("Network Adapter", interfaceDescription.networkAdapter);
    QList<QStandardItem *> interfaceTypeItems = createStandardItemList(
        "Interface Type", interfaceDescription.interfaceType, QIcon(":/icons/images/icons/cil-save.png"));
    QList<QStandardItem *> hardwareAddressItems =
        createStandardItemList("Hardware Address", interfaceDescription.hardwareAddress);
    QList<QStandardItem *> connectionNameItems =
        createStandardItemList("Connection Name", interfaceDescription.connectionName);
    QList<QStandardItem *> connectionSpeedItems = createStandardItemList(
        "Connection Speed", QString::number(interfaceDescription.connectionSpeed / 1000000) + QString(" Mbps"));
    QList<QStandardItem *> mtuItems =
        createStandardItemList("MTU", QString::number(interfaceDescription.MTU).append(" bytes"));

    downloadedItem = createStandardItemList("Bytes Received", "0");
    uploadedItem = createStandardItemList("Bytes Sent", "0");
    speed->setNetworkSpeedForAdapter(value, interfaceDescription.rowHardwareAddr);

    networkAdapterPropertiesItem->appendRow(networkAdapterItems);
    networkAdapterPropertiesItem->appendRow(interfaceTypeItems);
    networkAdapterPropertiesItem->appendRow(hardwareAddressItems);
    networkAdapterPropertiesItem->appendRow(connectionNameItems);
    networkAdapterPropertiesItem->appendRow(mtuItems);
    networkAdapterPropertiesItem->appendRow(connectionSpeedItems);
    networkAdapterPropertiesItem->appendRow(downloadedItem);
    networkAdapterPropertiesItem->appendRow(uploadedItem);

    QStandardItem *networkAdapterAddressesItem = new QStandardItem("Network Adapter Addresses");
    topLevelItems.append(networkAdapterAddressesItem);

    // Create child items for "Network Adapter Addresses"
    auto ipItem = createStandardItemList("IP", intrfaceDescriptionAddreses.IPAddr);
    auto maskItem = createStandardItemList("Subnet Mask", intrfaceDescriptionAddreses.subnetMask);
    auto dhcpItem = createStandardItemList("DHCP address", intrfaceDescriptionAddreses.DHCPAddr);
    auto dhcpLeaseObtainedItem = createStandardItemList(
        "DHCP Lease Obtained", intrfaceDescriptionAddreses.DHCPLeaseObtained.toString("dd.MM.yyyy hh:mm:ss"));
    auto dhcpLeaseExpiresItem = createStandardItemList(
        "DHCP Lease Expires", intrfaceDescriptionAddreses.DHCPLeaseExpires.toString("dd.MM.yyyy hh:mm:ss"));
    auto gatewayItem = createStandardItemList("Default Gateway", intrfaceDescriptionAddreses.gatewayAddr);
    auto dnsItem = createStandardItemList("DNS", intrfaceDescriptionAddreses.DNSServers);
    auto dnsSuffixItem = createStandardItemList("DNS Suffix", intrfaceDescriptionAddreses.DNSSuffix);

    networkAdapterAddressesItem->appendRow(ipItem);
    networkAdapterAddressesItem->appendRow(maskItem);
    networkAdapterAddressesItem->appendRow(dhcpItem);
    networkAdapterAddressesItem->appendRow(dhcpLeaseObtainedItem);
    networkAdapterAddressesItem->appendRow(dhcpLeaseExpiresItem);
    networkAdapterAddressesItem->appendRow(gatewayItem);
    networkAdapterAddressesItem->appendRow(dnsItem);
    networkAdapterAddressesItem->appendRow(dnsSuffixItem);

    model->appendRow(networkAdapterPropertiesItem);
    model->appendRow(new QStandardItem(""));
    model->appendRow(networkAdapterAddressesItem);
    model->appendRow(new QStandardItem(""));

    /*Wireless*/
    wirelessAdapter->stopTimer();
    if (adapter->isInterfaceWireless(value))
    {

        wirelessAdapter->updateWlanProperties(); // update every time

        if (wirelessAdapter->isInterfaceConnectedToWifi(interfaceDescription.networkAdapter))
        {

            auto wirelessAdapterItem = new QStandardItem("Network Adapter Addresses");

            auto wirelessProperties = wirelessAdapter->getWlanProperties(interfaceDescription.networkAdapter);

            auto networkTypeItem = createStandardItemList("Network Type", wirelessProperties.networkType);
            auto ssidItem = createStandardItemList("SSID", wirelessProperties.ssid);
            auto bssidItem = createStandardItemList("BSSID", wirelessProperties.bssid);
            auto authAlgo = createStandardItemList("Authentication Algorithm", wirelessProperties.authAlgorithm);
            auto cipherAlgo = createStandardItemList("Cipher Algorithm", wirelessProperties.cipherAlgo);

            channelItem = createStandardItemList(
                "Channel", QString::number(wirelessAdapter->getWifiChannelNumFromMhz(wirelessProperties.channelMHz)) +
                               " (" + QString::number(wirelessProperties.channelMHz) + " Mhz)");

            signalStreghtItem = createStandardItemList("Signal Strength", "");

            transmitRateItem =
                createStandardItemList("Transmit Rate", QString::number(wirelessProperties.transmitRate) + " Mbps");
            receiveRateItem =
                createStandardItemList("Receive Rate", QString::number(wirelessProperties.receieveRate) + " Mbps");

            wirelessAdapter->setWlanInterfaceForUpdating(interfaceDescription.networkAdapter);
            wirelessAdapter->startTimer(1000);

            wirelessAdapterItem->appendRow(networkTypeItem);
            wirelessAdapterItem->appendRow(ssidItem);
            wirelessAdapterItem->appendRow(bssidItem);
            wirelessAdapterItem->appendRow(authAlgo);
            wirelessAdapterItem->appendRow(cipherAlgo);
            wirelessAdapterItem->appendRow(channelItem);
            wirelessAdapterItem->appendRow(signalStreghtItem);
            wirelessAdapterItem->appendRow(transmitRateItem);
            wirelessAdapterItem->appendRow(receiveRateItem);

            model->appendRow(wirelessAdapterItem);
        }
    }

    ui->treeView_interfaces->expandAll();
}

void CMainControlBlock::setupComboBox()
{
    ui->comboBox_interface->clear();
    QMap<int, QString> interfaceList = adapter->getListKeyValueInterface();

    for (auto it = interfaceList.constBegin(); it != interfaceList.constEnd(); ++it)
        ui->comboBox_interface->addItem(it.value(), QVariant(it.key()));
}

QList<QStandardItem *> CMainControlBlock::createStandardItemList(QString name, QString value, QIcon icon)
{
    QList<QStandardItem *> items;
    items.append(new QStandardItem(icon, name));
    items.append(new QStandardItem(value));
    return items;
}

CNetworkAdapterSpeed *CMainControlBlock::getNetworkSpeedAdapter()
{
    return speed;
}

CWirelessNetworkAdapter *const CMainControlBlock::getWirelessNetworkAdapter()
{
    return wirelessAdapter;
}
