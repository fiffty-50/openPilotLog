#ifndef PROCESSPILOTS_H
#define PROCESSPILOTS_H
#include <QtCore>
#include <src/opl.h>
#include <QMap>
#include <QVector>

class ProcessPilots
{
public:
    ProcessPilots(const QVector<QStringList> &raw_csv_data)
        : rawData(raw_csv_data){};

    void init(){
        parseRawData();
        processParsedData();
    };

    QMap<QString, QMap<QString, QVariant> > getProcessedPilotMaps() const;
    QMap<QString, int> getProcessedPilotsIds() const;

private:

    void parseRawData();
    void processParsedData();

    QVector<QPair<QStringList, int>> rawPilotsAndIds;
    QVector<QStringList> rawData;

    /*!
     * \brief processedPilotMaps Holds a map of the processed pilots
     *
     * \details Here, the imported pilots are stored after having been read from the CSV file.
     * The key is the original string as it was originally in the PILOT_NAME field and allows
     * mapping the data to ID's later on. The value is a QMap<QString, QVariant> and contains
     * the data processed as it will be represented in the OPL database later on.
     */
    QMap<QString, QMap<QString, QVariant>> processedPilotMaps;

    /*!
     * \brief processedPilotsIds Holds a map of the ids that have been given to the processed pilots
     *
     * \details The pilot data, once processed is held in processedPilotMaps. With processePilotsIds it
     * is possible to map the original String to the PilotID that has been assigned during the parsing process.
     */
    QMap<QString, int> processedPilotsIds;
};

#endif // PROCESSPILOTS_H
