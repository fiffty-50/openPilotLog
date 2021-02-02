/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
    {QLatin1String("Breeze"),      QLatin1String(":breeze_light.qss")},
    {QLatin1String("Breeze-Dark"), QLatin1String(":breeze_dark.qss")},
    {QLatin1String("QDarkStyle"),  QLatin1String(":qdarkstyle/qdarkstyle.qss")},
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

void AStyle::resetStyle()
{
    qApp->setStyle(QStyleFactory::create(defaultStyle));
    qApp->setStyleSheet(QString());
    qApp->setPalette(qApp->style()->standardPalette());

}

void AStyle::setStyle(const QString &style_key)
{
    resetStyle();
    DEB << "Setting style: " << style_key;
    qApp->setStyleSheet(QString());
    QApplication::setStyle(QStyleFactory::create(style_key));
    currentStyle = style_key;
}

void AStyle::setStyle(const StyleSheet &style_sheet)
{
    resetStyle();
    DEB << "Setting stylesheet: " << style_sheet.styleSheetName;
    qApp->setStyleSheet(read_stylesheet(style_sheet.fileName));
    currentStyle = style_sheet.styleSheetName;
}

void AStyle::setStyle(const QPalette &palette)
{
    resetStyle();
    DEB << "Setting Palette...";
    qApp->setPalette(palette);
}

QPalette AStyle::darkPalette()
{
    auto palette = QPalette();
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    return palette;
}

const QString& AStyle::style()
{
    return currentStyle;
}
