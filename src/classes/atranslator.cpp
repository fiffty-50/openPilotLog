#include "atranslator.h"

const static QMap<Opl::Translations, QString> AVAILABLE_L10N {
    {Opl::Translations::English, QStringLiteral("l10n/openpilotlog_en")},
    {Opl::Translations::German,  QStringLiteral("l10n/openpilotlog_de")},
    {Opl::Translations::Spanish, QStringLiteral("l10n/openpilotlog_es")},
};

QTranslator* ATranslator::translator;

void ATranslator::installTranslator(Opl::Translations language)
{
    translator = new QTranslator();
    if (translator->load(AVAILABLE_L10N[language]))
        LOG << "Translations loaded. Selected language: " << AVAILABLE_L10N[language].right(2);
    else
        LOG << "Loading translations has failed. Selected language: " << AVAILABLE_L10N[language].right(2);


    if (qApp->installTranslator(translator))
        LOG << "Translator installed.";
    else
        LOG << "Unable to install Translator.";
}
