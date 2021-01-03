#include "astyle.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include "src/testing/adebug.h"
#include "src/classes/asettings.h"

const QStringList AStyle::styles = QStyleFactory::keys();
const QString AStyle::defaultStyle = QStringLiteral("kvantum-dark");
QString AStyle::currentStyle;

// [G]: Are there leaks when style changes?
void AStyle::setup()
{
    QVariant app_style = ASettings::read(ASettings::Main::Theme);
    if(!app_style.toBool()){
        DEB << "Setting style to default:" << defaultStyle;
        app_style = defaultStyle;
        ASettings::write(ASettings::Main::Theme, app_style);
    }
    DEB << "Style set to:" << app_style;
    QApplication::setStyle(QStyleFactory::create(app_style.toString()));
    currentStyle = app_style.toString();
}

void AStyle::setStyle(const QString& style)
{
    DEB << "Setting style to:" << style;
    QApplication::setStyle(QStyleFactory::create(style));
    ASettings::write(ASettings::Main::Theme, style);
}

const QString& AStyle::style()
{
    return currentStyle;
}
