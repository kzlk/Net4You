#include "cnetworkadapterspeed.h"
#include "qdebug.h"

CNetworkAdapterSpeed::CNetworkAdapterSpeed()
{
    timer = new QTimer(this);
}

void CNetworkAdapterSpeed::setNetworkSpeedForAdapter(int index, BYTE *rowHardwareAddr)
{

    disconnect(timer, &QTimer::timeout, this, &CNetworkAdapterSpeed::updateSpeed);

    this->index = index;
    this->rowHardwareAddr = rowHardwareAddr;

    preInBytes = preOutBytes = inBytes = outBytes = 0;

    DWORD Size = sizeof(MIB_IFTABLE);
    MIB_IFTABLE *Table = (PMIB_IFTABLE)malloc(Size);
    Size = sizeof(MIB_IFTABLE);

    while (GetIfTable(Table, &Size, TRUE) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(Table);
        Table = (PMIB_IFTABLE)malloc(Size);
    }

    for (DWORD i = 0; i < Table->dwNumEntries; i++)
    {
        if (Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
            Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
        {
            continue;
        }
        if (memcmp(Table->table[i].bPhysAddr, rowHardwareAddr, 6) == 0 /*||
            (Table->table[i].dwType == Table->table[index - 1].dwType)*/)
        {

            inBytes += Table->table[i].dwInOctets;
            outBytes += Table->table[i].dwOutOctets;
        }
    }

    free(Table);

    connect(timer, &QTimer::timeout, this, &CNetworkAdapterSpeed::updateSpeed);
}

void CNetworkAdapterSpeed::stopSpeedUpdating()
{
    timer->stop();
}

void CNetworkAdapterSpeed::setIntervalForUpdatingSpeed(int interval)
{
    timer->start(interval);
}

float CNetworkAdapterSpeed::setPrecision(float v)
{
    const int i = v * 10.0;
    return i / 10.0;
}

QString CNetworkAdapterSpeed::convertSpeed(float speed)
{
    if ((speed) >= 8 * 1024 * 1024)
    {
        speed = (speed) / (8 * 1024.0 * 1024);

        speed = (setPrecision(speed));

        qDebug() << QLatin1String("Speed is : ") << speed << QLatin1String(" MB/s ");

        return QString::number(speed) + QString(" MB/s");
    }
    else if (((speed)) >= 8 * 1024)
    {
        speed = (speed) / (8 * 1024.0);

        speed = (setPrecision(speed));

        qDebug() << QLatin1String("Speed is : ") << speed << QLatin1String(" KB/s ");

        return QString::number(speed) + QString(" KB/s");
    }
    else
    {
        speed = (speed) / 8;

        return QString::number(speed) + QString(" B/s");

        qDebug() << QLatin1String("Speed is: ") << speed << QLatin1String(" B/s");
    }
}

float CNetworkAdapterSpeed::convert(float &bytes, SPEED_MEASURE speed)
{
    switch (speed)
    {
    case SPEED_MEASURE::KILOBIT:
        return bytes * 8 / 1000;
    case SPEED_MEASURE::MEGABIT:
        return bytes * 8 / pow(10, 6);
    default:
        return bytes;
    }
}

void CNetworkAdapterSpeed::updateSpeed()
{
    DWORD Size = sizeof(MIB_IFTABLE);
    MIB_IFTABLE *Table = (PMIB_IFTABLE)malloc(Size);
    Size = sizeof(MIB_IFTABLE);

    MIB_IFROW row1{};
    row1.dwIndex = index;

    if (GetIfEntry(&row1) != NO_ERROR)
    {
        QMessageBox::critical(nullptr, tr("Error memory alloc"), tr("Could not get network speed info GetIfEntry"),
                              QMessageBox::Ok);
        return;
    }

    if (row1.dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED && row1.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
    {
        disconnect(timer, &QTimer::timeout, this, &CNetworkAdapterSpeed::updateSpeed);
        return;
    }

    while (GetIfTable(Table, &Size, TRUE) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(Table);
        Table = (PMIB_IFTABLE)malloc(Size);
    }

    if (Table == nullptr)
    {
        QMessageBox::critical(nullptr, tr("Error memory alloc"), tr("Could not get network speed info GetIfTable"),
                              QMessageBox::Ok);
        return;
    }

    preInBytes = inBytes;
    preOutBytes = outBytes;

    inBytes = 0;
    outBytes = 0;

    for (DWORD i = 0; i < Table->dwNumEntries; i++)
    {
        if (Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
            Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
        {
            continue;
        }
        // qDebug() << "MAC dw = " << Table->table[i].bPhysAddr << " MAC " << rowHardwareAddr << '\n';
        //   Check if the current interface index matches the selected adapter's index
        //  qDebug() << "index = " << Table->table[i].dwInOctets << "\t" << Table->table[adapterInfo->Index - 1].
        //  dwInOctets;
        if (memcmp(Table->table[i].bPhysAddr, rowHardwareAddr, 6) == 0 ||
            (Table->table[i].dwType == Table->table[index - 1].dwType))
        {

            // qDebug() << "Counter = " << cnt << " index is  " << Table->table[i].dwIndex << '\n';
            inBytes += Table->table[i].dwInOctets;
            outBytes += Table->table[i].dwOutOctets;
        }
        // qDebug() << "i = " << i << " ||  " << Table->table[i].dwInOctets << " || " << Table->table[i].bDescr << " ||
        // "
        //  << Table->table[i].dwIndex << '\n';
    }
    free(Table);

    qDebug("inBytes: %llu , outBytes: %llu\n", inBytes, outBytes);

    float inKBs = 0;
    float outKBs = 0;

    // Speed
    inKBs = (inBytes - preInBytes);
    outKBs = (outBytes - preOutBytes);

    // emit this->networkSpeedChangeg(inKBs, outKBs);
    // emit this->networkBytesReceivedChanged(row1.dwInOctets, row1.dwOutOctets);
    emit this->networkBytesReceivedChanged(row1.dwInOctets, row1.dwOutOctets, inKBs, outKBs);
}
