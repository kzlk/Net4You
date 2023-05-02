#ifndef REALTIMEMULTICHART_H
#define REALTIMEMULTICHART_H


#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QScrollBar>
#include <QButtonGroup>
#include <QComboBox>
#include "qchartviewer.h"


class RealTimeMultiChart: public QDialog {
    Q_OBJECT
public:
    RealTimeMultiChart(QWidget *parent = 0);
    ~RealTimeMultiChart();

private:
    // The number of samples per data series used in this demo
    static const int sampleSize = 10000;

    // The initial full range is set to 60 seconds of data.
    static const int initialFullRange = 60;

    // The maximum zoom in is 10 seconds.
    static const int zoomInLimit = 10;

    double m_timeStamps[sampleSize];	// The timestamps for the data series
    double m_dataSeriesA[sampleSize];	// The values for the data series A
    double m_dataSeriesB[sampleSize];	// The values for the data series B
    double m_dataSeriesC[sampleSize];	// The values for the data series C

    int m_currentIndex;                 // Index of the array position to which new values are added.

    QDateTime m_nextDataTime;           // Used by the random number generator to generate realtime data.

    QLabel *m_ValueA;                   // Label to display the realtime value A
    QLabel *m_ValueB;                   // Label to display the realtime value B
    QLabel *m_ValueC;                   // Label to display the realtime value C

    QButtonGroup *mouseUsage;           // Mouse usage buttons
    QChartViewer *m_ChartViewer;        // QChartViewer control
    QComboBox *updatePeriod;            // The chart update rate
    QTimer *m_ChartUpdateTimer;         // The chart update timer
    QScrollBar *m_HScrollBar;           // The scroll bar

    // Draw chart
    XYChart* drawXYChart(QChartViewer* viewer, const double* dataSeries, const char* name,
        int color, Axis* xAxisScale, bool hasXAxis);
    void drawMultiChart(QChartViewer *viewer);
    void deleteMultiChart(MultiChart *m);

    // Draw track line
    void drawMultiTrackLine(MultiChart* m, int mouseX);
    void drawXYTrackLine(DrawArea* d, XYChart* c, int mouseX, bool hasXAxis);

    void updateControls(QChartViewer *viewer);      // Update other controls as viewport changes

private slots:
    void onMouseUsageChanged(QAbstractButton *b);   // Pointer/zoom in/zoom out button clicked
    void onSave(bool);                              // Save button clicked
    void onUpdatePeriodChanged(int);                // The chart update timer interval has changed.
    void onMouseMovePlotArea(QMouseEvent *event);   // Mouse move on plot area
    void onDataTimer();                             // Get new data values
    void onChartUpdateTimer();                      // Update the chart.
    void onViewPortChanged();                       // Viewport has changed
    void onHScrollBarChanged(int value);            // Scrollbar changed
};


#endif // REALTIMEMULTICHART_H
