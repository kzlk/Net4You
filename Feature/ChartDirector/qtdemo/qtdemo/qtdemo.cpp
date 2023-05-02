#include <QApplication>
#include <QGuiApplication>
#include <QStyleFactory>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QScrollArea>
#include <QHeaderView>
#include <QScrollBar>
#include "qchartviewer.h"
#include "qtdemo.h"
#include "democharts.h"
#include "hotspotdialog.h"


int main(int argc, char *argv[])
{
// This sample prorgram is designed with high DPI support.
#if QT_VERSION >= 0x050600 && QT_VERSION < 0x060000
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
// Before Qt 5.14, Qt only supports scaling by integer factors (eg. 200%), which is commonly used
// in macOS. Windows often uses non-integral scaling (eg, 150%). We enable non-integral scaling for
// Qt version >= 5.14.
#if QT_VERSION >= 0x051400
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication app(argc, argv);

    // Set the fusion style to ensure the GUI looks the same in differnt OS
    app.setStyle(QStyleFactory::create("Fusion"));

    // Must set a default font size, otherwise in some OS and Qt versions, Qt may not resize the
    // text in some widgets even if high DPI scaling is enabled.
    app.setStyleSheet("QWidget {font-size:12px}");

    QTDemo demo;
    demo.show();
    return app.exec();
}


QTDemo::QTDemo(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(Qt::Window);
    setWindowTitle("ChartDirector Qt Demo");
    resize(1000, 600);

    // Use horizontal layout to put the tree view on the left and the charts on the right
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setContentsMargins(4, 4, 4, 4);
    horizontalLayout->setSpacing(4);

    // Tree view on the left
    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setMaximumSize(250, 16777215);
    treeWidget->setHeaderHidden(true);
    horizontalLayout->addWidget(treeWidget);

    // In addition to the visible column, we add an invisible column in the tree view to hold
    // an index to the demo modules.
    treeWidget->setColumnCount(2);
    treeWidget->hideColumn(1);

    // Enable tree view horizontal scroll bar
    treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    treeWidget->header()->setStretchLastSection(false);
    treeWidget->setStyleSheet("QTreeView::item {padding: 1px 0px;}");

    // Loop through all the demo charts to populate the tree view
    QTreeWidgetItem *currentCategory = 0;
    for (DemoChart *d = demoCharts; 0 != d->name; ++d)
    {
        if (d->noOfCharts == 0)
        {
            // The item is a folder.
            currentCategory = new QTreeWidgetItem(QStringList() << d->name);
            currentCategory->setIcon(0, QIcon(d->icon));
            treeWidget->addTopLevelItem(currentCategory);
        }
        else
        {
            // The item is a demo module. We add the index in the hidden column.
            QTreeWidgetItem *item = new QTreeWidgetItem(
                QStringList() << d->name << QString("%1").arg(d - demoCharts));
            item->setIcon(0, currentCategory->icon(0));
            currentCategory->addChild(item);
        }
    }

    connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(onSelectionChanged()));

    // A scroll area on the right side
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("background-color:#FFFFFF;");
    scrollArea->setWidgetResizable(true);
    horizontalLayout->addWidget(scrollArea);
    QWidget *scrollAreaContent = new QWidget();
    scrollArea->setWidget(scrollAreaContent);

    // Put the chart viewers in the scroll area
    for (int i = 0; i < noOfChartViewers; ++i)
    {
        chartViewers[i] = new QChartViewer(scrollAreaContent);
        chartViewers[i]->hide();
        connect(chartViewers[i], SIGNAL(clicked(QMouseEvent*)), SLOT(onChartClicked(QMouseEvent*)));
    }
}

QTDemo::~QTDemo()
{
    // delete the charts
    for (int i = 0; i < 8; ++i)
        delete chartViewers[i]->getChart();
}

//
// User selects an item from the tree view
//
void QTDemo::onSelectionChanged()
{   
    //((QTreeWidget *)QObject::sender())->resizeColumnToContents(0);

    // Get the item selected
    QTreeWidgetItem *item = ((QTreeWidget *)QObject::sender())->currentItem();

    // The hidden column should contain the index to the demo module. The index is 0 if the item
    // is just a category heading.
    int demoIndex = item->data(1, Qt::DisplayRole).toString().toInt();
    if (demoIndex <= 0)
        return;

    DemoChart *d = demoCharts + demoIndex;

    // Hide and clear the old charts
    for (int i = 0; i < 8; ++i)
    {
        chartViewers[i]->setVisible(false);
        delete chartViewers[i]->getChart();
        chartViewers[i]->setChart(0);
    }

    // Create the selected demo charts
    for (int i = 0; i < d->noOfCharts; ++i)
    {
        d->createChart(chartViewers[i], i);
        chartViewers[i]->setVisible(true);
    }

    // Flow layout the charts
    flowLayoutCharts();
}

//
// Re-layout the charts upon container resize
//
void QTDemo::resizeEvent(QResizeEvent * /*event*/)
{
    flowLayoutCharts();
}

//
// Helper method to flow layout the charts
//
void QTDemo::flowLayoutCharts()
{
    const int margin = 5;

    // initial cursor position
    int cursorX = margin;
    int cursorY = margin;

    // initial page size
    int pageWidth = 0;
    int pageHeight = 0;

    // the width limit, beyond which wrapping will occur
    int wrapWidth = chartViewers[0]->parentWidget()->parentWidget()->width();

    for (int i = 0; i < noOfChartViewers; ++i)
    {
        // only layout visible charts
        if (!chartViewers[i]->isVisible())
            continue;

        int chartWidth =  chartViewers[i]->width();
        int chartHeight = chartViewers[i]->height();

        if (cursorX + margin + chartWidth > wrapWidth)
        {
            // no enough position in current line, so move to next line
            cursorX = margin;
            cursorY = pageHeight + margin;
        }

        // put chart in current line
        chartViewers[i]->setGeometry(cursorX, cursorY, chartWidth, chartHeight);

        // advance cursor
        cursorX += chartWidth + margin;

        if (pageWidth < cursorX)
            pageWidth = cursorX;
        if (pageHeight < cursorY + chartHeight)
            pageHeight = cursorY + chartHeight;
    }

    // resize the container to the proper size
    chartViewers[0]->parentWidget()->setFixedSize(pageWidth, pageHeight);
}

//
// User clicks on the QChartViewer
//
void QTDemo::onChartClicked(QMouseEvent * /*event*/)
{
    QChartViewer *viewer = (QChartViewer *)QObject::sender();
    ImageMapHandler *handler = viewer->getImageMapHandler();
    if (0 != handler)
    {
        // Query the ImageMapHandler to see if the mouse is on a clickable hot spot. We
        // consider the hot spot as clickable if its href ("path") parameter is not empty.
        const char *path = handler->getValue("path");
        if ((0 != path) && (0 != *path))
        {
            // In this sample code, we just show all hot spot parameters.
            HotSpotDialog hs;
            hs.setData(handler);
            hs.exec();
        }
    }
}
