#include "cpaintnetworkgraphic.h"
#include "qbuttongroup.h"
#include "qtimer.h"
#include <math.h>
#include <QFileDialog>
#include <vector>
#include <sstream>
#include <QWidget>
static const int DataInterval = 1000;

CPaintNetworkGraphic::CPaintNetworkGraphic(Ui::MainWindow *qMain, CNetworkAdapterSpeed *speedNet,
                                           CNetworkAdapter *adapter)
{
    my = qMain;
    this->adapter = adapter;

    /// this->speed = speed;
    this->speed = new CNetworkAdapterSpeed();

    my->frame_5->setFrameShape(QFrame::StyledPanel);

    updatePeriod = qMain->comboBox_period;
    m_HScrollBar = qMain->horizontalScrollBar_2;

    // The Pointer/Zoom In/Zoom Out buttons form a button group
    mouseUsage = new QButtonGroup();
    mouseUsage->addButton(qMain->btn_scroll, Chart::MouseUsageScroll);
    mouseUsage->addButton(qMain->btn_zoom_in, Chart::MouseUsageZoomIn);
    mouseUsage->addButton(qMain->btn_zoom_out, Chart::MouseUsageZoomOut);
    connect(mouseUsage, SIGNAL(buttonPressed(QAbstractButton *)), SLOT(onMouseUsageChanged(QAbstractButton *)));

    startStopUsage = new QButtonGroup();
    startStopUsage->addButton(qMain->btn_run_graph, 1);
    startStopUsage->addButton(qMain->btn_pause_graph, 0);
    startStopUsage->addButton(qMain->btn_clear_graph, 2);

    connect(startStopUsage, &QButtonGroup::buttonPressed, [=](QAbstractButton *b) {
        if (startStopUsage->id(b) == 1)
        {
            // speed->setIntervalForUpdatingSpeed(my->comboBox_period->currentText().toInt());

            m_ChartUpdateTimer->start();

            //  connect(speed, &CNetworkAdapterSpeed::networkBytesReceivedChanged, this,
            //        &CPaintNetworkGraphic::updateSpeed);
        }

        else if (startStopUsage->id(b) == 0)
        {
            m_ChartUpdateTimer->stop();
            // speed->stopSpeedUpdating();
        }
        else if (startStopUsage->id(b) == 2)
        {
            for (int i = 0; i < sampleSize; i++)
            {
                m_timeStamps[i] = 0.0;
                m_dataSeriesA[i] = 0.0;
                m_dataSeriesB[i] = 0.0;
            }

            m_currentIndex = 0;

            m_ChartViewer->clearAllRanges();
        }
    });

    qMain->comboBox_period->addItems(QStringList() << "1000"
                                                   << "1250"
                                                   << "1500"
                                                   << "1750"
                                                   << "2000");

    qMain->comboBox_period->setCurrentIndex(0);

    connect(qMain->comboBox_period, SIGNAL(currentIndexChanged(int)), SLOT(onUpdatePeriodChanged(int)));

    // Alpha Value display
    (new QLabel("Download"))->setGeometry(6, 280, 48, 21);
    m_ValueA = new QLabel();
    m_ValueA->setGeometry(55, 280, 59, 21);
    m_ValueA->setFrameShape(QFrame::StyledPanel);

    // Beta Value display
    (new QLabel("Upload"))->setGeometry(6, 303, 48, 21);
    m_ValueB = new QLabel();
    m_ValueB->setGeometry(55, 303, 59, 21);
    m_ValueB->setFrameShape(QFrame::StyledPanel);

    // Gamma Value display
    (new QLabel("Gamma"))->setGeometry(6, 326, 48, 21);
    m_ValueC = new QLabel();
    m_ValueC->setGeometry(55, 326, 59, 21);
    m_ValueC->setFrameShape(QFrame::StyledPanel);

    connect(qMain->btn_save_graph, SIGNAL(clicked(bool)), SLOT(onSave(bool)));

    // Chart Viewer
    m_ChartViewer = new QChartViewer(this);
    m_ChartViewer->setGeometry(128, 4, 640, 350);

    connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(m_ChartViewer, SIGNAL(mouseMovePlotArea(QMouseEvent *)), SLOT(onMouseMovePlotArea(QMouseEvent *)));

    connect(qMain->horizontalScrollBar_2, SIGNAL(valueChanged(int)), SLOT(onHScrollBarChanged(int)));

    // Clear data arrays to Chart::NoValue
    for (int i = 0; i < sampleSize; ++i)
        m_timeStamps[i] = m_dataSeriesA[i] = m_dataSeriesB[i] = m_dataSeriesC[i] = Chart::NoValue;
    m_currentIndex = 0;

    // Set m_nextDataTime to the current time. It is used by the real time random number
    // generator so it knows what timestamp should be used for the next data point.
    m_nextDataTime = QDateTime::currentDateTime();

    // Initially set the mouse to drag to scroll mode.
    qMain->btn_scroll->click();

    // Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
    m_ChartViewer->setMouseWheelZoomRatio(1.1);

    // Combobox index updating
    setupComboBox();

    connect(qMain->comboBox_interface_2, &QComboBox::currentIndexChanged, this,
            &CPaintNetworkGraphic::updateComboBoxValue);

    // Updating speed
    connect(speed, &CNetworkAdapterSpeed::networkBytesReceivedChanged, this, &CPaintNetworkGraphic::updateSpeed);
    speed->setIntervalForUpdatingSpeed(DataInterval);

    // Set up the chart update timer
    m_ChartUpdateTimer = new QTimer(this);
    connect(m_ChartUpdateTimer, SIGNAL(timeout()), SLOT(onChartUpdateTimer()));

    // Can start now
    m_ChartUpdateTimer->start();

    updateComboBoxValue(my->comboBox_interface_2->currentIndex());

    qMain->verticalLayout_graph->addWidget(m_ChartViewer);
}

CPaintNetworkGraphic::~CPaintNetworkGraphic()
{
    delete m_ChartViewer->getChart();
}

//
// The Pointer, Zoom In or Zoom out button is pressed
//
void CPaintNetworkGraphic::onMouseUsageChanged(QAbstractButton *b)
{
    m_ChartViewer->setMouseUsage(mouseUsage->id(b));
}

//
// The Save button is pressed
//
void CPaintNetworkGraphic::onSave(bool)
{
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save"), "network_chart",
                                     "PNG (*.png);;JPG (*.jpg);;GIF (*.gif);;BMP (*.bmp);;SVG (*.svg);;PDF (*.pdf)");

    if (!fileName.isEmpty())
    {
        // Save the chart
        BaseChart *c = m_ChartViewer->getChart();
        if (0 != c)
            c->makeChart(fileName.toUtf8().constData());
    }
}

//
// User changes the chart update period
//
void CPaintNetworkGraphic::onUpdatePeriodChanged(int)
{
    m_ChartUpdateTimer->start(updatePeriod->currentText().toInt());
    disconnect(speed, &CNetworkAdapterSpeed::networkBytesReceivedChanged, this, &CPaintNetworkGraphic::updateSpeed);
    speed->setIntervalForUpdatingSpeed(updatePeriod->currentText().toInt());
    connect(speed, &CNetworkAdapterSpeed::networkBytesReceivedChanged, this, &CPaintNetworkGraphic::updateSpeed);
}

//
// Update the chart and the viewport periodically
//
void CPaintNetworkGraphic::onChartUpdateTimer()
{
    if (m_currentIndex > 0)
    {
        //
        // As we added more data, we may need to update the full range of the viewport.
        //

        double startDate = m_timeStamps[0];
        double endDate = m_timeStamps[m_currentIndex - 1];

        // Use the initialFullRange (which is 60 seconds in this demo) if this is sufficient.
        double duration = endDate - startDate;
        if (duration < initialFullRange)
            endDate = startDate + initialFullRange;

        // Update the full range to reflect the actual duration of the data. In this case,
        // if the view port is viewing the latest data, we will scroll the view port as new
        // data are added. If the view port is viewing historical data, we would keep the
        // axis scale unchanged to keep the chart stable.
        int updateType = Chart::ScrollWithMax;
        if (m_ChartViewer->getViewPortLeft() + m_ChartViewer->getViewPortWidth() < 0.999)
            updateType = Chart::KeepVisibleRange;
        bool scaleHasChanged = m_ChartViewer->updateFullRangeH("x", startDate, endDate, updateType);

        // Set the zoom in limit as a ratio to the full range
        m_ChartViewer->setZoomInWidthLimit(
            zoomInLimit / (m_ChartViewer->getValueAtViewPort("x", 1) - m_ChartViewer->getValueAtViewPort("x", 0)));

        // Trigger the viewPortChanged event to update the display if the axis scale has changed
        // or if new data are added to the existing axis scale.
        if (scaleHasChanged || (duration < initialFullRange))
            m_ChartViewer->updateViewPort(true, false);
    }
}

//
// View port changed event
//
void CPaintNetworkGraphic::onViewPortChanged()
{
    // In addition to updating the chart, we may also need to update other controls that
    // changes based on the view port.
    updateControls(m_ChartViewer);

    // Update the chart if necessary
    if (m_ChartViewer->needUpdateChart())
        drawChart(m_ChartViewer);
}

//
// User clicks on the the horizontal scroll bar
//
void CPaintNetworkGraphic::onHScrollBarChanged(int value)
{
    if (!m_ChartViewer->isInViewPortChangedEvent())
    {
        // Set the view port based on the scroll bar
        int scrollBarLen = m_HScrollBar->maximum() + m_HScrollBar->pageStep();
        m_ChartViewer->setViewPortLeft(value / (double)scrollBarLen);

        // Update the chart display without updating the image maps. (We can delay updating
        // the image map until scrolling is completed and the chart display is stable.)
        m_ChartViewer->updateViewPort(true, false);
    }
}

//
// Update controls in the user interface when the view port changed
//
void CPaintNetworkGraphic::updateControls(QChartViewer *viewer)
{
    // The logical length of the scrollbar. It can be any large value. The actual value does
    // not matter.
    const int scrollBarLen = 1000000000;

    // Update the horizontal scroll bar
    m_HScrollBar->setEnabled(viewer->getViewPortWidth() < 1);
    m_HScrollBar->setPageStep((int)ceil(viewer->getViewPortWidth() * scrollBarLen));
    m_HScrollBar->setSingleStep((std::min)(scrollBarLen / 100, m_HScrollBar->pageStep()));
    m_HScrollBar->setRange(0, scrollBarLen - m_HScrollBar->pageStep());
    m_HScrollBar->setValue((int)(0.5 + viewer->getViewPortLeft() * scrollBarLen));
}

void CPaintNetworkGraphic::setupComboBox()
{
    my->comboBox_interface_2->clear();

    QMap<int, QString> interfaceList = adapter->getOnlyActiveInterface();

    for (auto it = interfaceList.constBegin(); it != interfaceList.constEnd(); ++it)
        my->comboBox_interface_2->addItem(it.value(), QVariant(it.key()));
}

void CPaintNetworkGraphic::stopPaintingGraph()
{
    m_ChartUpdateTimer->stop();
}

void CPaintNetworkGraphic::startPaintingGraph()
{
    m_ChartUpdateTimer->start();
}

void CPaintNetworkGraphic::updateComboBoxValue(int index)
{
    if (my->comboBox_interface_2->count() == 0)
        return;

    // get index of interf. from combobox
    auto value = my->comboBox_interface_2->itemData(index).toInt();

    // Set speed for selected network adapter
    speed->setNetworkSpeedForAdapter(value, adapter->getRowHardwareAddr(value));
}

void CPaintNetworkGraphic::updateSpeed(uint received, uint sent, float download, float upload)
{
    // The current time
    QDateTime now = QDateTime::currentDateTime();
    // This is our formula for the random number generator
    do
    {
        // We need the currentTime in millisecond resolution
        qint64 t = m_nextDataTime.toMSecsSinceEpoch();
        double currentTime = Chart::chartTime2((int)(t / 1000)) + (t % 1000) / 250 * 0.25;

        // Get a data sample
        double dataA = speed->convert(download, CNetworkAdapterSpeed::SPEED_MEASURE::MEGABIT);
        double dataB = speed->convert(upload, CNetworkAdapterSpeed::SPEED_MEASURE::MEGABIT);

        // In this demo, if the data arrays are full, the oldest 5% of data are discarded.
        if (m_currentIndex >= sampleSize)
        {
            m_currentIndex = sampleSize * 95 / 100 - 1;

            for (int i = 0; i < m_currentIndex; ++i)
            {
                int srcIndex = i + sampleSize - m_currentIndex;
                m_timeStamps[i] = m_timeStamps[srcIndex];
                m_dataSeriesA[i] = m_dataSeriesA[srcIndex];
                m_dataSeriesB[i] = m_dataSeriesB[srcIndex];
            }
        }

        // Store the new values in the current index position, and increment the index.
        m_timeStamps[m_currentIndex] = currentTime;
        m_dataSeriesA[m_currentIndex] = dataA;
        m_dataSeriesB[m_currentIndex] = dataB;
        ++m_currentIndex;

        m_nextDataTime = m_nextDataTime.addMSecs(DataInterval);
    } while (m_nextDataTime < now);
}

//
// Draw chart
//
void CPaintNetworkGraphic::drawChart(QChartViewer *viewer)
{
    // Get the start date and end date that are visible on the chart.
    double viewPortStartDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft());
    double viewPortEndDate = viewer->getValueAtViewPort("x", viewer->getViewPortLeft() + viewer->getViewPortWidth());

    // Extract the part of the data arrays that are visible.
    DoubleArray viewPortTimeStamps;
    DoubleArray viewPortDataSeriesA;
    DoubleArray viewPortDataSeriesB;
    DoubleArray viewPortDataSeriesC;

    if (m_currentIndex > 0)
    {
        // Get the array indexes that corresponds to the visible start and end dates
        int startIndex = (int)floor(Chart::bSearch(DoubleArray(m_timeStamps, m_currentIndex), viewPortStartDate));
        int endIndex = (int)ceil(Chart::bSearch(DoubleArray(m_timeStamps, m_currentIndex), viewPortEndDate));
        int noOfPoints = endIndex - startIndex + 1;

        // Extract the visible data
        viewPortTimeStamps = DoubleArray(m_timeStamps + startIndex, noOfPoints);
        viewPortDataSeriesA = DoubleArray(m_dataSeriesA + startIndex, noOfPoints);
        viewPortDataSeriesB = DoubleArray(m_dataSeriesB + startIndex, noOfPoints);
        viewPortDataSeriesC = DoubleArray(m_dataSeriesC + startIndex, noOfPoints);
    }

    //
    // At this stage, we have extracted the visible data. We can use those data to plot the chart.
    //

    //================================================================================
    // Configure overall chart appearance.
    //================================================================================

    // Create an XYChart object of size 640 x 350 pixels
    XYChart *c = new XYChart(my->frame_5->frameSize().width(), my->frame_5->frameSize().height() + 8);

    // Set the plotarea at (55, 50) with width 80 pixels less than chart width, and height 80 pixels
    // less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
    // as background. Set border to transparent and grid lines to white (ffffff).
    c->setPlotArea(55, 50, c->getWidth() - 85, c->getHeight() - 80,
                   c->linearGradientColor(0, 50, 0, c->getHeight() - 35, 0x80a0bf, 0x5c7d9c), -1, Chart::Transparent,
                   0xffffff, 0xffffff);

    // As the data can lie outside the plotarea in a zoomed chart, we need enable clipping.
    c->setClipping();

    // Add a title to the chart using 18pt Arial font
    c->addTitle(tr("Realtime network speed graphic").toUtf8().constData(), "Arial", 18, 0xffffff);

    // Add a legend box at (55, 25) using horizontal layout. Use 10pt Arial Bold as font. Set the
    // background and border color to transparent and use line style legend key.
    LegendBox *b = c->addLegend(55, 25, false, "Arial Bold", 10);
    b->setBackground(Chart::Transparent);
    b->setLineStyleKey();
    b->setFontColor(0xffffff);

    // Set the x and y axis stems to transparent and the label font to 10pt Arial
    c->xAxis()->setColors(Chart::Transparent);
    c->yAxis()->setColors(Chart::Transparent);
    c->xAxis()->setLabelStyle("Arial", 10);
    c->yAxis()->setLabelStyle("Arial", 10);

    // Set the y-axis tick length to 0 to disable the tick and put the labels closer to the axis.
    c->yAxis()->setTickLength(0);

    // Add axis title using 12pt Arial Bold Italic font
    c->yAxis()->setTitle(tr("Network adapter speed").toUtf8().constData(), "Arial Bold", 12, 0xffffff);

    //================================================================================
    // Add data to chart
    //================================================================================

    //
    // In this example, we represent the data by lines. You may modify the code below to use other
    // representations (areas, scatter plot, etc).
    //

    // Add a line layer for the lines, using a line width of 2 pixels
    LineLayer *layer = c->addLineLayer();
    layer->setLineWidth(2);
    layer->setFastLineMode();

    // Now we add the 3 data series to a line layer, using the color red (ff0000), green (00cc00)
    // and blue (0000ff)
    layer->setXData(viewPortTimeStamps);
    layer->addDataSet(viewPortDataSeriesA, 0xff0000, tr("Download").toUtf8().constData());
    layer->addDataSet(viewPortDataSeriesB, 0x00cc00, tr("Upload").toUtf8().constData());
    //================================================================================
    // Configure axis scale and labelling
    //================================================================================

    // Set the x-axis as a date/time axis with the scale according to the view port x range.
    if (m_currentIndex > 0)
        c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

    // For the automatic axis labels, set the minimum spacing to 75/30 pixels for the x/y axis.
    c->xAxis()->setTickDensity(75);
    c->yAxis()->setTickDensity(30);
    c->yAxis()->setColors(0xffffff, 0xffffff);

    c->xAxis()->setColors(0xffffff, 0xffffff);

    //
    // In this demo, the time range can be from many hours to a few seconds. We can need to define
    // the date/time format the various cases.
    //

    // If all ticks are hour algined, we use "hh:nn<*br*>mmm dd" in bold font as the first label of
    // the Day, and "hh:nn" for other labels.
    c->xAxis()->setFormatCondition("align", 3600);
    c->xAxis()->setMultiFormat(Chart::StartOfDayFilter(), "<*font=bold*>{value|hh:nn<*br*>mmm dd}",
                               Chart::AllPassFilter(), "{value|hh:nn}");

    // If all ticks are minute algined, then we use "hh:nn" as the label format.
    c->xAxis()->setFormatCondition("align", 60);
    c->xAxis()->setLabelFormat("{value|hh:nn}");

    // If all other cases, we use "hh:nn:ss" as the label format.
    c->xAxis()->setFormatCondition("else");
    c->xAxis()->setLabelFormat("{value|hh:nn:ss}");

    // We make sure the tick increment must be at least 1 second.
    c->xAxis()->setMinTickInc(1);

    //================================================================================
    // Output the chart
    //================================================================================

    // We need to update the track line too. If the mouse is moving on the chart (eg. if
    // the user drags the mouse on the chart to scroll it), the track line will be updated
    // in the MouseMovePlotArea event. Otherwise, we need to update the track line here.
    if (!viewer->isInMouseMoveEvent())
    {
        trackLineLabel(c, (0 == viewer->getChart()) ? c->getPlotArea()->getRightX() : viewer->getPlotAreaMouseX());
    }

    // Set the chart image to the QChartViewer
    delete viewer->getChart();
    viewer->setChart(c);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void CPaintNetworkGraphic::onMouseMovePlotArea(QMouseEvent *)
{
    trackLineLabel((XYChart *)m_ChartViewer->getChart(), m_ChartViewer->getPlotAreaMouseX());
    m_ChartViewer->updateDisplay();
}

//
// Draw the track line with data point labels
//
void CPaintNetworkGraphic::trackLineLabel(XYChart *c, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();

    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);
    if (xCoor < plotArea->getLeftX())
        return;

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, 0x888888);

    // Draw a label on the x-axis to show the track line position.
    std::ostringstream xlabel;
    xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue, "hh:nn:ss.ff") << " <*/font*>";
    TTFText *t = d->text(xlabel.str().c_str(), "Arial Bold", 10);

    // Restrict the x-pixel position of the label to make sure it stays inside the chart image.
    int xLabelPos = (std::max)(0, (std::min)(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
    t->draw(xLabelPos, plotArea->getBottomY() + 6, 0xffffff);
    t->destroy();

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i)
    {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j)
        {
            DataSet *dataSet = layer->getDataSetByZ(j);
            const char *dataSetName = dataSet->getDataName();

            // Get the color, name and position of the data label
            int color = dataSet->getDataColor();
            int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

            // Draw a track dot with a label next to it for visible data points in the plot area
            if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color != Chart::Transparent) &&
                dataSetName && *dataSetName)
            {
                d->circle(xCoor, yCoor, 4, 4, color, color);

                auto f = speed->convertSpeed(dataSet->getValue(xIndex) * 1000000).toLatin1().constData();
                std::ostringstream label;
                label << "<*font,bgColor=" << std::hex << color << "*> "
                      << f /*c->formatValue(dataSet->getValue(xIndex), "{value|P4}")*/ << " <*font*>";

                t = d->text(label.str().c_str(), "Arial Bold", 10);

                // t = d->text(f, "Arial Bold", 10);

                qDebug() << "Hello from graph " << label.str().c_str() << "\n";
                //
                //  Draw the label on the right side of the dot if the mouse is on the left side the
                //  chart, and vice versa. This ensures the label will not go outside the chart image.

                if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
                    t->draw(xCoor + 6, yCoor, 0xffffff, Chart::Left);
                else
                    t->draw(xCoor - 6, yCoor, 0xffffff, Chart::Right);

                t->destroy();
            }
        }
    }
}
