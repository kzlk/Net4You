#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgraphicseffect.h"
#include <QMainWindow>
#include <Settings.h>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QFile>
#include <QSizeGrip>
#include "CustomGrip.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void buttonClick();
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     void maximize_restore();
 bool returnStatus();
  void setStatus(bool status);
 void toggleMenu(bool enable);
 void toggleLeftBox(bool enable);
  void toggleRightBox(bool enable);
 void start_box_animation(int left_box_width, int right_box_width, QString direction);
 void uiDefinitions();
 QString selectMenu(QString getStyle);
 QString deselectMenu(QString getStyle);
 void selectStandardMenu(QWidget* widget);
 void resetStyle(QWidget* widget);
  void theme(QString file, bool useCustomTheme);
void resizeGrips();
private:
     Settings settings;
    Ui::MainWindow *ui;
    QGraphicsDropShadowEffect * shadow;
    CustomGrip*  left_grip;
    CustomGrip*  right_grip;
    CustomGrip*    top_grip;
    CustomGrip*   bottom_grip;
    QSizeGrip*  sizegrip;
    QPointF dragPos;
};
#endif // MAINWINDOW_H
