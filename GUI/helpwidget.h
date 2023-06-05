#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include "qtreewidget.h"
#include <QMainWindow>

namespace Ui
{
class HelpWidget;
}

class HelpWidget : public QMainWindow
{
    Q_OBJECT

  public:
    explicit HelpWidget(QWidget *parent = nullptr);
    ~HelpWidget();

  private:
    Ui::HelpWidget *ui;

  private slots:
    void handleItemClicked(QTreeWidgetItem *item, int column);
};

#endif // HELPWIDGET_H
