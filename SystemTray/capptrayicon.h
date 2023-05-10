#ifndef CAPPTRAYICON_H
#define CAPPTRAYICON_H

#include "QSystemTrayIcon"
#include "qevent.h"
#include "qwidget.h"

class CAppTrayIcon : public QWidget
{
    Q_OBJECT
  public:
    CAppTrayIcon();

  private:
    QSystemTrayIcon *tray{};
    void closeEvent(QCloseEvent *event) override;

  private slots:
    void closeApplicationToTray(QCloseEvent *event);
};

#endif // CAPPTRAYICON_H
