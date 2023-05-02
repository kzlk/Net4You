#ifndef MEGAREALTIMEZOOMSCROLL_H
#define MEGAREALTIMEZOOMSCROLL_H


#include <QDialog>
#include <QScrollBar>
#include <QButtonGroup>
#include "qchartviewer.h"


class MegaRealTimeZoomScroll : public QDialog {
    Q_OBJECT
public:
    MegaRealTimeZoomScroll(QWidget *parent = 0);
    ~MegaRealTimeZoomScroll();

private:

    QButtonGroup *mouseUsage;           // Mouse usage buttons
    QChartViewer *m_ChartViewer;        // QChartViewer control
    QScrollBar *m_HScrollBar;           // The scroll har
    QTimer *m_ChartUpdateTimer;         // The chart update timer
    QPushButton *m_PlotChartPB;         // The "Plot Chart" push button

    //
    // Data arrays
    //
    std::vector<double> m_dataSeriesA;
    std::vector<double> m_dataSeriesB;
    std::vector<double> m_dataSeriesC;
    std::vector<double> m_timeStamps;

    // The index position that new data will be added.
    int m_currentIndex;

    // Data Accelerator for handle large data sets
    DataAccelerator* m_fastData;

    // Flag to indicate the chart has been drawn so the zoom/scroll and track cursor
    // event handlers can run.
    bool m_hasFinishedInitialization;

    // Load data into data arrays
    void loadData();

    // One real-time random number generator
    RanSeries m_realTimeData;
    QElapsedTimer m_lastUpdateTime;
    bool getRealTimeData();

    // Update the axis scale to accomodate new data
    void updateAxisScale(QChartViewer* viewer);

    // Initialize the CChartViewer
    void initChartViewer(QChartViewer* viewer);

    // Draw chart
    void drawChart(QChartViewer* viewer);
    void trackLineLabel(XYChart* c, int mouseX);

    // Update other controls when viewport changed
    void updateControls(QChartViewer* viewer);

private slots:
    void onClickPlotChart(bool);
    void onHScrollBarChanged(int value);
    void onMouseUsageChanged(QAbstractButton *b);
    void onViewPortChanged();
    void onMouseMovePlotArea(QMouseEvent *event);
    void onChartUpdateTimer();
};


#endif // MEGAREALTIMEZOOMSCROLL_H
