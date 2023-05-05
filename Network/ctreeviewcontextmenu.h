#ifndef CTREEVIEWCONTEXTMENU_H
#define CTREEVIEWCONTEXTMENU_H

#include "Network/cnetworkadapter.h"
#include <QTreeView>
#include <QMenu>

class CTreeViewContextMenu : public QObject
{
    Q_OBJECT
    QTreeView *myTreeView{};
    CNetworkAdapter *myNetworkAdapter{};

  public:
    CTreeViewContextMenu(QTreeView *view, CNetworkAdapter *adapter);

  public slots:

    void showContextMenu(const QPoint &pos);
  private slots:

    void copySelectedValue();
    void copyAll();
    void refresh();
    void copyValueItem();

  signals:
    void refreshClicked();
};

#endif // CTREEVIEWCONTEXTMENU_H
