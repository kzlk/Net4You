#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifdef WIN32
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib,"iphlpapi.lib")
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->setWindowOpacity(0.7);
    //this->setAttribute(Qt::WA_TranslucentBackground,true);

    preInBytes = preOutBytes = inBytes = outBytes = 0;

#ifdef WIN32
    DWORD           Size  = sizeof(MIB_IFTABLE);
    MIB_IFTABLE*	Table = (PMIB_IFTABLE)malloc(Size);
    Size = sizeof(MIB_IFTABLE);

    while(GetIfTable(Table,&Size,TRUE) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(Table);
        Table = (PMIB_IFTABLE)malloc(Size);
    }

    for(DWORD i = 0; i < Table->dwNumEntries; i++)
    {
        if(   Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
              Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL   )
        {
            continue;
        }

        inBytes += Table->table[i].dwInOctets;
        outBytes += Table->table[i].dwOutOctets;
    }

    free(Table);

#else

#endif

    QTimer* timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateSpeed()
{
#ifdef WIN32
    DWORD           Size  = sizeof(MIB_IFTABLE);
    MIB_IFTABLE*	Table = (PMIB_IFTABLE)malloc(Size);
    Size = sizeof(MIB_IFTABLE);

    while(GetIfTable(Table,&Size,TRUE) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(Table);
        Table = (PMIB_IFTABLE)malloc(Size);

    }

    preInBytes = inBytes;
    preOutBytes = outBytes;
    inBytes     = 0;
    outBytes    = 0;

    for(DWORD i = 0; i < Table->dwNumEntries; i++)
    {
        if(   Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
              Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL   )
        {
            continue;
        }

        inBytes  += Table->table[i].dwInOctets;
        outBytes += Table->table[i].dwOutOctets;
    }

    free(Table);

    qDebug("inBytes: %d , outBytes: %d\n",inBytes,outBytes);

#else

#endif

    this->setUpdatesEnabled(true);
    this->repaint();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        dragable = true;
        dragPoint = event->globalPos() - frameGeometry().topLeft();
        event->accept();

    }
    else if(event->button() == Qt::RightButton)
    {
        destroy();
        close();
#ifdef WIN32
        TerminateProcess(GetCurrentProcess(),0);
#endif
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(dragable)
    {
        move(event->globalPos() - dragPoint);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    dragable = false;
}

float setPrecision(float v)
{
    int i = v * 10.0;
    return  i / 10.0;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter    paint(this);
    QRect       rc;

    QString     text,str;

    float       inKBs = 0;
    float       outKBs= 0;

    if((inBytes - preInBytes) >= 8*1024*1024     ||
       (outBytes - preOutBytes) >= 8*1024*1024   )
    {
        inKBs = (inBytes - preInBytes) / (8*1024.0*1024);
        outKBs = (outBytes - preOutBytes) / (8*1024.0*1024);

        str.setNum(setPrecision(inKBs));
        text = QLatin1String("In: ") + str + QLatin1String("MB/s ");
        str.setNum(setPrecision(outKBs));
        text += QLatin1String("Out: ") + str + QLatin1String("MB/s");
    }
    else if((inBytes - preInBytes) >= 8*1024 ||
            (outBytes - preOutBytes) >= 8*1024)
    {
        inKBs = (inBytes - preInBytes) / (8*1024.0);
        outKBs = (outBytes - preOutBytes) / (8*1024.0);

        str.setNum(setPrecision(inKBs));
        text = QLatin1String("In: ") + str + QLatin1String("KB/s ");
        str.setNum(setPrecision(outKBs));
        text += QLatin1String("Out: ") + str + QLatin1String("KB/s");
    }
    else
    {
        inKBs = (inBytes - preInBytes) / 8;
        outKBs = (outBytes - preOutBytes) / 8;
        str.setNum(inKBs);
        text = QLatin1String("In: ") + str + QLatin1String("B/s ");
        str.setNum(outKBs);
        text += QLatin1String("Out: ") + str + QLatin1String("B/s");

    }

    paint.setPen(Qt::yellow);
    paint.setFont(QFont("Arial", 8));
    //paint.drawText(rect(), Qt::AlignCenter, "Qt");

    paint.fillRect(rect(),Qt::black);
    paint.drawText(rect(),Qt::AlignCenter,text);

    qWarning(text.toLatin1().data());
}
