#include "atranslator.h"

QTranslator* ATranslator::translator;

void ATranslator::installTranslator(OPL::Translation language)
{
    translator = new QTranslator();
    if (translator->load(OPL::GLOBALS->getLanguageFilePath(language)))
        LOG << "Translations loaded. Selected language: " << OPL::GLOBALS->getLanguageFilePath(language);
    else
        LOG << "Loading translations has failed. Selected language: " << OPL::GLOBALS->getLanguageFilePath(language);


    if (qApp->installTranslator(translator))
        LOG << "Translator installed.";
    else
        LOG << "Unable to install Translator.";
}
