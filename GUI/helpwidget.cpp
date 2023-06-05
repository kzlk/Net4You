#include "helpwidget.h"
#include "ui_helpwidget.h"

HelpWidget::HelpWidget(QWidget *parent) : QMainWindow(parent), ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
            SLOT(handleItemClicked(QTreeWidgetItem *, int)));
    ui->stackedWidget->setCurrentWidget(ui->page_inProgress);
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::handleItemClicked(QTreeWidgetItem *item, int column)
{

    if (item->text(column) == tr("Graph"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_visualMonitoring);
    }
    else if (item->text(column) == tr("Network Adapter"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_networkAdapter);
    }
    else if (item->text(column) == tr("Routes"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_Routes);
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->page_inProgress);
    }
}
