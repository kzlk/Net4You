#pragma once

#ifndef CMAINCONTROLBLOCK_H
#define CMAINCONTROLBLOCK_H

#include <QObject>
#include <QMainWindow>
#include "ui_main.h"
#include "cnetworkadapter.h"
#include "qstandarditemmodel.h"
#include "cnetworkadapterspeed.h"
#include "cwirelessnetworkadapter.h"
#include "cviewcontextmenu.h"
#include "croutetable.h"
#include "Network/cpaintnetworkgraphic.h"

class CMainControlBlock : public QObject
{
    Q_OBJECT
  public:
    CMainControlBlock(Ui::MainWindow *main);

    CNetworkAdapterSpeed *getNetworkSpeedAdapter();
    CWirelessNetworkAdapter *const getWirelessNetworkAdapter();

    void setupRouteTable();

  private:
    void setupComboBox();
    QList<QStandardItem *> createStandardItemList(QString name, QString value, QIcon icon = QIcon());

    Ui::MainWindow *ui;
    QStandardItemModel *model;

    CNetworkAdapter *adapter{};
    CNetworkAdapterSpeed *speed{};
    CWirelessNetworkAdapter *wirelessAdapter{};
    CPaintNetworkGraphic *graph{};

    QList<QStandardItem *> networkAdapterItems{};

    QList<QStandardItem *> downloadedItem{};
    QList<QStandardItem *> uploadedItem{};

    // For Wlan
    QList<QStandardItem *> channelItem{};
    QList<QStandardItem *> signalStreghtItem{};
    QList<QStandardItem *> transmitRateItem{};
    QList<QStandardItem *> receiveRateItem{};

    //
    CViewContextMenu *contextTreeMenu{};

    //
    CRouteTable *routeTable{};

    //
    QString speedStr = "%1(%2) - %3/s";

  public slots:
    void setupInterfaceInfo(int index);
};

#endif // CMAINCONTROLBLOCK_H
