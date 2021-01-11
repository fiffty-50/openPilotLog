#include "astyle.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include "src/testing/adebug.h"
#include "src/classes/asettings.h"

const QString AStyle::defaultStyle = QStringLiteral("Fusion");

const QString AStyle::defaultStyleSheet = QStringLiteral("");

const QStringList AStyle::styles = QStyleFactory::keys();

const QMap<AStyle::StyleSheet, QFileInfo> AStyle::defaultStyleSheets = {
    {Dark, QFileInfo(QStringLiteral("dark.qss"))},
    {Light, QFileInfo(QStringLiteral("light.qss"))},
    {Default, QFileInfo(defaultStyleSheet)},
};

QString AStyle::currentStyle = defaultStyle;
QString AStyle::currentStyleSheet = defaultStyleSheet;


static inline
QString read_stylesheet(const AStyle::StyleSheet stylesheet)
{
    QFileInfo qss_file_info = AStyle::defaultStyleSheets[stylesheet];
    DEB << "reading:" << ":" + qss_file_info.fileName();

    QFile file(":" + qss_file_info.fileName());
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    return stream.readAll();
}

/*!
 * \brief Setup style and stylesheet by reading from openPilotLog.ini
 */
void AStyle::setup()
{
    // [G]: Are there leaks when style changes?
    QVariant app_style = ASettings::read(ASettings::Main::Style);
    if(!app_style.toBool()){
        DEB << "Setting style to default:" << defaultStyle;
        app_style = defaultStyle;
        ASettings::write(ASettings::Main::Style, app_style);
    }
    DEB << "Style set to:" << app_style;
    QApplication::setStyle(QStyleFactory::create(app_style.toString()));
    currentStyle = app_style.toString();

    auto app_stylesheet = ASettings::read(ASettings::Main::StyleSheet);
    if(!app_stylesheet.toBool()){
        DEB << "Setting stylesheet to default:" << defaultStyleSheet;
        app_stylesheet = defaultStyleSheet;
        ASettings::write(ASettings::Main::StyleSheet, app_stylesheet);
    }
    DEB << "Stylesheet set to:" << app_stylesheet;
    qApp->setStyleSheet(read_stylesheet(static_cast<StyleSheet>(app_stylesheet.toUInt())));
    currentStyleSheet = app_stylesheet.toString();
}

void AStyle::setStyle(const QString style)
{
    DEB << "Setting style to:" << style;
    QApplication::setStyle(QStyleFactory::create(style));
    ASettings::write(ASettings::Main::Style, style);
    currentStyle = style;
}

void AStyle::setStyleSheet(const StyleSheet stylesheet)
{
    DEB << "Setting stylesheet to:" << defaultStyleSheets[stylesheet].baseName();
    qApp->setStyleSheet(read_stylesheet(stylesheet));
    ASettings::write(ASettings::Main::StyleSheet, stylesheet);
    currentStyleSheet = defaultStyleSheets[stylesheet].fileName();
}

const QString& AStyle::style()
{
    return currentStyle;
}

const QString& AStyle::styleSheet()
{
    return currentStyleSheet;
}
