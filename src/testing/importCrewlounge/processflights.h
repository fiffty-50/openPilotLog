#ifndef PROCESSFLIGHTS_H
#define PROCESSFLIGHTS_H
#include <QtCore>
#include <src/opl.h>

class ProcessFlights
{
public:
    ProcessFlights(const QVector<QStringList> &raw_csv_data,
                   const QHash<QString, int> &processed_pilots_ids,
                   const QHash<QString, int> &processed_tails_ids)
        : rawData(raw_csv_data),
          processedPilotsIds(processed_pilots_ids),
          processedTailsIds(processed_tails_ids){};

    void init(){
        parseRawData();
        processParsedData();
    };
    QVector<OPL::RowData_T> getProcessedFlights() const;

private:
    void parseRawData();
    void processParsedData();
    QVector<QStringList> rawData;
    QVector<QStringList> rawFlightData;

    QVector<OPL::RowData_T> processedFlights;

    QHash<QString, int> processedPilotsIds;
    QHash<QString, int> processedTailsIds;
};

#endif // PROCESSFLIGHTS_H
