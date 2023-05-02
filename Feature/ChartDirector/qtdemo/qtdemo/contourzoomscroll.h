#ifndef CONTOURZOOMSCROLL_H
#define CONTOURZOOMSCROLL_H

#include <QDialog>
#include <QSlider>
#include <QButtonGroup>
#include <vector>
#include "qchartviewer.h"
#include "chartdir.h"


class ContourZoomScroll: public QDialog {
    Q_OBJECT
public:
    ContourZoomScroll(QWidget *parent = 0);
    ~ContourZoomScroll();

private:

    // Data arrays for the scrollable / zoomable chart.
    std::vector<double> dataX;
    std::vector<double> dataY;
    std::vector<double> dataZ;

    // To ensure color consistency, we need to save the first color axis and
    // synchronize the color axis of the zoom-in charts with the this axis.
    XYChart* firstChart;
    ColorAxis* firstColorAxis;

    // The widgets in this demo
    QButtonGroup *mouseUsage;
    QSlider *m_ZoomBar;
    QChartViewer *m_ChartViewer;
    QViewPortControl *m_ViewPortControl;

    // Load data into data arrays
    void loadData();

    // Draw chart
    void drawChart(QChartViewer *viewer);
    void drawFullChart(QViewPortControl *vpc, QChartViewer *viewer);

    // Update other controls when viewport changed
    void updateControls(QChartViewer *viewer);

    // utility to convert std::vector to a DoubleArray
    DoubleArray vectorToArray(std::vector<double> &v);

private slots:
    void onMouseUsageChanged(QAbstractButton *b);
    void onSave(bool);
    void onZoomBarChanged(int value);
    void onViewPortChanged();
};

#endif // CONTOURZOOMSCROLL_H
