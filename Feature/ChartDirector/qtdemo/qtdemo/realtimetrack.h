#ifndef REALTIMETRACK_H
#define REALTIMETRACK_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>
#include "qchartviewer.h"
#include <vector>


class RealTimeTrack : public QDialog {
    Q_OBJECT
public:
    RealTimeTrack(QWidget *parent = 0);
    ~RealTimeTrack();

private:
    int m_currentIndex;                 // Index of the array position to which new values are added.

    // The number of samples per data series used in this demo

    std::vector<double> m_timeStamps;	// The timestamps for the data series
    std::vector<double> m_dataSeriesA;	// The values for the data series A
    std::vector<double> m_dataSeriesB;	// The values for the data series B
    std::vector<double> m_dataSeriesC;	// The values for the data series C

    QDateTime m_nextDataTime;           // Used by the random number generator to generate realtime data.

    QLabel *m_ValueA;                   // Label to display the realtime value A
    QLabel *m_ValueB;                   // Label to display the realtime value B
    QLabel *m_ValueC;                   // Label to display the realtime value C

    QButtonGroup *runFreezeControl;     // Run/Freeze buttons
    QChartViewer *m_ChartViewer;        // QChartViewer control
    QComboBox *updatePeriod;            // The chart update rate
    QTimer *m_ChartUpdateTimer;         // The chart update timer

    void trackLineLegend(XYChart *c, int mouseX);   // Draw Track Cursor

private slots:
    void onRunFreezeChanged(QAbstractButton *b);    // The "Run" or "Freeze" button has been pressed
    void onUpdatePeriodChanged(int);                // The chart update timer interval has changed.
    void onMouseMovePlotArea(QMouseEvent *event);   // Mouse move on plot area
    void getData();                                 // Get new data values
    void updateChart();                             // Update the chart.
    void drawChart();                               // Draw chart

};

#endif // REALTIMETRACK_H
