#include "helpwidget.h"
#include "ui_helpwidget.h"

HelpWidget::HelpWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
}

HelpWidget::~HelpWidget()
{
    delete ui;
}
