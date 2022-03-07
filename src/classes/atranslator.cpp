#include "atranslator.h"

QTranslator* ATranslator::translator;

void ATranslator::installTranslator(Opl::Translation language)
{
    translator = new QTranslator();
    if (translator->load(Opl::GLOBALS->getLanguageFilePath(language)))
        LOG << "Translations loaded. Selected language: " << Opl::GLOBALS->getLanguageFilePath(language);
    else
        LOG << "Loading translations has failed. Selected language: " << Opl::GLOBALS->getLanguageFilePath(language);


    if (qApp->installTranslator(translator))
        LOG << "Translator installed.";
    else
        LOG << "Unable to install Translator.";
}
