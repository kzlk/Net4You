#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <Qtimer>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent* event);

private slots:
    void updateSpeed();

private:
    Ui::MainWindow *ui;
    bool                        dragable;
    QPoint                      dragPoint;
    unsigned    __int64         inBytes;
    unsigned    __int64         outBytes;
    unsigned    __int64         preInBytes;
    unsigned    __int64         preOutBytes;
    QLabel* downSpeed;
    QLabel* upSpeed;
};

#endif // MAINWINDOW_H
