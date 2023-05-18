#ifndef CTRANSLATEAPP_H
#define CTRANSLATEAPP_H

#include <QTranslator>
#include <QLocale>
#include <QMainWindow>
#include "ui_main.h"

#define UA_PATH "languages/Ukrainian_ua"
#define UA_LANG "Ukrainian"

class CTranslateApp : public QWidget
{
    Q_OBJECT
  public:
    CTranslateApp(Ui::MainWindow *main);

  private:
    Ui::MainWindow *mMain{};
    QTranslator *translator{};
    void changeLanguage(QString language);
    void changeEvent(QEvent *event) override;

  private slots:
    void changeComboBoxIndex(const QString &index);
};

#endif // CTRANSLATEAPP_H
