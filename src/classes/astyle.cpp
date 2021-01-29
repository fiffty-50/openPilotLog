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

const QString AStyle::defaultStyle = QStringLiteral("Fusion");

const QString AStyle::defaultStyleSheet = QString();

const QStringList AStyle::styles = QStyleFactory::keys();

const QList<QPair <QString, QString>> AStyle::styleSheets = {
    {QStringLiteral("Breeze"), QStringLiteral(":light.qss")},
    {QStringLiteral("Breeze-Dark"), QStringLiteral(":dark.qss")}
};

QString AStyle::currentStyle = defaultStyle;
QString AStyle::currentStyleSheet = defaultStyleSheet;


static inline QString read_stylesheet(const QString &stylesheet)
{
    QFile file(stylesheet);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    return stream.readAll();
}

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
    for (const auto &style_sheet_name : styleSheets) {
        if (style_setting == style_sheet_name.first) {
            setStyle(style_sheet_name);
            currentStyle = style_sheet_name.first;
            return;
        }
    }
}

void AStyle::setStyle(const QString style)
{
    DEB << "Setting style: " << style;
    qApp->setStyleSheet(QString());
    QApplication::setStyle(QStyleFactory::create(style));
    currentStyle = style;
}

void AStyle::setStyle(const QPair<QString, QString> stylesheet)
{
    DEB << "Setting stylesheet: " << stylesheet.first;
    qApp->setStyleSheet(read_stylesheet(stylesheet.second));
    currentStyleSheet = stylesheet.first;
}

const QString& AStyle::style()
{
    return currentStyle;
}
