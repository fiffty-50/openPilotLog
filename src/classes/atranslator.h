#ifndef ATRANSLATOR_H
#define ATRANSLATOR_H
#include "src/opl.h"

/*!
 * \brief The ATranslator class is responsible for managing the QTranslator used for providing localisations. Translations
 * are planned, but are not currently being worked on. The way QT language tools are working with CMAKE, there are some
 * considerations for when works on translation actually start. See [here](https://bugreports.qt.io/browse/QTBUG-41736)
 * and [here](https://bugreports.qt.io/browse/QTBUG-76410).
 * Long story short, make sure to use a version of CMake that is 3.16 or earlier, or 3.19.2 or later.
 *
 * The translation source files (.ts) are located at /l10n and once compiled, the translation binaries
 * are at `$BINDIR/l10n/` and can be accessed via `OPL::L10N_FILES`.
 *
 * When a QTranslator is installed, the UI is updated through [QWidget::changeEvent()](https://doc.qt.io/qt-5/qwidget.html#changeEvent).
 *
 */
class ATranslator  : public QObject {
    Q_OBJECT
public:
    /*!
     * \brief Installs a QTranslator with the selected language. Defaults to English. Call this function before constructing the Main Window.
     */
    static void installTranslator(OPL::Translation language = OPL::Translation::English);

private:
    static QTranslator *translator;
};

#endif // ATRANSLATOR_H
