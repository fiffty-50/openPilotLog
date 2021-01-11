#ifndef ASTYLE_H
#define ASTYLE_H
#include <QString>
#include <QFileInfo>
#include <QMap>

/*!
 * \brief The AStyle class encapsulates style and stylesheet logic.
 * \todo Agree upon the file naming of the assets that will be read.
 * for now it is assumed that dark means "dark.qss"
 */
class AStyle
{
public:
    enum StyleSheet{
        Default = 0,
        Dark,
        Light
    };
private:
    static QString currentStyle;
    static QString currentStyleSheet;
public:
    static const QStringList styles;
    static const QString defaultStyle;
    static const QString defaultStyleSheet;
    static const QMap<StyleSheet, QFileInfo> defaultStyleSheets;

    static void setup();
    static void setStyle(const QString style);
    static void setStyleSheet(const StyleSheet stylesheet);
    static const QString& style();
    static const QString& styleSheet();
};

#endif // ASTYLE_H
