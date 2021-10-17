#ifndef PROCESSAIRCRAFT_H
#define PROCESSAIRCRAFT_H
#include <QtCore>
#include "src/opl.h"

class ProcessAircraft
{
public:
    ProcessAircraft(const QVector<QStringList> &raw_csv_data)
        : rawData(raw_csv_data){};

    void init(){
        parseRawData();
        processParsedData();
    };

    QMap<QString, QMap<QString, QVariant> > getProcessedTailMaps() const;
    QMap<QString, int> getProcessedTailIds() const;

private:
    void parseRawData();
    void processParsedData();
    QVector<QStringList> rawData;

    /*!
     * \brief unique_tails contains a list of all unique tails with relevant details
     */
    QVector<QStringList> unique_tails;

    /*!
     * \brief processedPilotMaps Holds a map of the processed pilots
     *
     * \details Here, the imported tails are stored after having been read from the CSV file.
     * The key is the original string as it was originally in the AC_REG field and allows
     * mapping the data to ID's later on. The value is a QMap<QString, QVariant> and contains
     * the data processed as it will be represented in the OPL database later on.
     */
    QMap<QString, QMap<QString, QVariant>> processedTailMaps;

    /*!
     * \brief processedTailIds Holds a map of the ids that have been given to the processed pilots
     *
     * \details The tail data, once processed is held in processedTailMaps. With processedTailIds it
     * is possible to map the original String to the tail_id that has been assigned during the parsing process.
     */
    QMap<QString, int> processedTailIds;
};

#endif // PROCESSAIRCRAFT_H
