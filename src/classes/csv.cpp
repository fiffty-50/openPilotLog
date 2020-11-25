#include "csv.h"

Csv::Csv()
{

}
/*!
 * \brief Csv::read reads from a CSV file
 * \param filename - QString to csv file.
 * \return QVector<QStringList> of the CSV data, where each QStringList is one column of the input file
 */
QVector<QStringList> Csv::read(QString filename)
{
    QFile csvfile(filename);
    csvfile.open(QIODevice::ReadOnly);
    QTextStream stream(&csvfile);

    QVector<QStringList> values;

    //Read CSV headers and create QStringLists accordingly
    QString line = stream.readLine();
    auto items = line.split(",");
    for (int i = 0; i < items.length(); i++) {
        QStringList list;
        list.append(items[i]);
        values.append(list);
    }
    //Fill QStringLists with data
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        auto items = line.split(",");
        for (int i = 0; i < values.length(); i++) {
            values[i].append(items[i]);
        }
    }
    return values;
}
