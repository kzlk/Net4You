#include <QApplication>
#include <QHBoxLayout>
#include <QObjectList>
#include <QDateTime>
#include <QPushButton>
#include "financedemo.h"
#include "FinanceChart.h"
#include <string>
#include <vector>
#include <sstream>


//
// Contents of the CFinanceDemoDlg combo boxes
//

static const char* timeRanges[] =
{
    "30", "1 month",
    "60", "2 months",
    "90", "3 months",
    "180", "6 months",
    "360", "1 year",
    "720", "2 years",
    "1080", "3 years",
    "1440", "4 years",
    "1800", "5 years",
    "3600", "10 years"
};
static int timeRangeCount = (int)(sizeof(timeRanges) / sizeof(*timeRanges));

static const char* chartTypes[] =
{
    "None", "None",
    "CandleStick", "CandleStick",
    "Close", "Closing Price",
    "Median", "Median Price",
    "OHLC", "OHLC",
    "TP", "Typical Price",
    "WC", "Weighted Close"
};
static int chartTypeCount = (int)(sizeof(chartTypes) / sizeof(*chartTypes));

static const char* bandTypes[] =
{
    "None", "None",
    "BB", "Bollinger Band",
    "DC", "Donchain Channel",
    "Envelop", "Envelop (SMA 20 +/- 10%)"
};
static int bandTypeCount = (int)(sizeof(bandTypes) / sizeof(*bandTypes));

static const char* avgTypes[] =
{
    "None", "None",
    "SMA", "Simple",
    "EMA", "Exponential",
    "TMA", "Triangular",
    "WMA", "Weighted"
};
static int avgTypeCount = (int)(sizeof(avgTypes) / sizeof(*avgTypes));

static const char* indicatorTypes[] =
{
    "None", "None",
    "AccDist", "Accumulation/Distribution",
    "AroonOsc", "Aroon Oscillator",
    "Aroon", "Aroon Up/Down",
    "ADX", "Avg Directional Index",
    "ATR", "Avg True Range",
    "BBW", "Bollinger Band Width",
    "CMF", "Chaikin Money Flow",
    "COscillator", "Chaikin Oscillator",
    "CVolatility", "Chaikin Volatility",
    "CLV", "Close Location Value",
    "CCI", "Commodity Channel Index",
    "DPO", "Detrended Price Osc",
    "DCW", "Donchian Channel Width",
    "EMV", "Ease of Movement",
    "FStoch", "Fast Stochastic",
    "MACD", "MACD",
    "MDX", "Mass Index",
    "Momentum", "Momentum",
    "MFI", "Money Flow Index",
    "NVI", "Neg Volume Index",
    "OBV", "On Balance Volume",
    "Performance", "Performance",
    "PPO", "% Price Oscillator",
    "PVO", "% Volume Oscillator",
    "PVI", "Pos Volume Index",
    "PVT", "Price Volume Trend",
    "ROC", "Rate of Change",
    "RSI", "RSI",
    "SStoch", "Slow Stochastic",
    "StochRSI", "StochRSI",
    "TRIX", "TRIX",
    "UO", "Ultimate Oscillator",
    "Vol", "Volume",
    "WilliamR", "William's %R"
};
static int indicatorTypeCount = (int)(sizeof(indicatorTypes) / sizeof(*indicatorTypes));

//
// Helper utility to initialize a combo box from an array of text
//
static void initComboBox(QComboBox* b, const char* list[], int count, const char* initial)
{
    b->clear();
    for (int i = 0; i < count; i += 2)
        //The odd index are the display text, the even index are the keys
        b->addItem(list[i + 1], list[i]);
    b->setCurrentIndex(b->findText(initial));
}


FinanceDemo::FinanceDemo(QWidget *parent) :
        QDialog(parent, Qt::Window)
{
    //
    // Set up the GUI
    //

    setWindowTitle("ChartDirector Interactive Financial Chart Demonstration");
    resize(954, 510);
    setStyleSheet("QDialog {background:#FFFFFF;} QLabel {font-weight:bold;}" );

    // Use horizontal layout to put the tree view on the left and the charts on the right
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->setSpacing(0);

    // The frame on the left side
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setStyleSheet("#leftPanel {background-color:#BADDFF}");
    leftPanel->setFixedWidth(155);
    horizontalLayout->addWidget(leftPanel);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle(QFrame::VLine | QFrame::Plain);
    horizontalLayout->addWidget(separator);

    QWidget *rightPanel = new QWidget(this);
    horizontalLayout->addWidget(rightPanel);
    rightPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    int yCursor = 8;

    // Pointer push button
    QPushButton *pointerPB = new QPushButton(QIcon(":/icons/scroll_icon.png"), "", leftPanel);
    pointerPB->setGeometry(8, yCursor, 32, 32);
    pointerPB->setCheckable(true);

    // Zoom In push button
    QPushButton *zoomInPB = new QPushButton(QIcon(":/icons/zoomin_icon.png"), "", leftPanel);
    zoomInPB->setGeometry(61, yCursor, 32, 32);
    zoomInPB->setCheckable(true);

    // Zoom Out push button
    QPushButton *zoomOutPB = new QPushButton(QIcon(":/icons/zoomout_icon.png"), "", leftPanel);
    zoomOutPB->setGeometry(114, yCursor, 32, 32);
    zoomOutPB->setCheckable(true);

    // The Pointer/Zoom In/Zoom Out buttons form a button group
    mouseUsage = new QButtonGroup(leftPanel);
    mouseUsage->addButton(pointerPB, Chart::MouseUsageScroll);
    mouseUsage->addButton(zoomInPB, Chart::MouseUsageZoomIn);
    mouseUsage->addButton(zoomOutPB, Chart::MouseUsageZoomOut);
    connect(mouseUsage, SIGNAL(buttonPressed(QAbstractButton*)),
        SLOT(onMouseUsageChanged(QAbstractButton*)));

    // Ticker Symbol
    (new QLabel("Ticker Symbol", leftPanel))->setGeometry(8, yCursor += 40, 140, 18);
    m_TickerSymbol = new QLineEdit("ASE", leftPanel);
    m_TickerSymbol->setGeometry(8, yCursor += 16, 140, 20);

    // Compare With
    (new QLabel("Compare With", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_CompareWith = new QLineEdit(leftPanel);
    m_CompareWith->setGeometry(8, yCursor += 16, 140, 20);

    // Time Period
    (new QLabel("Time Period", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_TimeRange = new QComboBox(leftPanel);
    m_TimeRange->setGeometry(8, yCursor += 16, 140, 20);
    connect(m_TimeRange, SIGNAL(currentIndexChanged(int)), SLOT(onTimeRangeChanged(int)));

    // Value bars/Log Scale/Grid Lines
    m_VolumeBars = new QCheckBox("Show Volume Bars", leftPanel);
    m_ParabolicSAR = new QCheckBox("Parabolic SAR", leftPanel);
    m_LogScale = new QCheckBox("Log Scale", leftPanel);
    m_PercentageScale = new QCheckBox("Percentage Grid", leftPanel);
    m_VolumeBars->setChecked(true);
    m_VolumeBars->setGeometry(8, yCursor += 28, 140, 20);
    m_ParabolicSAR->setGeometry(8, yCursor += 20, 140, 20);
    m_LogScale->setGeometry(8, yCursor += 20, 140, 20);
    m_PercentageScale->setGeometry(8, yCursor += 20, 140, 20);

    // Chart Type
    (new QLabel("Chart Type", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_ChartType = new QComboBox(leftPanel);
    m_ChartType->setGeometry(8, yCursor += 16, 140, 20);

    // Price Bands
    (new QLabel("Price Bands", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_PriceBand = new QComboBox(leftPanel);
    m_PriceBand->setGeometry(8, yCursor += 16, 140, 20);

    // Moving Averages
    (new QLabel("Moving Averages", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_AvgType1 = new QComboBox(leftPanel);
    m_AvgType2 = new QComboBox(leftPanel);
    m_MovAvg1 = new QLineEdit("10", leftPanel);
    m_MovAvg2 = new QLineEdit("25", leftPanel);
    m_AvgType1->setGeometry(8, yCursor += 16, 105, 20);
    m_MovAvg1->setGeometry(113, yCursor, 35, 20);
    m_AvgType2->setGeometry(8, yCursor += 22, 105, 20);
    m_MovAvg2->setGeometry(113, yCursor, 35, 20);

    // Technical Indicators
    (new QLabel("Technical Indicators", leftPanel))->setGeometry(8, yCursor += 28, 140, 18);
    m_Indicator1 = new QComboBox(leftPanel);
    m_Indicator2 = new QComboBox(leftPanel);
    m_Indicator1->setGeometry(8, yCursor += 16, 140, 20);
    m_Indicator2->setGeometry(8, yCursor += 22, 140, 20);

    // Chart Viewer
    m_ChartViewer = new QChartViewer(rightPanel);
    m_ChartViewer->move(8, 12);
    connect(m_ChartViewer, SIGNAL(viewPortChanged()), SLOT(onViewPortChanged()));
    connect(m_ChartViewer, SIGNAL(mouseMovePlotArea(QMouseEvent*)),
        SLOT(onMouseMovePlotArea(QMouseEvent*)));

    //
    // Fill the contents of the combo boxes
    //

    //
    // Intialialize the controls
    //
    initComboBox(m_TimeRange, timeRanges, timeRangeCount, "3 months");
    initComboBox(m_ChartType, chartTypes, chartTypeCount, "CandleStick");
    initComboBox(m_PriceBand, bandTypes, bandTypeCount, "Bollinger Band");
    initComboBox(m_AvgType1, avgTypes, avgTypeCount, "Simple");
    initComboBox(m_AvgType2, avgTypes, avgTypeCount, "Simple");
    initComboBox(m_Indicator1, indicatorTypes, indicatorTypeCount, "RSI");
    initComboBox(m_Indicator2, indicatorTypes, indicatorTypeCount, "MACD");

    // Use the same action hanlder for all controls
    const QObjectList &allControls = leftPanel->children();
    for (int i = 0; i < allControls.count(); ++i)
    {
        if (allControls[i] == m_TimeRange)
            continue;

        QObject *obj;
        if ((obj = qobject_cast<QComboBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(currentIndexChanged(int)), SLOT(onComboBoxChanged(int)));
        else if ((obj = qobject_cast<QCheckBox *>(allControls[i])) != 0)
            connect(obj, SIGNAL(clicked()), SLOT(onCheckBoxChanged()));
        else if ((obj = qobject_cast<QLineEdit *>(allControls[i])) != 0)
            connect(obj, SIGNAL(editingFinished()), SLOT(onLineEditChanged()));
    }

    // Enable mouse wheel zooming by setting the zoom ratio to 1.1 per wheel event
    m_ChartViewer->setMouseWheelZoomRatio(1.1);

    // Initially set the mouse to drag to scroll mode.
    pointerPB->setChecked(true);
    m_ChartViewer->setMouseUsage(Chart::MouseUsageScroll);

    onLineEditChanged();

    // Update the chart
    drawChart(m_ChartViewer);
}

//
// Destructor
//
FinanceDemo::~FinanceDemo()
{
    delete m_ChartViewer->getChart();
}

//
// The Pointer, Zoom In or Zoom out button is pressed
//
void FinanceDemo::onMouseUsageChanged(QAbstractButton *b)
{
    m_ChartViewer->setMouseUsage(mouseUsage->id(b));
}

void FinanceDemo::onTimeRangeChanged(int)
{
    int durationInDays = m_TimeRange->itemData(m_TimeRange->currentIndex()).toInt();
    m_ChartViewer->setViewPortWidth(durationInDays / (double)m_dailyPrice.timeStamps.size());
    m_ChartViewer->setViewPortLeft(1 - m_ChartViewer->getViewPortWidth());
    m_ChartViewer->updateViewPort(true, true);
}

void FinanceDemo::onCheckBoxChanged()
{
    drawChart(m_ChartViewer);
}

void FinanceDemo::onComboBoxChanged(int)
{
    drawChart(m_ChartViewer);
}

void FinanceDemo::onLineEditChanged()
{
    int new_avgPeriod1 = m_MovAvg1->text().toInt();
    int new_avgPeriod2 = m_MovAvg2->text().toInt();

    QString tickerKey = m_TickerSymbol->text();
    if (tickerKey.isEmpty())
    {
        // Default tickerKey
        tickerKey = "ASE";
        m_TickerSymbol->setText(tickerKey);
    }

    QString compareKey = m_CompareWith->text();

    bool needReloadData = (m_tickerKey != tickerKey) || (m_compareKey != compareKey);
    if (needReloadData)
        loadData(tickerKey, compareKey);

    if (needReloadData || (new_avgPeriod1 != m_avgPeriod1) || (new_avgPeriod2 != m_avgPeriod2))
        drawChart(m_ChartViewer);
}

/////////////////////////////////////////////////////////////////////////////
// Data source

void FinanceDemo::loadData(const QString& ticker, const QString& compare)
{
    // The financial simulator
    if (m_tickerKey != ticker)
    {
        m_tickerKey = ticker;

        // Simulator to generate realistic random OHLC values
        FinanceSimulator db(ticker.toUtf8().data(), Chart::chartTime(2010, 1, 1),
            Chart::chartTime(2020, 12, 31), 86400);

        // Copy daily data to the data arrays
        m_dailyPrice.timeStamps = arrayToVector(db.getTimeStamps());
        m_dailyPrice.highData = arrayToVector(db.getHighData());
        m_dailyPrice.lowData = arrayToVector(db.getLowData());
        m_dailyPrice.openData = arrayToVector(db.getOpenData());
        m_dailyPrice.closeData = arrayToVector(db.getCloseData());
        m_dailyPrice.volData = arrayToVector(db.getVolData());

        //
        // Set up the viewport to display the duration selected by the user
        //

        m_ChartViewer->setFullRange("x", 0, (int)m_dailyPrice.timeStamps.size() - 1);
        int durationInDays = m_TimeRange->itemData(m_TimeRange->currentIndex()).toInt();
        m_ChartViewer->setViewPortWidth(durationInDays / (double)m_dailyPrice.timeStamps.size());
        m_ChartViewer->setViewPortLeft(1 - m_ChartViewer->getViewPortWidth());
    }

    if (m_compareKey != compare)
    {
        m_compareKey = compare;
        if (m_compareKey.isEmpty())
            m_dailyPrice.compareData.clear();
        else
        {
            // Simulator to generate realistic random OHLC values
            FinanceSimulator db2(compare.toUtf8().data(), Chart::chartTime(2010, 1, 1),
                Chart::chartTime(2020, 12, 31), 86400);
            m_dailyPrice.compareData = arrayToVector(db2.getCloseData());
        }
    }

    // In this example, we will compute the weekly and monthly prices on demand. We just
    // need to clear the old data here.
    m_weeklyPrice.timeStamps.clear();
    m_monthlyPrice.timeStamps.clear();
}

/// <summary>
/// An internal method used to aggregate daily data.
/// </summary>
void FinanceDemo::aggregateData(const ArrayMath& aggregator, PriceData& p)
{
    int size = aggregator.aggregate(vectorToArray(p.timeStamps), Chart::AggregateFirst).len;
    p.timeStamps.resize(size);
    size = aggregator.aggregate(vectorToArray(p.highData), Chart::AggregateMax).len;
    p.highData.resize(size);
    size = aggregator.aggregate(vectorToArray(p.lowData), Chart::AggregateMin).len;
    p.lowData.resize(size);
    size = aggregator.aggregate(vectorToArray(p.openData), Chart::AggregateFirst).len;
    p.openData.resize(size);
    size = aggregator.aggregate(vectorToArray(p.closeData), Chart::AggregateLast).len;
    p.closeData.resize(size);
    size = aggregator.aggregate(vectorToArray(p.volData), Chart::AggregateSum).len;
    p.volData.resize(size);
    size = aggregator.aggregate(vectorToArray(p.compareData), Chart::AggregateLast).len;
    p.compareData.resize(size);
}

/////////////////////////////////////////////////////////////////////////////
// Chart Creation

/// <summary>
/// Add a moving average line to the FinanceChart object.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="avgType">The moving average type (SMA/EMA/TMA/WMA).</param>
/// <param name="avgPeriod">The moving average period.</param>
/// <param name="color">The color of the line.</param>
static LineLayer* addMovingAvg(FinanceChart *m, QString avgType, int avgPeriod, int color)
{
    if (avgPeriod > 1)
    {
        if (avgType == "SMA")
            return m->addSimpleMovingAvg(avgPeriod, color);
        else if (avgType == "EMA")
            return m->addExpMovingAvg(avgPeriod, color);
        else if (avgType == "TMA")
            return m->addTriMovingAvg(avgPeriod, color);
        else if (avgType == "WMA")
            return m->addWeightedMovingAvg(avgPeriod, color);
    }

    return 0;
}

/// <summary>
/// Add an indicator chart to the FinanceChart object. In this demo example, the indicator
/// parameters (such as the period used to compute RSI, colors of the lines, etc.) are hard
/// coded to commonly used values. You are welcome to design a more complex user interface
/// to allow users to set the parameters.
/// </summary>
/// <param name="m">The FinanceChart object to add the line to.</param>
/// <param name="indicator">The selected indicator.</param>
/// <param name="height">Height of the chart in pixels</param>
static XYChart* addIndicator(FinanceChart *m, QString indicator, int height)
{
    if (indicator == "RSI")
        return m->addRSI(height, 14, 0x800080, 20, 0xff6666, 0x6666ff);
    else if (indicator == "StochRSI")
        return m->addStochRSI(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "MACD")
        return m->addMACD(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "FStoch")
        return m->addFastStochastic(height, 14, 3, 0x6060, 0x606000);
    else if (indicator == "SStoch")
        return m->addSlowStochastic(height, 14, 3, 0x6060, 0x606000);
    else if (indicator == "ATR")
        return m->addATR(height, 14, 0x808080, 0xff);
    else if (indicator == "ADX")
        return m->addADX(height, 14, 0x8000, 0x800000, 0x80);
    else if (indicator == "DCW")
        return m->addDonchianWidth(height, 20, 0xff);
    else if (indicator == "BBW")
        return m->addBollingerWidth(height, 20, 2, 0xff);
    else if (indicator == "DPO")
        return m->addDPO(height, 20, 0xff);
    else if (indicator == "PVT")
        return m->addPVT(height, 0xff);
    else if (indicator == "Momentum")
        return m->addMomentum(height, 12, 0xff);
    else if (indicator == "Performance")
        return m->addPerformance(height, 0xff);
    else if (indicator == "ROC")
        return m->addROC(height, 12, 0xff);
    else if (indicator == "OBV")
        return m->addOBV(height, 0xff);
    else if (indicator == "AccDist")
        return m->addAccDist(height, 0xff);
    else if (indicator == "CLV")
        return m->addCLV(height, 0xff);
    else if (indicator == "WilliamR")
        return m->addWilliamR(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "Aroon")
        return m->addAroon(height, 14, 0x339933, 0x333399);
    else if (indicator == "AroonOsc")
        return m->addAroonOsc(height, 14, 0xff);
    else if (indicator == "CCI")
        return m->addCCI(height, 20, 0x800080, 100, 0xff6666, 0x6666ff);
    else if (indicator == "EMV")
        return m->addEaseOfMovement(height, 9, 0x6060, 0x606000);
    else if (indicator == "MDX")
        return m->addMassIndex(height, 0x800080, 0xff6666, 0x6666ff);
    else if (indicator == "CVolatility")
        return m->addChaikinVolatility(height, 10, 10, 0xff);
    else if (indicator == "COscillator")
        return m->addChaikinOscillator(height, 0xff);
    else if (indicator == "CMF")
        return m->addChaikinMoneyFlow(height, 21, 0x8000);
    else if (indicator == "NVI")
        return m->addNVI(height, 255, 0xff, 0x883333);
    else if (indicator == "PVI")
        return m->addPVI(height, 255, 0xff, 0x883333);
    else if (indicator == "MFI")
        return m->addMFI(height, 14, 0x800080, 30, 0xff6666, 0x6666ff);
    else if (indicator == "PVO")
        return m->addPVO(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "PPO")
        return m->addPPO(height, 26, 12, 9, 0xff, 0xff00ff, 0x8000);
    else if (indicator == "UO")
        return m->addUltimateOscillator(height, 7, 14, 28, 0x800080, 20, 0xff6666, 0x6666ff);
    else if (indicator == "Vol")
        return m->addVolIndicator(height, 0x99ff99, 0xff9999, 0xc0c0c0);
    else if (indicator == "TRIX")
        return m->addTRIX(height, 12, 0xff);
    else
        return 0;
}

/// <summary>
/// Creates a dummy chart to show an error message.
/// </summary>
/// <param name="viewer">The WinChartViewer to display the error message.</param>
/// <param name="msg">The error message</param>
static void errMsg(QChartViewer* viewer, const char *msg)
{
     MultiChart m(400, 200);
     m.addTitle(Chart::Center, msg, "Arial", 10)->setMaxWidth(m.getWidth());
     viewer->setChart(&m);
}

/// <summary>
/// Draw the chart according to user selection and display it in the ChartViewer.
/// </summary>
/// <param name="viewer">The ChartViewer object to display the chart.</param>
void FinanceDemo::drawChart(QChartViewer *viewer)
{
    // The first moving average period selected by the user.
    m_avgPeriod1 = m_MovAvg1->text().toInt();
    if (m_avgPeriod1 < 0)
        m_avgPeriod1 = 0;
    if (m_avgPeriod1 > 300)
        m_avgPeriod1 = 300;

    // The second moving average period selected by the user.
    m_avgPeriod2 = m_MovAvg2->text().toInt();
    if (m_avgPeriod2 < 0)
        m_avgPeriod2 = 0;
    if (m_avgPeriod2 > 300)
        m_avgPeriod2 = 300;

    // We need extra leading data points in order to compute moving averages.
    int extraPoints = (m_avgPeriod1 > m_avgPeriod2) ? m_avgPeriod1 : m_avgPeriod2;
    if (extraPoints < 25)
        extraPoints = 25;

    // Get the start and end index visible on the viewport
    int startIndex = (int)floor(viewer->getValueAtViewPort("x", viewer->getViewPortLeft()));
    int endIndex = (int)ceil(viewer->getValueAtViewPort("x", viewer->getViewPortRight()));

    PriceData* p = &m_dailyPrice;
    if (endIndex - startIndex >= 400)
    {
        if (endIndex - startIndex < 2000)
        {
            p = &m_weeklyPrice;
            if (p->timeStamps.empty())
            {
                ArrayMath a(vectorToArray(m_dailyPrice.timeStamps));
                aggregateData(a.selectStartOfWeek(), *p = m_dailyPrice);
            }
        }
        else
        {
            p = &m_monthlyPrice;
            if (p->timeStamps.empty())
            {
                ArrayMath a(vectorToArray(m_dailyPrice.timeStamps));
                aggregateData(a.selectStartOfMonth(), *p = m_dailyPrice);
            }
        }

        // Re-compute the index based on weekly data
        DoubleArray ts = vectorToArray(p->timeStamps);
        startIndex = (int)floor(Chart::bSearch(ts, m_dailyPrice.timeStamps[startIndex]));
        endIndex = (int)ceil(Chart::bSearch(ts, m_dailyPrice.timeStamps[endIndex]));
    }

    //
    // At this stage, we have determined the price data to use. We need to obtain the part
    // of the arrays that are within the visible x-axis range.
    //

    // Add extra points before the startIndex, as moving averages and may technical indicators
    // requires data before the first visible point.
    extraPoints = (std::min)(extraPoints, startIndex);
    startIndex -= extraPoints;
    int duration = endIndex - startIndex + 1;

    // Obtain the visible part of the data plus the extra points
    DoubleArray timeStamps = vectorToArray(p->timeStamps, startIndex, duration);
    DoubleArray highData = vectorToArray(p->highData, startIndex, duration);
    DoubleArray lowData = vectorToArray(p->lowData, startIndex, duration);
    DoubleArray openData = vectorToArray(p->openData, startIndex, duration);
    DoubleArray closeData = vectorToArray(p->closeData, startIndex, duration);
    DoubleArray volData = vectorToArray(p->volData, startIndex, duration);
    DoubleArray compareData = vectorToArray(p->compareData, startIndex, duration);

    //
    // Now, we have obtain the data. We can plot the chart.
    //
    int width = 780;
    int mainHeight = 260;
    int indicatorHeight = 80;

    // Create the chart object using the selected size
    FinanceChart *m = new FinanceChart(width);

    // Disable default legend box, as we are using dynamic legend
    m->setLegendStyle("normal", 8, Chart::Transparent, Chart::Transparent);

    // Set the data into the chart object
    m->setData(timeStamps, highData, lowData, openData, closeData, volData, extraPoints);

    //
    // We configure the title of the chart. In this demo chart design, we put the
    // company name as the top line of the title with left alignment.
    //
    m->addPlotAreaTitle(Chart::BottomLeft, m_tickerKey.toUtf8().data());

    // A copyright message at the bottom left corner the title area
    m->addPlotAreaTitle(Chart::BottomRight,
        "<*font=Arial,size=8*>(c) Advanced Software Engineering");

    //
    // Add the main chart
    //
    m->addMainChart(mainHeight);

    //
    // Set log or linear scale according to user preference
    //
    m->setLogScale(m_LogScale->isChecked());

    //
    // Set axis labels to show data values or percentage change to user preference
    //
    if (m_PercentageScale->isChecked())
            m->setPercentageAxis();

    //
    // Draw the main chart depending on the chart type the user has selected
    //
    QString selectedType = m_ChartType->itemData(m_ChartType->currentIndex()).toString();
    if (selectedType == "Close")
        m->addCloseLine(0x000040);
    else if (selectedType == "TP")
        m->addTypicalPrice(0x000040);
    else if (selectedType == "WC")
        m->addWeightedClose(0x000040);
    else if (selectedType == "Median")
        m->addMedianPrice(0x000040);

    //
    // Add comparison line if there is data for comparison
    //
    if (compareData.len > 0)
        m->addComparison(compareData, 0x0000ff, m_compareKey.toUtf8().data());

    //
    // Add moving average lines.
    //
    addMovingAvg(m, m_AvgType1->itemData(m_AvgType1->currentIndex()).toString(),
                 m_avgPeriod1, 0x663300);
    addMovingAvg(m, m_AvgType2->itemData(m_AvgType2->currentIndex()).toString(),
                 m_avgPeriod2, 0x9900ff);

    //
    // Draw the main chart if the user has selected CandleStick or OHLC. We
    // draw it here to make sure it is drawn behind the moving average lines
    // (that is, the moving average lines stay on top.)
    //
    if (selectedType == "CandleStick")
        m->addCandleStick(0x33ff33, 0xff3333);
    else if (selectedType == "OHLC")
        m->addHLOC(0x8000, 0x800000);

    //
    // Add parabolic SAR if necessary
    //
    if (m_ParabolicSAR->isChecked())
        m->addParabolicSAR(0.02, 0.02, 0.2, Chart::DiamondShape, 5, 0x008800, 0x000000);

    //
    // Add price band/channel/envelop to the chart according to user selection
    //
    QString selectedBand = m_PriceBand->itemData(m_PriceBand->currentIndex()).toString();
    if (selectedBand == "BB")
        m->addBollingerBand(20, 2, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "DC")
        m->addDonchianChannel(20, 0x9999ff, 0xc06666ff);
    else if (selectedBand == "Envelop")
        m->addEnvelop(20, 0.1, 0x9999ff, 0xc06666ff);

    //
    // Add volume bars to the main chart if necessary
    //
    if (m_VolumeBars->isChecked())
        m->addVolBars(indicatorHeight, 0x99ff99, 0xff9999, 0xc0c0c0);

    //
    // Add additional indicators as according to user selection.
    //
    addIndicator(m, m_Indicator1->itemData(m_Indicator1->currentIndex()).toString(),
                 indicatorHeight);
    addIndicator(m, m_Indicator2->itemData(m_Indicator2->currentIndex()).toString(),
                 indicatorHeight);

    // We may need to update the track line. If the mouse is moving on the chart, the track line
    // will be updated in MouseMovePlotArea. Otherwise, we need to update the track line here.
    if (!viewer->isInMouseMoveEvent())
    {
        trackFinance(m, (0 == viewer->getChart()) ?
            ((XYChart *)m->getChart(0))->getPlotArea()->getRightX() : viewer->getPlotAreaMouseX());
    }

    // Set the chart to the viewer
    delete viewer->getChart();
    viewer->setChart(m);
}

//
// Draw track cursor when mouse is moving over plotarea
//
void FinanceDemo::onMouseMovePlotArea(QMouseEvent*)
{
    trackFinance((MultiChart *)m_ChartViewer->getChart(), m_ChartViewer->getPlotAreaMouseX());
    m_ChartViewer->updateDisplay();
}

//
// Draw finance chart track line with legend
//
void FinanceDemo::trackFinance(MultiChart* m, int mouseX)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = m->initDynamicLayer();

    // It is possible for a FinanceChart to be empty, so we need to check for it.
    if (m->getChartCount() == 0)
        return;

    // Get the data x-value that is nearest to the mouse
    int xValue = (int)(((XYChart *)m->getChart(0))->getNearestXValue(mouseX));

    // Iterate the XY charts (main price chart and indicator charts) in the FinanceChart
    XYChart *c = 0;
    for (int i = 0; i < m->getChartCount(); ++i) {
        c = (XYChart *)m->getChart(i);

        // Variables to hold the legend entries
        std::ostringstream ohlcLegend;
        std::vector<std::string> legendEntries;

        // Iterate through all layers to find the highest data point
        for (int j = 0; j < c->getLayerCount(); ++j) {
            Layer *layer = c->getLayerByZ(j);
            int xIndex = layer->getXIndexOf(xValue);
            int dataSetCount = layer->getDataSetCount();

            // In a FinanceChart, only layers showing OHLC data can have 4 data sets
            if (dataSetCount == 4) {
                double highValue = layer->getDataSet(0)->getValue(xIndex);
                double lowValue = layer->getDataSet(1)->getValue(xIndex);
                double openValue = layer->getDataSet(2)->getValue(xIndex);
                double closeValue = layer->getDataSet(3)->getValue(xIndex);

                if (closeValue != Chart::NoValue) {
                    // Build the OHLC legend
                    ohlcLegend << "      <*block*>";
                    ohlcLegend << "Open: " << c->formatValue(openValue, "{value|P4}");
                    ohlcLegend << ", High: " << c->formatValue(highValue, "{value|P4}");
                    ohlcLegend << ", Low: " << c->formatValue(lowValue, "{value|P4}");
                    ohlcLegend << ", Close: " << c->formatValue(closeValue, "{value|P4}");

                    // We also draw an upward or downward triangle for up and down days and the %
                    // change
                    double lastCloseValue = layer->getDataSet(3)->getValue(xIndex - 1);
                    if (lastCloseValue != Chart::NoValue) {
                        double change = closeValue - lastCloseValue;
                        double percent = change * 100 / closeValue;
                        std::string symbol = (change >= 0) ?
                            "<*font,color=008800*><*img=@triangle,width=8,color=008800*>" :
                            "<*font,color=CC0000*><*img=@invertedtriangle,width=8,color=CC0000*>";

                        ohlcLegend << "  " << symbol << " " << c->formatValue(change, "{value|P4}");
                        ohlcLegend << " (" << c->formatValue(percent, "{value|2}") << "%)<*/font*>";
                    }

                    ohlcLegend << "<*/*>";
                }
            }
            else {
                // Iterate through all the data sets in the layer
                for (int k = 0; k < layer->getDataSetCount(); ++k) {
                    DataSet *dataSet = layer->getDataSetByZ(k);

                    std::string name = dataSet->getDataName();
                    double value = dataSet->getValue(xIndex);
                    if ((0 != name.size()) && (value != Chart::NoValue)) {

                        // In a FinanceChart, the data set name consists of the indicator name and
                        // its latest value. It is like "Vol: 123M" or "RSI (14): 55.34". As we are
                        // generating the values dynamically, we need to extract the indictor name
                        // out, and also the volume unit (if any).

                        // The volume unit
                        std::string unitChar;

                        // The indicator name is the part of the name up to the colon character.
                        int delimiterPosition = (int)name.find(':');
                        if (name.npos != delimiterPosition) {

                            // The unit, if any, is the trailing non-digit character(s).
                            int lastDigitPos = (int)name.find_last_of("0123456789");
                            if ((name.npos != lastDigitPos) && (lastDigitPos + 1 < (int)name.size()) &&
                                (lastDigitPos > delimiterPosition))
                                unitChar = name.substr(lastDigitPos + 1);

                            name.resize(delimiterPosition);
                        }

                        // In a FinanceChart, if there are two data sets, it must be representing a
                        // range.
                        if (dataSetCount == 2) {
                            // We show both values in the range in a single legend entry
                            value = layer->getDataSet(0)->getValue(xIndex);
                            double value2 = layer->getDataSet(1)->getValue(xIndex);
                            name = name + ": " + c->formatValue((std::min)(value, value2), "{value|P3}");
                            name = name + " - " + c->formatValue((std::max)(value, value2), "{value|P3}");
                        }
                        else {
                            // In a FinanceChart, only the layer for volume bars has 3 data sets for
                            // up/down/flat days
                            if (dataSetCount == 3) {
                                // The actual volume is the sum of the 3 data sets.
                                value = layer->getDataSet(0)->getValue(xIndex) + layer->getDataSet(1
                                    )->getValue(xIndex) + layer->getDataSet(2)->getValue(xIndex);
                            }

                            // Create the legend entry
                            name = name + ": " + c->formatValue(value, "{value|P3}") + unitChar;
                        }

                        // Build the legend entry, consist of a colored square box and the name (with
                        // the data value in it).
                        std::ostringstream legendEntry;
                        legendEntry << "<*block*><*img=@square,width=8,edgeColor=000000,color="
                            << std::hex << dataSet->getDataColor() << "*> " << name << "<*/*>";
                        legendEntries.push_back(legendEntry.str());
                    }
                }
            }
        }

        // Get the plot area position relative to the entire FinanceChart
        PlotArea *plotArea = c->getPlotArea();
        int plotAreaLeftX = plotArea->getLeftX() + c->getAbsOffsetX();
        int plotAreaTopY = plotArea->getTopY() + c->getAbsOffsetY();

        // The legend begins with the date label, then the ohlcLegend (if any), and then the
        // entries for the indicators.
        std::ostringstream legendText;
        legendText << "<*block,valign=top,maxWidth=" << (plotArea->getWidth() - 5)
            << "*><*font=Arial Bold*>[" << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy")
            << "]<*/font*>" << ohlcLegend.str();
        for (int i = ((int)legendEntries.size()) - 1; i >= 0; --i) {
            legendText << "      " << legendEntries[i];
        }
        legendText << "<*/*>";

        // Draw a vertical track line at the x-position
        d->vline(plotAreaTopY, plotAreaTopY + plotArea->getHeight(), c->getXCoor(xValue) +
            c->getAbsOffsetX(), d->dashLineColor(0x000000, 0x0101));

        // Display the legend on the top of the plot area
        TTFText *t = d->text(legendText.str().c_str(), "Arial", 8);
        t->draw(plotAreaLeftX + 5, plotAreaTopY + 3, 0x000000, Chart::TopLeft);
        t->destroy();
    }
}

//
// The ViewPortChanged event handler. This event occurs if the user scrolls or zooms in or
// out the chart by dragging or clicking on the chart. It can also be triggered by calling
// CChartViewer.updateViewPort.
//
void FinanceDemo::onViewPortChanged()
{
    // Update the chart if necessary
    if (m_ChartViewer->needUpdateChart())
        drawChart(m_ChartViewer);
}

//
// Convert std::vector to a DoubleArray
//
DoubleArray FinanceDemo::vectorToArray(const std::vector<double>& v, int startIndex, int length)
{
    if ((length < 0) || (length + startIndex > (int)v.size()))
        length = ((int)v.size()) - startIndex;
    return (length <= 0) ? DoubleArray() : DoubleArray(&(v[startIndex]), length);
}

//
// Convert DoubleArray to std::vector
//
std::vector<double> FinanceDemo::arrayToVector(DoubleArray a)
{
    return std::vector<double>(a.data, a.data + a.len);
}


