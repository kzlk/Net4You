#ifndef ZOOMSCROLLPDF_H
#define ZOOMSCROLLPDF_H

#include <QDialog>
#include <QScrollBar>
#include <QButtonGroup>
#include "qchartviewer.h"

class ZoomScrollPdf : public QDialog {
    Q_OBJECT
public:
    ZoomScrollPdf(QWidget *parent = 0);
    ~ZoomScrollPdf();

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
    QScrollBar *m_HScrollBar;
    QChartViewer *m_ChartViewer;

    void loadData();                                    // Load data into data arrays
    void initChartViewer(QChartViewer *viewer);         // Initialize the QChartViewer
    void drawChart(QChartViewer *viewer);               // Draw chart
    XYChart* drawXYChart(double startX, double endX);   // Draw chart in given x range
    void trackLineLabel(XYChart *c, int mouseX);       // Draw Track Cursor
    void updateControls(QChartViewer *viewer);          // Update other controls
    void createPdfReport(const char* filename);         // Generate PDF report


private slots:
    void onHScrollBarChanged(int value);
    void onMouseUsageChanged(QAbstractButton *b);
    void onViewPortChanged();
    void onMouseMovePlotArea(QMouseEvent *event);
    void onSaveChart(bool);
    void onSaveReport(bool);
};

#endif // ZOOMSCROLLPDF_H
