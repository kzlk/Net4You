#include "ctranslateapp.h"
#include <QSettings>
CTranslateApp::CTranslateApp(Ui::MainWindow *main) : mMain(main)
{
    // Load the last selected language from settings
    QSettings settings("MyApp", "MyApplication");

    QString lastLanguage = settings.value("language").toString();
    if (!lastLanguage.isEmpty())
    {
        int index = mMain->comboBox_language->findText(lastLanguage);
        if (index != -1)
        {
            mMain->comboBox_language->setCurrentIndex(index);
        }
    }

    connect(mMain->comboBox_language, &QComboBox::currentTextChanged, this, &CTranslateApp::changeComboBoxIndex);
}

void CTranslateApp::changeLanguage(QString language)
{
    QApplication::removeTranslator(translator);
    translator = new QTranslator(this);
    translator->load(language);
    QApplication::installTranslator(translator);
}

void CTranslateApp::changeEvent(QEvent *event)
{
}

void CTranslateApp::changeComboBoxIndex(const QString &index)
{
    // Save the selected language to settings
    QSettings settings("MyApp", "MyApplication");
    settings.setValue("language", index);

    if (index == UA_LANG)
    {
        changeLanguage(UA_PATH);
    }
    else
    {
        changeLanguage("Default");
        // QApplication::removeTranslator(translator);
    }
}
