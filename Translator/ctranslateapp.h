#ifndef CTRANSLATEAPP_H
#define CTRANSLATEAPP_H

#include <QTranslator>
#include <QLocale>
#include <QMainWindow>
#include "ui_main.h"

#define UA_PATH ":/languages/languages/Ukrainian_ua.qm"
#define UA_LANG "Ukrainian"

class CTranslateApp : public QObject
{
    Q_OBJECT
  public:
    CTranslateApp(Ui::MainWindow *main);

  private:
    Ui::MainWindow *mMain{};
    QTranslator *translator{};
    void changeLanguage(QString language);

  private slots:
    void changeComboBoxIndex(const QString &index);
};

#endif // CTRANSLATEAPP_H
