#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifdef WIN32
#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <QLayout>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    downSpeed = new QLabel(this);
    upSpeed = new QLabel(this);

    QVBoxLayout* la = new QVBoxLayout(this);

    la->addWidget(downSpeed);
    la->addWidget(upSpeed);

    this->ui->centralWidget->setLayout(la);

    preInBytes = preOutBytes = inBytes = outBytes = 0;
/*********************************************************************/

//    DWORD           Size  = sizeof(MIB_IFTABLE);
//    MIB_IFTABLE*	Table = (PMIB_IFTABLE)malloc(Size);
//    Size = sizeof(MIB_IFTABLE);

//    while(GetIfTable(Table,&Size,TRUE) == ERROR_INSUFFICIENT_BUFFER)
//    {
//        free(Table);
//        Table = (PMIB_IFTABLE)malloc(Size);
//    }

//    for(DWORD i = 0; i < Table->dwNumEntries; i++)
//    {
//        if(   Table->table[i].dwOperStatus != MIB_IF_OPER_STATUS_CONNECTED &&
//              Table->table[i].dwOperStatus != IF_OPER_STATUS_OPERATIONAL   )
//        {
//            continue;
//        }

//        inBytes += Table->table[i].dwInOctets;
//        outBytes += Table->table[i].dwOutOctets;

//    }

//    free(Table);

    QTimer* timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(updateSpeed()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
float setPrecision(float v)
{
    int i = v * 10.0;
    return  i / 10.0;
}

void MainWindow::updateSpeed()
{
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

        qDebug() << "Index " << i << '\n';


        inBytes  += Table->table[i].dwInOctets;
        outBytes += Table->table[i].dwOutOctets;


    }

 qDebug() << "Octets " <<Table->dwNumEntries << '\n';
    free(Table);


    qDebug("inBytes: %d , outBytes: %d\n",inBytes,outBytes);

    this->setUpdatesEnabled(true);

    QString     text,str, text2;

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
        text2 = QLatin1String("Out: ") + str + QLatin1String("MB/s");

       //// downSpeed->setText(text);
       //// upSpeed->setText(text2);
    }
    else if((inBytes - preInBytes) >= 8*1024 ||
            (outBytes - preOutBytes) >= 8*1024)
    {
        inKBs = (inBytes - preInBytes) / (8*1024.0);
        outKBs = (outBytes - preOutBytes) / (8*1024.0);

        str.setNum(setPrecision(inKBs));
        text = QLatin1String("In: ") + str + QLatin1String("KB/s ");
        str.setNum(setPrecision(outKBs));
        text2 = QLatin1String("Out: ") + str + QLatin1String("KB/s");

        //downSpeed->setText(text);
        //upSpeed->setText(text2);
    }
    else
    {
        inKBs = (inBytes - preInBytes) / 8;
        outKBs = (outBytes - preOutBytes) / 8;
        str.setNum(inKBs);
        text = QLatin1String("In: ") + str + QLatin1String("B/s ");
        str.setNum(outKBs);
        text2 = QLatin1String("Out: ") + str + QLatin1String("B/s");



    }
    downSpeed->setText(QString::number(inBytes) + " - " +  text);
        upSpeed->setText(text2);
  //  this->repaint();
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


void MainWindow::paintEvent(QPaintEvent* event)
{
//    QPainter    paint(this);
//    QRect       rc;

//    QString     text,str;

//    float       inKBs = 0;
//    float       outKBs= 0;

//    if((inBytes - preInBytes) >= 8*1024*1024     ||
//       (outBytes - preOutBytes) >= 8*1024*1024   )
//    {
//        inKBs = (inBytes - preInBytes) / (8*1024.0*1024);
//        outKBs = (outBytes - preOutBytes) / (8*1024.0*1024);

//        str.setNum(setPrecision(inKBs));
//        text = QLatin1String("In: ") + str + QLatin1String("MB/s ");
//        str.setNum(setPrecision(outKBs));
//        text += QLatin1String("Out: ") + str + QLatin1String("MB/s");
//    }
//    else if((inBytes - preInBytes) >= 8*1024 ||
//            (outBytes - preOutBytes) >= 8*1024)
//    {
//        inKBs = (inBytes - preInBytes) / (8*1024.0);
//        outKBs = (outBytes - preOutBytes) / (8*1024.0);

//        str.setNum(setPrecision(inKBs));
//        text = QLatin1String("In: ") + str + QLatin1String("KB/s ");
//        str.setNum(setPrecision(outKBs));
//        text += QLatin1String("Out: ") + str + QLatin1String("KB/s");
//    }
//    else
//    {
//        inKBs = (inBytes - preInBytes) / 8;
//        outKBs = (outBytes - preOutBytes) / 8;
//        str.setNum(inKBs);
//        text = QLatin1String("In: ") + str + QLatin1String("B/s ");
//        str.setNum(outKBs);
//        text += QLatin1String("Out: ") + str + QLatin1String("B/s");

//    }

//    paint.setPen(Qt::yellow);
//    paint.setFont(QFont("Arial", 8));
//    //paint.drawText(rect(), Qt::AlignCenter, "Qt");

//    paint.fillRect(rect(),Qt::black);
//    paint.drawText(rect(),Qt::AlignCenter,text);

//    qWarning(text.toLatin1().data());
}
