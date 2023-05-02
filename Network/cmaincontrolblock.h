#ifndef CMAINCONTROLBLOCK_H
#define CMAINCONTROLBLOCK_H
#include <QObject>
#include <QMainWindow>
#include "ui_main.h"
#include "cnetworkadapter.h"
#include "qstandarditemmodel.h"
#include "cnetworkadapterspeed.h"
#include "cwirelessnetworkadapter.h"

class CMainControlBlock : public QObject
{
    Q_OBJECT
  public:
    CMainControlBlock(Ui::MainWindow *main);

  private:
    void setupComboBox();
    QList<QStandardItem *> createStandardItemList(QString name, QString value, QIcon icon = QIcon());

    Ui::MainWindow *ui;
    QStandardItemModel *model;

    CNetworkAdapter *adapter{};
    CNetworkAdapterSpeed *speed{};
    CWirelessNetworkAdapter *wirelessAdapter{};

    QList<QStandardItem *> downloadedItem{};
    QList<QStandardItem *> uploadedItem{};

  public slots:
    void setupInterfaceInfo(int index);
};

#endif // CMAINCONTROLBLOCK_H
