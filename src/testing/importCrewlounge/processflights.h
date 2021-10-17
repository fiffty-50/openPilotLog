#ifndef PROCESSFLIGHTS_H
#define PROCESSFLIGHTS_H
#include <QtCore>
#include <src/opl.h>

class ProcessFlights
{
public:
    ProcessFlights(const QVector<QStringList> &raw_csv_data,
                   const QMap<QString, int> &processed_pilots_ids,
                   const QMap<QString, int> &processed_tails_ids)
        : rawData(raw_csv_data),
          processedPilotsIds(processed_pilots_ids),
          processedTailsIds(processed_tails_ids){};

    void init(){
        parseRawData();
        processParsedData();
    };
    QVector<QMap<QString, QVariant> > getProcessedFlights() const;

private:
    void parseRawData();
    void processParsedData();
    QVector<QStringList> rawData;
    QVector<QStringList> rawFlightData;

    QVector<QMap<QString, QVariant>> processedFlights;

    QMap<QString, int> processedPilotsIds;
    QMap<QString, int> processedTailsIds;
};

#endif // PROCESSFLIGHTS_H
