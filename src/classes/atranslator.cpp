#include "atranslator.h"

QTranslator* ATranslator::translator;

void ATranslator::installTranslator(Opl::Translations language)
{
    translator = new QTranslator();
    if (translator->load(Opl::L10N_FILES[language]))
        LOG << "Translations loaded. Selected language: " << Opl::L10N_FILES[language].right(2);
    else
        LOG << "Loading translations has failed. Selected language: " << Opl::L10N_FILES[language].right(2);


    if (qApp->installTranslator(translator))
        LOG << "Translator installed.";
    else
        LOG << "Unable to install Translator.";
}
