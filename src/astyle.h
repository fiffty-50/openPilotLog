#ifndef ASTYLE_H
#define ASTYLE_H
#include <QString>
#include <QFile>

class AStyle
{
public:
    static void setup();
    static void setStyle(const QString& style);
    static void setStyleSheet(const QString& qss_file);
};

#endif // ASTYLE_H
