#include "speedwidget.h"
#include "ui_speedwidget.h"

SpeedWidget::SpeedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpeedWidget)
{
    ui->setupUi(this);
}

SpeedWidget::~SpeedWidget()
{
    delete ui;
}
