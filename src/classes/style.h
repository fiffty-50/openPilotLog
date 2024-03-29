/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#ifndef STYLE_H
#define STYLE_H
#include <QString>
#include <QFileInfo>
#include <QHash>
#include <QTextStream>
#include <QComboBox>

namespace OPL {

/*!
 * \brief The StyleSheet struct holds the Display Name and File Name (in the
 * resource system) for the available stylesheets.
 */
struct StyleSheet
{
    StyleSheet(QLatin1String style_sheet_name, QLatin1String file_name)
        : styleSheetName(style_sheet_name), fileName(file_name)
    {}
    QLatin1String styleSheetName;
    QLatin1String fileName;
};

static inline QString read_stylesheet(const QString &stylesheet)
{
    QFile file(stylesheet);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    return stream.readAll();
}

/*!
 * \brief The AStyle class encapsulates style and stylesheet logic.
 */
class Style
{
private:
    static QString currentStyle;
    static QLatin1String DARK_PALETTE;
    static void resetStyle();
public:

    enum class StyleType {Light, Dark};

    static const QStringList styles;
    static const inline QString defaultStyle = QStringLiteral("Fusion");
    static const QList<StyleSheet> styleSheets;

    static void setup();
    static void setStyle(const QString &style_key);
    static void setStyle(const StyleSheet &style_sheet);
    static void setStyle(const QPalette &palette);
    static QString getCurrentStyle() {return currentStyle;}
    static StyleType getStyleType();
    static QPalette darkPalette();
    static const QString& style();

    static inline void loadStylesComboBox(QComboBox *combo_box){
        const QSignalBlocker blocker(combo_box);
        combo_box->addItems(Style::styles);
        // TODO enable style sheets.. disabled for now because they are yanky, inconsistant and randomly break the layout
        // for (const auto &style_sheet : Style::styleSheets) {
        //     combo_box->addItem(style_sheet.styleSheetName);
        // }
        combo_box->addItem(QStringLiteral("Dark-Palette"));
        combo_box->model()->sort(0);
    }
};

} // namespace OPL

#endif // STYLE_H
