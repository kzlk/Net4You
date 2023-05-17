#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QMainWindow>

namespace Ui {
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
};

#endif // HELPWIDGET_H
