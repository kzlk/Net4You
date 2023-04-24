#ifndef CMAINCONTROLBLOCK_H
#define CMAINCONTROLBLOCK_H
#include <QObject>
#include <QMainWindow>
#include "ui_main.h"
#include "cnetworkadapter.h"
#include "qstandarditemmodel.h"

class CMainControlBlock : public QWidget
{
    // Q_OBJECT
  public:
    CMainControlBlock(Ui::MainWindow *main);

  private:
    void setupComboBox();

    Ui::MainWindow *ui;
    CNetworkAdapter *adapter{};

  public slots:
    void setupInterfaceInfo(int index);
};

#endif // CMAINCONTROLBLOCK_H
