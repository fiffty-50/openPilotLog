#ifndef ASTYLE_H
#define ASTYLE_H
#include <QString>
#include <QFile>

class AStyle
{
private:
    static QString currentStyle;
    static QString currentStyleSheet;
public:
    static const QStringList styles;
    static const QString defaultStyle;
    static const QString defaultQStyleSheet;

    static void setup();
    static void setStyle(const QString style);
    static void setStyleSheet(const QString style_sheet);
    static const QString& style();
    static const QString& styleSheet();
};

#endif // ASTYLE_H
