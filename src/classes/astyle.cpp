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
 *
 * Breeze Stylesheets (c) Alexander Huszagh
 * https://github.com/Alexhuszagh/BreezeStyleSheets
 *
 * QDarksStyle Stylesheet (c) Colin Duquesnoy
 * https://github.com/ColinDuquesnoy/QDarkStyleSheet
 *
 * Dark Palette (c) 2017 by Juergen Skrotzky
 * https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle
 *
 */
#include "astyle.h"
#include "src/opl.h"
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QFont>
#include "src/classes/asettings.h"
#include "src/functions/alog.h"

const QString AStyle::defaultStyle = QLatin1String("Fusion");

const QStringList AStyle::styles = QStyleFactory::keys();

const QList<StyleSheet> AStyle::styleSheets = {
    {QLatin1String("Breeze"),      QLatin1String(":breeze_light.qss")},
    {QLatin1String("Breeze-Dark"), QLatin1String(":breeze_dark.qss")},
    {QLatin1String("QDarkStyle"),  QLatin1String(":qdarkstyle/qdarkstyle.qss")},
};

QString AStyle::currentStyle = defaultStyle;
QLatin1String AStyle::DARK_PALETTE = QLatin1String("Dark-Palette");
/*!
 * \brief Setup Application style by reading from openPilotLog.ini
 */
void AStyle::setup()
{
    if (!ASettings::read(ASettings::Main::SetupComplete).toBool()) // Use system default for first run
        return;
    // Set Font
    if (!ASettings::read(ASettings::Main::UseSystemFont).toBool()) {
        QFont font(ASettings::read(ASettings::Main::Font).toString());
        font.setPointSize(ASettings::read(ASettings::Main::FontSize).toUInt());
        qApp->setFont(font);
        LOG << "Application Font set: " << font.toString().split(',').first();
    }
    // Set style, stylesheet or palette
    QString style_setting = ASettings::read(ASettings::Main::Style).toString();

    if (style_setting == DARK_PALETTE) {
        AStyle::setStyle(AStyle::darkPalette());
        ASettings::write(ASettings::Main::Style, style_setting);
        return;
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
            setStyle(style_sheet);
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
    LOG << "Setting style: " << style_key;
    QApplication::setStyle(QStyleFactory::create(style_key));
    currentStyle = style_key;
}

void AStyle::setStyle(const StyleSheet &style_sheet)
{
    resetStyle();
    LOG << "Setting stylesheet: " << style_sheet.styleSheetName;
    qApp->setStyleSheet(read_stylesheet(style_sheet.fileName));
    currentStyle = style_sheet.styleSheetName;
}

void AStyle::setStyle(const QPalette &palette)
{
    resetStyle();
    LOG << "Setting Colour Palette...";
    currentStyle = DARK_PALETTE;
    qApp->setPalette(palette);
}

StyleType AStyle::getStyleType()
{
    const QStringList darkStyles = {
        QStringLiteral("Breeze-Dark"),
        QStringLiteral("QDarkStyle"),
        DARK_PALETTE,
    };

    if (darkStyles.contains(currentStyle))
        return StyleType::Dark;
    else
        return StyleType::Light;
}

QPalette AStyle::darkPalette()
{
    auto palette = QPalette();
    //palette.setColor(QPalette::Window, QColor(53, 53, 53));
    //palette.setColor(QPalette::WindowText, Qt::white);
    //palette.setColor(QPalette::Base, QColor(25, 25, 25));
    //palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    //palette.setColor(QPalette::ToolTipBase, Qt::black);
    //palette.setColor(QPalette::ToolTipText, Qt::white);
    //palette.setColor(QPalette::Text, Qt::white);
    //palette.setColor(QPalette::Button, QColor(53, 53, 53));
    //palette.setColor(QPalette::ButtonText, Qt::white);
    //palette.setColor(QPalette::BrightText, Qt::red);
    //palette.setColor(QPalette::Link, QColor(42, 130, 218));
    //palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    //palette.setColor(QPalette::HighlightedText, Qt::black);

    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::WindowText,
                     QColor(127, 127, 127));
    palette.setColor(QPalette::Base, QColor(42, 42, 42));
    palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Dark, QColor(35, 35, 35));
    palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                     QColor(127, 127, 127));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     QColor(127, 127, 127));

    return palette;

}

const QString& AStyle::style()
{
    return currentStyle;
}
