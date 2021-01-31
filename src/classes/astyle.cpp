/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * Stylesheets used (c) Alexander Huszagh
 * https://github.com/Alexhuszagh/BreezeStyleSheets
 */
#include "astyle.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include "src/testing/adebug.h"
#include "src/classes/asettings.h"

const QString AStyle::defaultStyle = QLatin1String("Fusion");

const QStringList AStyle::styles = QStyleFactory::keys();

const QList<StyleSheet> AStyle::styleSheets = {
    {QLatin1String("Breeze"),      QLatin1String(":light.qss")},
    {QLatin1String("Breeze-Dark"), QLatin1String(":dark.qss")}
};

QString AStyle::currentStyle = defaultStyle;

/*!
 * \brief Setup style and stylesheet by reading from openPilotLog.ini
 */
void AStyle::setup()
{
    QVariant style_setting = ASettings::read(ASettings::Main::Style);
    if(!style_setting.toBool()){
        //DEB << "Setting style to default:" << defaultStyle;
        style_setting = defaultStyle;
        ASettings::write(ASettings::Main::Style, style_setting);
    }
    for (const auto &style_name : styles) {
        if (style_setting == style_name) {
            setStyle(style_name);
            currentStyle = style_name;
            return;
        }
    }
    for (const auto &style_sheet : styleSheets) {
        if (style_setting == style_sheet.styleSheetName) {
            setStyle(style_sheet.styleSheetName);
            currentStyle = style_sheet.styleSheetName;
            return;
        }
    }
}

void AStyle::setStyle(const QString &style)
{
    DEB << "Setting style: " << style;
    qApp->setStyleSheet(QString());
    QApplication::setStyle(QStyleFactory::create(style));
    currentStyle = style;
}

void AStyle::setStyle(const StyleSheet &style_sheet)
{
    DEB << "Setting stylesheet: " << style_sheet.styleSheetName;
    qApp->setStyleSheet(read_stylesheet(style_sheet.fileName));
    currentStyle = style_sheet.styleSheetName;
}

const QString& AStyle::style()
{
    return currentStyle;
}
