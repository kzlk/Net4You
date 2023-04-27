#ifndef CNETWORKADAPTERSPEED_H
#define CNETWORKADAPTERSPEED_H
#include <QObject>
#include <QWidget>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <Iphlpapi.h>
#include <QTimer>
#include <QMessageBox>
class CNetworkAdapterSpeed : public QObject
{
    Q_OBJECT
  public:
    CNetworkAdapterSpeed();
    void setNetworkSpeedForAdapter(int index, BYTE *rowHardwareAddr);
    void stopSpeedUpdating();
    void setIntervalForUpdatingSpeed(int interval);
    QString convertSpeed(float speed);

  private:
    int adapterIndex{};
    QTimer *timer{};

    int index{};
    BYTE *rowHardwareAddr{};

    quint64 inBytes;
    quint64 outBytes;

    quint64 preInBytes;
    quint64 preOutBytes;

    float setPrecision(float v);

  private slots:
    void updateSpeed();

  signals:
    void networkSpeedChangeg(float download, float upload);
    // void networkBytesReceivedChanged(int received, int sent);
    void networkBytesReceivedChanged(int received, int sent, float download, float upload);
};

#endif // CNETWORKADAPTERSPEED_H
