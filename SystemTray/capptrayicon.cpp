#include "capptrayicon.h"

CAppTrayIcon::CAppTrayIcon()
{
    this->tray = new QSystemTrayIcon(this);
    tray->setToolTip("Net4U"
                     "\n"
                     "Your network helper");
    tray->setIcon(QIcon(":/images/images/images/logo4.png"));
    // tray->show();
}

void CAppTrayIcon::closeEvent(QCloseEvent *event)
{

    // event->ignore();
    //  this->hide();
    //  QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    // tray->showMessage("Tray Program",
    //                   ("Приложение свернуто в трей. Для того чтобы, "
    //                    "развернуть окно приложения, щелкните по иконке приложения в трее"),
    //                   icon, 2000);
}

void CAppTrayIcon::closeApplicationToTray(QCloseEvent *event)
{
}
