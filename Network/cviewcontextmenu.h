#ifndef CVIEWCONTEXTMENU_H
#define CVIEWCONTEXTMENU_H

#include "Network/cnetworkadapter.h"
#include <QTreeView>
#include <QTableView>
#include <QMenu>

class CViewContextMenu : public QObject
{
    Q_OBJECT
    QTreeView *myTreeView{};
    CNetworkAdapter *myNetworkAdapter{};
    QAbstractItemView *mView;

  public:
    CViewContextMenu(QTreeView *view, CNetworkAdapter *adapter);
    CViewContextMenu(QTableView *view, CNetworkAdapter *adapter);

  public slots:

    void showContextMenu(const QPoint &pos);
  private slots:

    void copySelectedValue();
    void copyAllSelectedRow();
    void copyAll();
    void refresh();
    void copyValueItem();
    void copyAllTableValue();

  signals:
    void refreshClicked();

  private:
    QMenu contextMenu{};

    const QString style = "QMenu { background-color: rgb(33, 37, 43); color : white; }"
                          "QMenu::item:selected { background-color: rgb(26, 58, 85);} "
                          "QMenu::separator { background-color: white; height: 1px; margin: 5px 0px 5px 0px; }";
};

#endif // CVIEWCONTEXTMENU_H
