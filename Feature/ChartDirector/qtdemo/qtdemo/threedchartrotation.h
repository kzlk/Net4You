#ifndef ThreeDChartRotation_H
#define ThreeDChartRotation_H

#include <QDialog>
#include <QCheckBox>
#include "qchartviewer.h"


class ThreeDChartRotation : public QDialog {
    Q_OBJECT
public:
    ThreeDChartRotation(QWidget *parent = 0);
    ~ThreeDChartRotation();

private:
    QChartViewer *m_ChartViewer;
    QCheckBox *m_DrawFrameOnRotate;
    void drawChart(QChartViewer *viewer);

    // 3D view angles
    double m_elevationAngle;
    double m_rotationAngle;

    // Keep track of mouse drag
    int m_lastMouseX;
    int m_lastMouseY;
    bool m_isDragging;

private slots:
    void onViewPortChanged();
    void onMouseMoveChart(QMouseEvent *event);
    void onMouseUpChart(QMouseEvent *event);
};

#endif // ThreeDChartRotation_H

