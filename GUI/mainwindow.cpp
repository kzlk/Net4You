#include "mainwindow.h"
#include <QTimer>
#include <qapplication.h>

#include "ui_main.h"
bool GLOBAL_STATE = false;
bool GLOBAL_TITLE_BAR = true;
Ui::MainWindow* widgets = nullptr;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     ::widgets = this->ui;
    //QWidget* w = new QWidget();
   // w->setStyleSheet("background-color: #6272a4;");
    //ui->setSyleSheet()
    ui->pushButton->setStyleSheet("background-color: #6272a4;");
    ui->plainTextEdit->setStyleSheet("background-color: #6272a4;");
    ui->tableWidget->setStyleSheet("QScrollBar:vertical { background: #6272a4; } QScrollBar:horizontal { background: #6272a4; }");
    ui->scrollArea->setStyleSheet("QScrollBar:vertical { background: #6272a4; } QScrollBar:horizontal { background: #6272a4; }");
    ui->comboBox->setStyleSheet("background-color: #6272a4;");
    ui->horizontalScrollBar->setStyleSheet("background-color: #6272a4;");
    ui->verticalScrollBar->setStyleSheet("background-color: #6272a4;");
    ui->commandLinkButton->setStyleSheet("color: #ff79c6;");

    settings.ENABLE_CUSTOM_TITLE_BAR = true;
    // APP NAME
           QString title = "Net4You";
           QString description = "Lightweiht program to network monitoring";

           // APPLY TEXTS
                this->setWindowTitle(title);
                 widgets->titleRightInfo->setText(description);


                 // TOGGLE MENU
                 connect(widgets->toggleButton, &QPushButton::clicked, [=]() {
                     MainWindow::toggleMenu(true);
                 });

                 // SET UI DEFINITIONS
                      MainWindow::uiDefinitions();

                      // QTableWidget PARAMETERS
                      widgets->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

                      // BUTTONS CLICK

                      // LEFT MENUS
                      connect(widgets->btn_home, &QPushButton::clicked, this, &MainWindow::buttonClick);
                      connect(widgets->btn_widgets, &QPushButton::clicked, this, &MainWindow::buttonClick);
                      connect(widgets->btn_new, &QPushButton::clicked, this, &MainWindow::buttonClick);
                      connect(widgets->btn_save, &QPushButton::clicked, this, &MainWindow::buttonClick);

                      // EXTRA LEFT BOX
                      connect(widgets->toggleLeftBox, &QPushButton::clicked, [=]() {
                          MainWindow::toggleLeftBox( true);
                      });
                      connect(widgets->extraCloseColumnBtn, &QPushButton::clicked, [=]() {
                           MainWindow::toggleLeftBox( true);
                      });

                      // EXTRA RIGHT BOX
                      connect(widgets->settingsTopBtn, &QPushButton::clicked, [=]() {
                           MainWindow::toggleRightBox( true);
                      });

                      // SHOW APP
                            //  this->show();

                              // SET CUSTOM THEME
                              bool useCustomTheme = false;
                              QString themeFile = "E:/Developer_Project/C++/QT/KURSOVA_SPZ/themes/py_dracula_dark.qss";

                              // SET THEME AND HACKS
                              if (useCustomTheme) {
                                  // LOAD AND APPLY STYLE
                                 MainWindow::theme(themeFile, true);

                                  // SET HACKS
                                  //AppFunctions::setThemeHack(this);
                                 settings.BTN_LEFT_BOX_COLOR = "background-color: #495474;";
                                         settings.BTN_RIGHT_BOX_COLOR = "background-color: #495474;";
                                         settings.MENU_SELECTED_STYLESHEET  = """ border-left: 22px solid qlineargradient(spread:pad, x1:0.034, y1:0, x2:0.216, y2:0, stop:0.499 rgba(255, 121, 198, 255), stop:0.5 rgba(85, 170, 255, 0));    background-color: #566388;   """;

                                         // SET MANUAL STYLES
                                        ui->lineEdit->setStyleSheet("background-color: #6272a4;");
                                        ui->pushButton->setStyleSheet("background-color: #6272a4;");
                                        ui->plainTextEdit->setStyleSheet("background-color: #6272a4;");
                                        ui->tableWidget->setStyleSheet("QScrollBar:vertical { background: #6272a4; } QScrollBar:horizontal { background: #6272a4; }");
                                        ui->scrollArea->setStyleSheet("QScrollBar:vertical { background: #6272a4; } QScrollBar:horizontal { background: #6272a4; }");
                                        ui->comboBox->setStyleSheet("background-color: #6272a4;");
                                        ui->horizontalScrollBar->setStyleSheet("background-color: #6272a4;");
                                        ui->verticalScrollBar->setStyleSheet("background-color: #6272a4;");
                                        ui->commandLinkButton->setStyleSheet("color: #ff79c6;");

                              }

                              // SET HOME PAGE AND SELECT MENU
                              widgets->stackedWidget->setCurrentWidget(widgets->home);
                              widgets->btn_home->setStyleSheet( MainWindow::selectMenu(widgets->btn_home->styleSheet()));

//                              QStandardItemModel* item = new QStandardItemModel(20,20, this);



//                              ui->tableView_interfaces->setModel(item);


//                              QModelIndex index{};
//                              for(int i = 0; i < 10; i++)
//                              {
//                                  for(int j = 0; j < 10; j++)
//                                  {
//                                      index = item->index(i, j );
//                                      item->setData(index, 2);
//                                  }
//                              }

//                              ui->tableView_interfaces->verticalHeader()->hide();

                              // Create a QStandardItemModel to hold the data
                              QStandardItemModel* model = new QStandardItemModel();

                              // Set the header data for the model
                              model->setHorizontalHeaderLabels({"Field", "Value"});

                              // Create the top-level item for "Network Adapter Properties"
                              QList<QStandardItem*> topLevelItems;
                              QStandardItem* networkAdapterPropertiesItem = new QStandardItem("Network Adapter Properties");
                              topLevelItems.append(networkAdapterPropertiesItem);

                              // Create child items for "Network Adapter Properties"
                              QList<QStandardItem*> networkAdapterItems;
                              networkAdapterItems.append(new QStandardItem("Network Adapter"));
                              networkAdapterItems.append(new QStandardItem("Realtek RTL8822CE 802.1 lac PCIe Adapter"));
                              networkAdapterPropertiesItem->appendRow(networkAdapterItems);

                              QList<QStandardItem*> interfaceTypeItems;
                              interfaceTypeItems.append(new QStandardItem(QIcon(":/icons/images/icons/cil-save.png"), "Interface Type"));
                              interfaceTypeItems.append(new QStandardItem("802.11 Wireless Ethernet"));
                              networkAdapterPropertiesItem->appendRow(interfaceTypeItems);

                              QList<QStandardItem*> hardwareAddressItems;
                              hardwareAddressItems.append(new QStandardItem("Hardware Address"));
                              hardwareAddressItems.append(new QStandardItem("5C-3A-45-92-F6-2B"));
                              networkAdapterPropertiesItem->appendRow(hardwareAddressItems);

                              QList<QStandardItem*> connectionNameItems;
                              connectionNameItems.append(new QStandardItem("Connection Name"));
                              connectionNameItems.append(new QStandardItem("Wi-Fi"));
                              networkAdapterPropertiesItem->appendRow(connectionNameItems);

                              QList<QStandardItem*> connectionSpeedItems;
                              connectionSpeedItems.append(new QStandardItem("Connection Speed"));
                              connectionSpeedItems.append(new QStandardItem("300 Mbps"));
                              networkAdapterPropertiesItem->appendRow(connectionSpeedItems);

                              model->appendRow(networkAdapterPropertiesItem);

                               ui->treeView_interfaces->setModel(model);
                               ui->treeView_interfaces->expandAll();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::maximize_restore()
   {
       bool status = GLOBAL_STATE;
       if (status == false)
       {
           showMaximized();
           GLOBAL_STATE = true;
           ui->appMargins->setContentsMargins(0, 0, 0, 0);
           ui->maximizeRestoreAppBtn->setToolTip("Restore");
           ui->maximizeRestoreAppBtn->setIcon(QIcon(":/icons/images/icons/icon_restore.png"));
           ui->frame_size_grip->hide();
           left_grip->hide();
          right_grip->hide();
           top_grip->hide();
          bottom_grip->hide();
       }
       else
       {
           GLOBAL_STATE = false;
           showNormal();
           resize(width() + 1, height() + 1);
           ui->appMargins->setContentsMargins(10, 10, 10, 10);
           ui->maximizeRestoreAppBtn->setToolTip("Maximize");
           ui->maximizeRestoreAppBtn->setIcon(QIcon(":/icons/images/icons/icon_maximize.png"));
           ui->frame_size_grip->show();
           left_grip->show();
           right_grip->show();
           top_grip->show();
           bottom_grip->show();
       }
   }

// RETURN STATUS
// ///////////////////////////////////////////////////////////////
bool MainWindow::returnStatus()
{
    return GLOBAL_STATE;
}

void MainWindow::setStatus(bool status)
{
    GLOBAL_STATE = status;
}

// TOGGLE MENU
  // ///////////////////////////////////////////////////////////////
  void MainWindow::toggleMenu(bool enable)
  {
      if (enable)
      {
          // GET WIDTH
          int width = ui->leftMenuBg->width();
          int maxExtend = settings.MENU_WIDTH;
          int standard = 60;

          // SET MAX WIDTH
          int widthExtended = (width == 60) ? maxExtend : standard;

          // ANIMATION
          QPropertyAnimation *animation = new QPropertyAnimation(ui->leftMenuBg, "minimumWidth");
          animation->setDuration(settings.TIME_ANIMATION);
          animation->setStartValue(width);
          animation->setEndValue(widthExtended);
          animation->setEasingCurve(QEasingCurve::InOutQuart);
          animation->start();
      }
  }

  // TOGGLE LEFT BOX
  // ///////////////////////////////////////////////////////////////
  void MainWindow::toggleLeftBox(bool enable)
  {
  if (enable) {
  // GET WIDTH
  int width = ui->extraLeftBox->width();
  int widthRightBox = ui->extraRightBox->width();
  int maxExtend =  settings.LEFT_BOX_WIDTH;
  QString color =  settings.BTN_LEFT_BOX_COLOR;
  int standard = 0;
  // GET BTN STYLE
    QString style = ui->toggleLeftBox->styleSheet();

    // SET MAX WIDTH
    if (width == 0) {
        int widthExtended = maxExtend;
        // SELECT BTN
        ui->toggleLeftBox->setStyleSheet(style + color);
        if (widthRightBox != 0) {
            style = ui->settingsTopBtn->styleSheet();
            ui->settingsTopBtn->setStyleSheet(style.replace(settings.BTN_RIGHT_BOX_COLOR, ""));
        }
    } else {
        int widthExtended = 0;
        // RESET BTN
        ui->toggleLeftBox->setStyleSheet(style.replace(color, ""));
    }

   //this->startBoxAnimation(this, width, widthRightBox, "left");
    this->start_box_animation(  width,widthRightBox, "left");
}
  }


  // TOGGLE RIGHT BOX
  // ///////////////////////////////////////////////////////////////
  void MainWindow::toggleRightBox(bool enable) {
  if (enable) {
  // GET WIDTH
  int width = ui->extraRightBox->width();
  int widthLeftBox = ui->extraLeftBox->width();
  int maxExtend = settings.RIGHT_BOX_WIDTH;
  QString color = settings.BTN_RIGHT_BOX_COLOR;
  int standard = 0;
  // GET BTN STYLE
     QString style = ui->settingsTopBtn->styleSheet();

     // SET MAX WIDTH
     if (width == 0) {
         int widthExtended = maxExtend;
         // SELECT BTN
         ui->settingsTopBtn->setStyleSheet(style + color);
         if (widthLeftBox != 0) {
             style = ui->toggleLeftBox->styleSheet();
             ui->toggleLeftBox->setStyleSheet(style.replace(settings.BTN_LEFT_BOX_COLOR, ""));
         }
     } else {
         int widthExtended = standard;
         // RESET BTN
         ui->settingsTopBtn->setStyleSheet(style.replace(color, ""));
     }

     this->start_box_animation( widthLeftBox, width, "right");
 }
  }


  void MainWindow::start_box_animation(int left_box_width, int right_box_width, QString direction) {
  int right_width = 0;
  int left_width = 0;
  // Check values
  if (left_box_width == 0 && direction == "left") {
      left_width = 240;
  } else {
      left_width = 0;
  }
  // Check values
  if (right_box_width == 0 && direction == "right") {
      right_width = 240;
  } else {
      right_width = 0;
  }

  // ANIMATION LEFT BOX
  QPropertyAnimation* left_box = new QPropertyAnimation(ui->extraLeftBox, "minimumWidth");
  left_box->setDuration(settings.TIME_ANIMATION);
  left_box->setStartValue(left_box_width);
  left_box->setEndValue(left_width);
  left_box->setEasingCurve(QEasingCurve::InOutQuart);

  // ANIMATION RIGHT BOX
  QPropertyAnimation* right_box = new QPropertyAnimation(ui->extraRightBox, "minimumWidth");
  right_box->setDuration(settings.TIME_ANIMATION);
  right_box->setStartValue(right_box_width);
  right_box->setEndValue(right_width);
  right_box->setEasingCurve(QEasingCurve::InOutQuart);

  // GROUP ANIMATION
  QParallelAnimationGroup* group = new QParallelAnimationGroup();
  group->addAnimation(left_box);
  group->addAnimation(right_box);
  group->start();
  }


  // SELECT/DESELECT MENU
  // SELECT
  QString  MainWindow::selectMenu(QString getStyle)
  {
      QString select = getStyle + settings.MENU_SELECTED_STYLESHEET;
      return select;
  }

  // DESELECT
  QString MainWindow::deselectMenu(QString getStyle)
  {
      QString deselect = getStyle.replace(settings.MENU_SELECTED_STYLESHEET, "");
      return deselect;
  }

  // START SELECTION
  void MainWindow::selectStandardMenu(QWidget* widget)
  {
      for(auto w : ui->topMenu->findChildren<QPushButton*>())
      {
          if(w->objectName() == widget->objectName())
          {
              w->setStyleSheet(selectMenu(w->styleSheet()));
              break;
          }
      }
  }

  // RESET SELECTION
  void MainWindow::resetStyle(QWidget* widget)
  {
      for(auto w : ui->topMenu->findChildren<QPushButton*>())
      {
          if(w->objectName() != widget->objectName())
          {
              w->setStyleSheet(deselectMenu(w->styleSheet()));
          }
      }
  }

  void MainWindow::theme(QString file, bool useCustomTheme) {
      if (useCustomTheme) {
          QFile styleFile(file);
          styleFile.open(QFile::ReadOnly);
          QString style = QString::fromLatin1(styleFile.readAll());
          ui->styleSheet->setStyleSheet(style);
      }
  }
  // START - GUI DEFINITIONS
  void MainWindow::uiDefinitions() {
      // Double-click on title bar to maximize/restore

        ui->titleRightInfo->installEventFilter(this);
      // Set window flags to create a custom title bar and enable moving and resizing the window
      if (settings.ENABLE_CUSTOM_TITLE_BAR) {
          setWindowFlags(Qt::FramelessWindowHint);
          setAttribute(Qt::WA_TranslucentBackground);


          left_grip = new CustomGrip(this, Qt::LeftEdge, true);
          right_grip = new CustomGrip(this, Qt::RightEdge, true);
          top_grip = new CustomGrip(this, Qt::TopEdge, true);
          bottom_grip = new CustomGrip(this, Qt::BottomEdge, true);
      } else {
          // Hide default title bar buttons and size grip
          ui->appMargins->setContentsMargins(0, 0, 0, 0);
          ui->minimizeAppBtn->hide();
          ui->maximizeRestoreAppBtn->hide();
          ui->closeAppBtn->hide();
          ui->frame_size_grip->hide();
      }

      // Add drop shadow effect to the window
      shadow = new QGraphicsDropShadowEffect(this);
      shadow->setBlurRadius(17);
      shadow->setXOffset(0);
      shadow->setYOffset(0);
      shadow->setColor(QColor(0, 0, 0, 150));
      ui->bgApp->setGraphicsEffect(shadow);

      // Create size grip for resizing the window
      sizegrip = new QSizeGrip(ui->frame_size_grip);
      sizegrip->setStyleSheet("width: 20px; height: 20px; margin 0px; padding: 0px;");

      // Connect title bar buttons to their respective slots
      connect(ui->minimizeAppBtn, &QPushButton::clicked, this, &MainWindow::showMinimized);
      connect(ui->maximizeRestoreAppBtn, &QPushButton::clicked, this, &MainWindow::maximize_restore);
      connect(ui->closeAppBtn, &QPushButton::clicked, this, &MainWindow::close);
  }
  // BUTTONS CLICK
  // Post here your functions for clicked buttons
  // ///////////////////////////////////////////////////////////////
  void MainWindow::buttonClick() {
  // GET BUTTON CLICKED
  QPushButton* btn = qobject_cast<QPushButton*>(sender());
  QString btnName = btn->objectName();
  // SHOW HOME PAGE
  if (btnName == "btn_home") {
      widgets->stackedWidget->setCurrentWidget(widgets->home);
      MainWindow::resetStyle(btn);
      btn->setStyleSheet( MainWindow::selectMenu(btn->styleSheet()));
  }

  // SHOW WIDGETS PAGE
  if (btnName == "btn_widgets") {
      widgets->stackedWidget->setCurrentWidget(widgets->widgets);
      MainWindow::resetStyle( btn);
      btn->setStyleSheet( MainWindow::selectMenu(btn->styleSheet()));
  }

  // SHOW NEW PAGE
  if (btnName == "btn_new") {
      widgets->stackedWidget->setCurrentWidget(widgets->new_page); // SET PAGE
      MainWindow::resetStyle(btn); // RESET ANOTHERS BUTTONS SELECTED
      btn->setStyleSheet( MainWindow::selectMenu(btn->styleSheet())); // SELECT MENU
  }

  if (btnName == "btn_save") {
      qDebug() << "Save BTN clicked!";
  }

  // PRINT BTN NAME
  qDebug() << "Button \"" << btnName << "\" pressed!";
  }


  // RESIZE EVENTS
  // ///////////////////////////////////////////////////////////////
  void MainWindow::resizeEvent(QResizeEvent* event)
  {
  // Update Size Grips
        MainWindow::resizeGrips();
  }

  void MainWindow::resizeGrips() {
  if (settings.ENABLE_CUSTOM_TITLE_BAR) {
   left_grip->setGeometry(0, 10, 10, this->height());
    right_grip->setGeometry(this->width() - 10, 10, 10, this->height());
    top_grip->setGeometry(0, 0, this->width(), 10);
    bottom_grip->setGeometry(0, this->height() - 10, this->width(), 10);
  }
  }
  bool MainWindow::eventFilter(QObject *obj, QEvent *event)
  {


      if (obj == ui->titleRightInfo && event->type() == QEvent::MouseButtonDblClick) {
              QTimer::singleShot(250, [this](){maximize_restore(); });
              return true;
          }


      else if (obj == ui->titleRightInfo && event->type() == QEvent::MouseButtonPress) {
          // Set drag position for move event
          QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
          if (mouseEvent->button() == Qt::LeftButton) {
              dragPos = mouseEvent->globalPos() - this->frameGeometry().topLeft();
              event->accept();
              return true;
          }
      }
      else if (obj == ui->titleRightInfo && event->type() == QEvent::MouseMove) {
          // Move window
          QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
          if (mouseEvent->buttons() & Qt::LeftButton) {
              QMainWindow::move((mouseEvent->globalPos() - dragPos).toPoint());
              event->accept();
          } else {
              // Check if cursor is within draggable area and set cursor shape accordingly
              if (draggableArea.contains(mapFromGlobal(QCursor::pos()))) {
                  setCursor(Qt::SizeAllCursor);
              } else {
                  setCursor(Qt::ArrowCursor);
              }
          }
      } else if (obj == ui->titleRightInfo && event->type() == QEvent::Leave) {
          // Reset cursor shape to default when mouse leaves draggable area
          setCursor(Qt::ArrowCursor);
      }
      return QObject::eventFilter(obj, event);


  }
  // MOUSE CLICK EVENTS
  // ///////////////////////////////////////////////////////////////
  void MainWindow::mousePressEvent(QMouseEvent* event) {
  // SET DRAG POS WINDOW
  this->dragPos = event->globalPosition();
      // PRINT MOUSE EVENTS
      if (event->button() == Qt::LeftButton) {
          qDebug() << "Mouse click: LEFT CLICK";
      }
      if (event->button() == Qt::RightButton) {
          qDebug() << "Mouse click: RIGHT CLICK";
      }
      }


  void MainWindow::dobleClickMaximizeRestore(QMouseEvent *event) {
      // IF DOUBLE CLICK CHANGE STATUS
      if (event->type() == QEvent::MouseButtonDblClick) {
          QTimer::singleShot(250, [this](){maximize_restore();});
      }
      event->accept();
  }
