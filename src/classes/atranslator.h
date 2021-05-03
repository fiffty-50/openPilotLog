#ifndef ATRANSLATOR_H
#define ATRANSLATOR_H
#include "src/opl.h"

/*!
 * \brief The ATranslator class is responsible for managing the QTranslator used for providing localisations. Translations
 * are planned, but are not currently being worked on. The way QT language tools are working with CMAKE, there are some
 * considerations for when works on translation actually start. See [here](https://bugreports.qt.io/browse/QTBUG-41736)
 * and [here](https://bugreports.qt.io/browse/QTBUG-76410).
 * Long story short, make sure to use a version of CMake that is 3.16 or earlier, or 3.19.2 or later.
 */
class ATranslator  : public QObject {
    Q_OBJECT
public:
    /*!
     * \brief Installs a QTranslator with the selected language. Defaults to English. Call this function before constructing the Main Window.
     */
    static void installTranslator(Opl::Translations language = Opl::Translations::English);

private:
    static QTranslator *translator;

signals:
    /*!
     * \brief languageChanged() is emitted whenever the active translator is reset. This signal can be
     * connected to a slot triggering ui->retranslateUi(this) for the widgets. (To Do)
     */
    void languageChanged();
};

#endif // ATRANSLATOR_H
