#include "cmaincontrolblock.h"

CMainControlBlock::CMainControlBlock(Ui::MainWindow *qMain)
{
    try
    {
        adapter = new CNetworkAdapter();
        ui = qMain;
        setupComboBox();
        // setupInterfaceInfo();
        connect(ui->comboBox_interface, &QComboBox::currentIndexChanged, this, &CMainControlBlock::setupInterfaceInfo);
        setupInterfaceInfo(ui->comboBox_interface->currentIndex());
        // ui->comboBox_interface->setCurrentIndex(ui->comboBox_interface.);
    }
    catch (std::bad_alloc &bed)
    {
        qDebug() << bed.what();
    }
}

void CMainControlBlock::setupInterfaceInfo(int index)
{
    auto value = ui->comboBox_interface->itemData(index).toInt();
    qDebug() << "Value index from combobox is " << value;

    // Create a QStandardItemModel to hold the data
    QStandardItemModel *model = new QStandardItemModel();

    // Set the header data for the model
    model->setHorizontalHeaderLabels({"Field", "Value"});

    auto interfaceDescription = adapter->getNetworkProperties(value);

    // Create the top-level item for "Network Adapter Properties"
    QList<QStandardItem *> topLevelItems;
    QStandardItem *networkAdapterPropertiesItem = new QStandardItem("Network Adapter Properties");
    topLevelItems.append(networkAdapterPropertiesItem);

    // Create child items for "Network Adapter Properties"
    QList<QStandardItem *> networkAdapterItems;
    networkAdapterItems.append(new QStandardItem("Network Adapter"));
    networkAdapterItems.append(new QStandardItem(interfaceDescription.networkAdapter));
    networkAdapterPropertiesItem->appendRow(networkAdapterItems);

    QList<QStandardItem *> interfaceTypeItems;
    interfaceTypeItems.append(new QStandardItem(QIcon(":/icons/images/icons/cil-save.png"), "Interface Type"));
    interfaceTypeItems.append(new QStandardItem(interfaceDescription.interfaceType));
    networkAdapterPropertiesItem->appendRow(interfaceTypeItems);

    QList<QStandardItem *> hardwareAddressItems;
    hardwareAddressItems.append(new QStandardItem("Hardware Address"));
    hardwareAddressItems.append(new QStandardItem(interfaceDescription.hardwareAddress));
    networkAdapterPropertiesItem->appendRow(hardwareAddressItems);

    QList<QStandardItem *> connectionNameItems;
    connectionNameItems.append(new QStandardItem("Connection Name"));
    connectionNameItems.append(new QStandardItem(interfaceDescription.connectionName));
    networkAdapterPropertiesItem->appendRow(connectionNameItems);

    QList<QStandardItem *> connectionSpeedItems;
    connectionSpeedItems.append(new QStandardItem("Connection Speed"));
    connectionSpeedItems.append(
        new QStandardItem(QString::number(interfaceDescription.connectionSpeed / 1000000) + QString(" Mbps")));
    networkAdapterPropertiesItem->appendRow(connectionSpeedItems);

    model->appendRow(networkAdapterPropertiesItem);

    ui->treeView_interfaces->setModel(model);
    ui->treeView_interfaces->setColumnWidth(0, 300);
    ui->treeView_interfaces->expandAll();
}

void CMainControlBlock::setupComboBox()
{
    ui->comboBox_interface->clear();
    QMap<int, QString> interfaceList = adapter->getListKeyValueInterface();

    for (auto it = interfaceList.constBegin(); it != interfaceList.constEnd(); ++it)
    {
        ui->comboBox_interface->addItem(it.value(), QVariant(it.key()));
    }
}
