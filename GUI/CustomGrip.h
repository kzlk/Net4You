#ifndef CUSTOMGRIP_H
#define CUSTOMGRIP_H
#include "qevent.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QFrame>
#include <QSizeGrip>
#include <QMouseEvent>
#include "Widget.h"

class CustomGrip : public QWidget
{
private:
     Widgets* wi = new Widgets();
     QPoint mousePos;
     QPoint m_dragPos;
public:
    CustomGrip(QWidget *parent, Qt::Edge position, bool disable_color = false)
        : QWidget(parent)
    {
        // SETUP UI
        setParent(parent);


        // SHOW TOP GRIP
        if (position == Qt::TopEdge)
        {
            wi->top(this);
            setGeometry(0, 0, parent->width(), 10);
            setMaximumHeight(10);

            // GRIPS
            QSizeGrip *top_left = new QSizeGrip(wi->top_left);
            QSizeGrip *top_right = new QSizeGrip(wi->top_right);

            // RESIZE TOP
            auto resize_top = [ &parent](QMouseEvent *event)
            {
                QPoint delta = event->pos();
                int height = qMax(parent->minimumHeight(), parent->height() - delta.y());
                QRect geo = parent->geometry();
                geo.setTop(geo.bottom() - height);
                parent->setGeometry(geo);
                event->accept();
            };
           // QObject::connect(wi->tops, &QWidget::mouseMoveEvent, resize_top);
           // wi->tops->mouseMoveEvent = resize_top;
            wi->tops->installEventFilter(this);

            // ENABLE COLOR
            if (disable_color)
            {
                wi->top_left->setStyleSheet("background: transparent");
                wi->top_right->setStyleSheet("background: transparent");
                wi->tops->setStyleSheet("background: transparent");
            }
        }

        // SHOW BOTTOM GRIP
        else if (position == Qt::BottomEdge)
        {
            wi->bottom(this);
            setGeometry(0, parent->height() - 10, parent->width(), 10);
            setMaximumHeight(10);

            // GRIPS
            QSizeGrip *bottom_left = new QSizeGrip(wi->bottom_left);
            QSizeGrip *bottom_right = new QSizeGrip(wi->bottom_right);

            // RESIZE BOTTOM
            auto resize_bottom = [&parent](QMouseEvent *event)
            {
                QPoint delta = event->pos();
                int height = qMax(parent->minimumHeight(), parent->height() + delta.y());
                parent->resize(parent->width(), height);
                event->accept();
            };
           // wi->bottoms->mouseMoveEvent = resize_bottom;
             wi->bottoms->installEventFilter(this);
          // QObject::connect(wi->bottoms, &QWidget::mouseMoveEvent, this, &resize_bottom);
            // ENABLE COLOR
            if (disable_color)
            {
                wi->bottom_left->setStyleSheet("background: transparent");
                wi->bottom_right->setStyleSheet("background: transparent");
                wi->bottoms->setStyleSheet("background: transparent");
            }
        }

        // SHOW LEFT GRIP
        else if (position == Qt::LeftEdge)
        {
            wi->left(this);
            setGeometry(0, 10, 10, parent->height());
            setMaximumWidth(10);

            // RESIZE LEFT
            auto resize_left = [&parent](QMouseEvent *event)
            {
                QPoint delta = event->pos();
                int width = qMax(parent->minimumWidth(), parent->width() - delta.x());
                QRect geo = parent->geometry();
                geo.setLeft(geo.right() - width);
                parent->setGeometry(geo);
                event->accept();
            };
           // wi->leftgrip->mouseMoveEvent = resize_left;
            wi->leftgrip->installEventFilter(this);


            // ENABLE COLOR
            if (disable_color)
            {
                wi->leftgrip->setStyleSheet("background: transparent");
            }
        }

        // RESIZE RIGHT
        else if (position == Qt::RightEdge) {
        this->wi->right(this);
        this->setGeometry(this->parentWidget()->width() - 10, 10, 10, this->parentWidget()->height());
        this->setMaximumWidth(10);
            auto resize_right = [=](QMouseEvent *event) {
                QPoint delta = event->pos();
                int width = qMax(this->parentWidget()->minimumWidth(), this->parentWidget()->width() + delta.x());
                this->parentWidget()->resize(width, this->parentWidget()->height());
                event->accept();
            };
            ///this->wi->rightgrip->mouseMoveEvent = resize_right;
              wi->rightgrip->installEventFilter(this);

            // ENABLE COLOR
            if (disable_color) {
                this->wi->rightgrip->setStyleSheet("background: transparent;");
            }
        }


        }



    void resizeLeft() {
        QPoint delta = QCursor::pos() - this->mousePos;
        int width = qMax(this->parentWidget()->minimumWidth(), this->parentWidget()->width() - delta.x());
        QRect geo = this->parentWidget()->geometry();
        geo.setLeft(geo.right() - width);
        this->parentWidget()->setGeometry(geo);
    }

    void resizeRight() {
        QPoint delta = QCursor::pos() - this->mousePos;
        int width = qMax(this->parentWidget()->minimumWidth(), this->parentWidget()->width() + delta.x());
        QRect geo = this->parentWidget()->geometry();
        geo.setWidth(width);
        this->parentWidget()->setGeometry(geo);
    }

    void resizeTop() {
        QPoint delta = QCursor::pos() - this->mousePos;
        int height = qMax(this->parentWidget()->minimumHeight(), this->parentWidget()->height() - delta.y());
        QRect geo = this->parentWidget()->geometry();
        geo.setTop(geo.bottom() - height);
        this->parentWidget()->setGeometry(geo);
    }

    void resizeBottom() {
        QPoint delta = QCursor::pos() - this->mousePos;
        int height = qMax(this->parentWidget()->minimumHeight(), this->parentWidget()->height() + delta.y());
        QRect geo = this->parentWidget()->geometry();
        geo.setHeight(height);
        this->parentWidget()->setGeometry(geo);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
     this->mousePos = QPoint();
    }
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::MouseMove) {
               if (obj == wi->leftgrip) {
                   resizeLeft();
               } else if (obj == wi->rightgrip) {
                   resizeRight();
               } else if (obj == wi->tops) {
                   resizeTop();
               } else if (obj == wi->bottoms) {
                   resizeBottom();
               }
               this->mousePos = QCursor::pos();
           }
           return false;
        }
    void resizeEvent(QResizeEvent *event) override {
    if (this->wi->container_top) {
    this->wi->container_top->setGeometry(0, 0, this->width(), 10);
    }
    else if (this->wi->container_bottom){
    this->wi->container_bottom->setGeometry(0, 0, this->width(), 10);
    }
    else if (this->wi->leftgrip) {
    this->wi->leftgrip->setGeometry(0, 0, 10, this->height() - 20);
    }
    else if (this->wi->rightgrip){
    this->wi->rightgrip->setGeometry(0, 0, 10, this->height() - 20);
    }

    }
};







#endif // CUSTOMGRIP_H
