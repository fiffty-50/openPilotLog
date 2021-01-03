#ifndef ASTYLE_H
#define ASTYLE_H
#include <QString>
#include <QFile>

class AStyle
{
private:
    static QString currentStyle;
public:
    static const QStringList styles;
    static const QString defaultStyle;

    static void setup();
    static void setStyle(const QString& style);
    static const QString& style();
};

#endif // ASTYLE_H
