#include "astyle.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include "src/testing/adebug.h"
#include "src/classes/asettings.h"

#ifdef __linux__
const QString AStyle::defaultStyle = QStringLiteral("fusion");
#elif defined(_WIN32) || defined(_WIN64)
const QString AStyle::defaultStyle = QStringLiteral("Windows");
#endif

const QString AStyle::defaultQStyleSheet = QStringLiteral("dark");
const QStringList AStyle::styles = QStyleFactory::keys();
QString AStyle::currentStyle;
QString AStyle::currentStyleSheet;

static inline
QString read_stylesheet(const QString stylesheet)
{
    DEB << "reading:" << ":" + stylesheet + ".qss";
    QFile file(":" + stylesheet + ".qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    return stream.readAll();
}

// [G]: Are there leaks when style changes?
void AStyle::setup()
{
    QVariant app_style = ASettings::read(ASettings::Main::Style);
    QVariant app_stylesheet = ASettings::read(ASettings::Main::StyleSheet);

    if(!app_style.toBool()){
        DEB << "Setting style to default:" << defaultStyle;
        app_style = defaultStyle;
        ASettings::write(ASettings::Main::Style, app_style);
    }
    DEB << "Style set to:" << app_style;
    QApplication::setStyle(QStyleFactory::create(app_style.toString()));
    currentStyle = app_style.toString();

    if(!app_stylesheet.toBool()){
        DEB << "Setting stylesheet to default:" << defaultQStyleSheet;
        app_stylesheet = defaultQStyleSheet;
        ASettings::write(ASettings::Main::StyleSheet, app_stylesheet);
    }
    DEB << "Stylesheet set to:" << app_stylesheet;
    qApp->setStyleSheet(read_stylesheet(app_stylesheet.toString()));
    currentStyleSheet = app_stylesheet.toString();
}

void AStyle::setStyle(const QString style)
{
    DEB << "Setting style to:" << style;
    QApplication::setStyle(QStyleFactory::create(style));
    ASettings::write(ASettings::Main::Style, style);
}

void AStyle::setStyleSheet(const QString stylesheet)
{
    qApp->setStyleSheet(read_stylesheet(stylesheet));
}

const QString& AStyle::style()
{
    return currentStyle;
}

const QString& AStyle::styleSheet()
{
    return currentStyleSheet;
}
