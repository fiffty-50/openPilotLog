#ifndef CSV_H
#define CSV_H
#include <QtCore>

class Csv
{
public:
    Csv();

    static QVector<QStringList> read(QString filename);

};

#endif // CSV_H
