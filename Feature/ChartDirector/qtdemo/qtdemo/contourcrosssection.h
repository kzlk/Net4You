#ifndef ContourCrossSection_H
#define ContourCrossSection_H

#include <QDialog>
#include <QCheckBox>
#include "qchartviewer.h"


class ContourCrossSection : public QDialog {
    Q_OBJECT
public:
    ContourCrossSection(QWidget *parent = 0);
    ~ContourCrossSection();

private:
    // Controls
    QChartViewer* m_ChartViewer;
    QChartViewer* m_CrossSectionViewerX;
    QChartViewer* m_CrossSectionViewerY;
    ContourLayer* m_ContourLayer;

    // Keep track of mouse dragging state
    enum DragState { Drag_None, Drag_Vertical, Drag_Horizontal };
    DragState m_isDragging;
    int m_dragOffset;
    int m_xTrackPosition;
    int m_yTrackPosition;

    // Draw chart, cross section  and crosshair
    void drawChart(QChartViewer* viewer);
    void drawCrossSectionX(int x);
    void drawCrossSectionX(QChartViewer* viewer, XYChart* mainChart, ContourLayer* layer, int x);
    void drawCrossSectionY(int y);
    void drawCrossSectionY(QChartViewer* viewer, XYChart* mainChart, ContourLayer* layer, int y);
    void drawCrossHair(QChartViewer* viewer);

private slots:
    void onMouseMovePlotArea(QMouseEvent *event);
};

#endif // ContourCrossSection_H
