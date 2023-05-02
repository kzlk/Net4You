#ifndef VIEWPORTCONTROLDEMO_H
#define VIEWPORTCONTROLDEMO_H

#include <QDialog>
#include <QScrollBar>
#include <QButtonGroup>
#include "qchartviewer.h"

class ViewPortControlDemo : public QDialog {
    Q_OBJECT
public:
    ViewPortControlDemo(QWidget *parent = 0);
    ~ViewPortControlDemo();

private:
    //
    // Data arrays for the scrollable / zoomable chart.
    // - In this demo, we just use a RanTable object to generate random data for the chart.
    //
    RanSeries *m_ranSeries;
    DoubleArray m_timeStamps;
    DoubleArray m_dataSeriesA;
    DoubleArray m_dataSeriesB;
    DoubleArray m_dataSeriesC;

    QButtonGroup *mouseUsage;
    QChartViewer *m_ChartViewer;
    QViewPortControl *m_ViewPortControl;

    void loadData();                                // Load data into data arrays
    void initChartViewer(QChartViewer *viewer);     // Initialize the QChartViewer
    void drawChart(QChartViewer *viewer);           // Draw chart
    void drawFullChart(QViewPortControl *vpc, QChartViewer *viewer);   // Draw full chart
    void trackLineLegend(XYChart *c, int mouseX);   // Draw Track Cursor

private slots:
    void onMouseUsageChanged(QAbstractButton *b);
    void onSave(bool);
    void onViewPortChanged();
    void onMouseMovePlotArea(QMouseEvent *event);
};

#endif // VIEWPORTCONTROLDEMO_H
