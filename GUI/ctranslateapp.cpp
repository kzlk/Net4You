#include "ctranslateapp.h"
#include <QSettings>
CTranslateApp::CTranslateApp(Ui::MainWindow *main) : mMain(main)
{
    // Load the last selected language from settings
    QSettings settings("MyApp", "MyApplication");

    QString lastLanguage = settings.value("language").toString();
    connect(mMain->comboBox_language, &QComboBox::currentTextChanged, this, &CTranslateApp::changeComboBoxIndex);
    qDebug() << "Last language " << lastLanguage << '\n';
    if (!lastLanguage.isEmpty())
    {
        if (lastLanguage == "Українська")
        {
            lastLanguage = UA_LANG;
        }
        int index = mMain->comboBox_language->findText(lastLanguage);
        if (index != -1)
        {
            mMain->comboBox_language->setCurrentIndex(index);
        }
    }
}

void CTranslateApp::changeLanguage(QString language)
{
    qApp->removeTranslator(translator);
    translator = new QTranslator(this);
    translator->load(language);
    qApp->installTranslator(translator);
}

void CTranslateApp::changeComboBoxIndex(const QString &index)
{
    // Save the selected language to settings
    QSettings settings("MyApp", "MyApplication");
    settings.setValue("language", index);
    if (index == UA_LANG || index == "Українська")
        changeLanguage(UA_PATH);
    else
    {
        changeLanguage("English");
    }
}
