#ifndef MEGAZOOMSCROLL_H
#define MEGAZOOMSCROLL_H

#include <QDialog>
#include <QScrollBar>
#include <QButtonGroup>
#include "qchartviewer.h"


class MegaZoomScroll : public QDialog {
    Q_OBJECT
public:
    MegaZoomScroll(QWidget *parent = 0);
    ~MegaZoomScroll();

private:

    QButtonGroup *mouseUsage;
    QChartViewer *m_ChartViewer;
    QScrollBar *m_HScrollBar;

    //
    // Data arrays
    //
    std::vector<double> m_dataSeriesA;
    std::vector<double> m_dataSeriesB;
    std::vector<double> m_dataSeriesC;
    std::vector<double> m_timeStamps;

    // Flag to indicate the chart has been drawn so the zoom/scroll and track cursor
    // event handlers can run.
    bool m_hasFinishedInitialization;

    // Load data into data arrays
    void loadData();

    // Data Accelerator for handle large data sets
    DataAccelerator* m_fastData;

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
};

#endif // MEGAZOOMSCROLL_H
