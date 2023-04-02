#ifndef WIDGET_H
#define WIDGET_H
#include "qboxlayout.h"
#include "qevent.h"
#include "qframe.h"
#include "qobject.h"
#include "qwidget.h"

class Widgets :  public QWidget
{
    Q_OBJECT


public:



    Widgets() = default;

    void top(QWidget* Form)
    {
        if (!Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));

        container_top = new QFrame(Form);
        container_top->setObjectName(QStringLiteral("container_top"));
        container_top->setGeometry(QRect(0, 0, 500, 10));
        container_top->setMinimumSize(QSize(0, 10));
        container_top->setMaximumSize(QSize(16777215, 10));
        container_top->setFrameShape(QFrame::NoFrame);
        container_top->setFrameShadow(QFrame::Raised);

        top_layout = new QHBoxLayout(container_top);
        top_layout->setSpacing(0);
        top_layout->setObjectName(QStringLiteral("top_layout"));
        top_layout->setContentsMargins(0, 0, 0, 0);

        top_left = new QFrame(container_top);
        top_left->setObjectName(QStringLiteral("top_left"));
        top_left->setMinimumSize(QSize(10, 10));
        top_left->setMaximumSize(QSize(10, 10));
        top_left->setCursor(Qt::SizeFDiagCursor);
        top_left->setStyleSheet(QStringLiteral("background-color: rgb(33, 37, 43);"));
        top_left->setFrameShape(QFrame::NoFrame);
        top_left->setFrameShadow(QFrame::Raised);
        top_layout->addWidget(top_left);

        tops = new QFrame(container_top);
        tops->setObjectName(QStringLiteral("top"));
        tops->setCursor(Qt::SizeVerCursor);
        tops->setStyleSheet(QStringLiteral("background-color: rgb(85, 255, 255);"));
        tops->setFrameShape(QFrame::NoFrame);
        tops->setFrameShadow(QFrame::Raised);
        top_layout->addWidget(tops);

        top_right = new QFrame(container_top);
        top_right->setObjectName(QStringLiteral("top_right"));
        top_right->setMinimumSize(QSize(10, 10));
        top_right->setMaximumSize(QSize(10, 10));
        top_right->setCursor(Qt::SizeBDiagCursor);
        top_right->setStyleSheet(QStringLiteral("background-color: rgb(33, 37, 43);"));
        top_right->setFrameShape(QFrame::NoFrame);
        top_right->setFrameShadow(QFrame::Raised);
        top_layout->addWidget(top_right);
    }

    void bottom(QWidget* Form)
    {
        if (!Form->objectName().isEmpty()) {
            Form->setObjectName(QStringLiteral("Form"));
        }
        container_bottom = new QFrame(Form);
        container_bottom->setObjectName(QStringLiteral("container_bottom"));
        container_bottom->setGeometry(QRect(0, 0, 500, 10));
        container_bottom->setMinimumSize(QSize(0, 10));
        container_bottom->setMaximumSize(QSize(16777215, 10));
        container_bottom->setFrameShape(QFrame::NoFrame);
        container_bottom->setFrameShadow(QFrame::Raised);
        bottom_layout = new QHBoxLayout(container_bottom);
        bottom_layout->setSpacing(0);
        bottom_layout->setObjectName(QStringLiteral("bottom_layout"));
        bottom_layout->setContentsMargins(0, 0, 0, 0);
        bottom_left = new QFrame(container_bottom);
        bottom_left->setObjectName(QStringLiteral("bottom_left"));
        bottom_left->setMinimumSize(QSize(10, 10));
        bottom_left->setMaximumSize(QSize(10, 10));
        bottom_left->setCursor(QCursor(Qt::SizeBDiagCursor));
        bottom_left->setStyleSheet(QStringLiteral("background-color: rgb(33, 37, 43);"));
        bottom_left->setFrameShape(QFrame::NoFrame);
        bottom_left->setFrameShadow(QFrame::Raised);
        bottom_layout->addWidget(bottom_left);
        bottoms = new QFrame(container_bottom);
        bottoms->setObjectName(QStringLiteral("bottom"));
        bottoms->setCursor(QCursor(Qt::SizeVerCursor));
        bottoms->setStyleSheet(QStringLiteral("background-color: rgb(85, 170, 0);"));
        bottoms->setFrameShape(QFrame::NoFrame);
        bottoms->setFrameShadow(QFrame::Raised);
        bottom_layout->addWidget(bottoms);
        bottom_right = new QFrame(container_bottom);
        bottom_right->setObjectName(QStringLiteral("bottom_right"));
        bottom_right->setMinimumSize(QSize(10, 10));
        bottom_right->setMaximumSize(QSize(10, 10));
        bottom_right->setCursor(QCursor(Qt::SizeFDiagCursor));
        bottom_right->setStyleSheet(QStringLiteral("background-color: rgb(33, 37, 43);"));
        bottom_right->setFrameShape(QFrame::NoFrame);
        bottom_right->setFrameShadow(QFrame::Raised);
        bottom_layout->addWidget(bottom_right);
    }

    void left(QWidget* Form)
    {
        if (!Form->objectName().isEmpty()) {
            Form->setObjectName(QStringLiteral("Form"));
        }
        leftgrip = new QFrame(Form);
        leftgrip->setObjectName(QStringLiteral("left"));
        leftgrip->setGeometry(QRect(0, 10, 10, 480));
        leftgrip->setMinimumSize(QSize(10, 0));
        leftgrip->setCursor(QCursor(Qt::SizeHorCursor));
        leftgrip->setStyleSheet(QStringLiteral("background-color: rgb(255, 121, 198);"));
        leftgrip->setFrameShape(QFrame::NoFrame);
        leftgrip->setFrameShadow(QFrame::Raised);
    }
    void right(QWidget *Form) {

       if (!Form->objectName().isEmpty()) {
            Form->setObjectName("Form");
        }
        Form->resize(500, 500);
       rightgrip = new QFrame(Form);
        rightgrip->setObjectName("right");
        rightgrip->setGeometry(QRect(0, 0, 10, 500));
        rightgrip->setMinimumSize(QSize(10, 0));
        rightgrip->setCursor(Qt::SizeHorCursor);
        rightgrip->setStyleSheet("background-color: rgb(255, 0, 127);");
        rightgrip->setFrameShape(QFrame::NoFrame);
        rightgrip->setFrameShadow(QFrame::Raised);
    }

public:
    QFrame* container_top;
    QHBoxLayout* top_layout;
    QFrame* top_left;
    QFrame* tops;
    QFrame* top_right;
    QFrame* container_bottom;
    QFrame*  bottom_right;
    QHBoxLayout* bottom_layout;
    QFrame* bottom_left;
    QFrame*   bottoms;
    QFrame* leftgrip;
     QFrame *rightgrip;

};

#endif // WIDGET_H
