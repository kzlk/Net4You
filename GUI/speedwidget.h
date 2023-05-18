#ifndef SPEEDWIDGET_H
#define SPEEDWIDGET_H

#include <QWidget>

namespace Ui {
class SpeedWidget;
}

class SpeedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpeedWidget(QWidget *parent = nullptr);
    ~SpeedWidget();

private:
    Ui::SpeedWidget *ui;
};

#endif // SPEEDWIDGET_H
