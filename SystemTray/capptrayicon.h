#ifndef CAPPTRAYICON_H
#define CAPPTRAYICON_H

#include "QSystemTrayIcon"
#include "qevent.h"
#include "qmainwindow.h"

class CAppTrayIcon : public QMainWindow
{
    Q_OBJECT
  public:
    CAppTrayIcon();
    ~CAppTrayIcon();
    void showMessage();

  private:
    QSystemTrayIcon *sysTrayIcon{};

    QMenu *menu{};
    QAction *viewWindow{};
    QAction *minimazeAction{};
    QAction *quitAction{};
    QAction *showWidgetAction{};
  signals:
    void showApp(bool checked);
    void closeApp(bool checked);
    void hideApp(bool checked);
    void activateTray(QSystemTrayIcon::ActivationReason reason);
};

#endif // CAPPTRAYICON_H
