#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qsystemtrayicon.h"
#pragma once
#include "qgraphicseffect.h"
#include <QMainWindow>
#include "Settings.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QFile>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>
#include <QSizeGrip>
#include "CustomGrip.h"
#include <QStandardItemModel>
#include "Network/cmaincontrolblock.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  protected:
    void buttonClick();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    //    void mouseMoveEvent(QMouseEvent *event) override;

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void maximize_restore();
    bool returnStatus();
    void dobleClickMaximizeRestore(QMouseEvent *event);
    void setStatus(bool status);
    void toggleMenu(bool enable);
    void toggleLeftBox(bool enable);
    void toggleRightBox(bool enable);
    void start_box_animation(int left_box_width, int right_box_width, QString direction);
    void uiDefinitions();
    QString selectMenu(QString getStyle);
    QString deselectMenu(QString getStyle);
    void selectStandardMenu(QWidget *widget);
    void resetStyle(QWidget *widget);
    void theme(QString file, bool useCustomTheme);
    void resizeGrips();
    // setup main stack widget
    void setupDeviceInfo();

  private:
    CMainControlBlock *controlBlock{};
    QRect draggableArea;
    Settings settings;
    Ui::MainWindow *ui;
    QGraphicsDropShadowEffect *shadow;
    CustomGrip *left_grip;
    CustomGrip *right_grip;
    CustomGrip *top_grip;
    CustomGrip *bottom_grip;
    QSizeGrip *sizegrip;
    QPointF dragPos;

    // QSystemTrayIcon* icon
    QSystemTrayIcon *sysTrayIcon{};

  private slots:
    void sysTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

  private:
    void closeEvent(QCloseEvent *event) override;

  signals:
    void appClose(QCloseEvent *event);
};
#endif // MAINWINDOW_H
