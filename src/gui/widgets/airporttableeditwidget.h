#ifndef AIRPORTTABLEEDITWIDGET_H
#define AIRPORTTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include <QObject>
#include "src/database/airportentry.h"

class AirportTableEditWidget : public TableEditWidget
{
    Q_OBJECT
public:
    AirportTableEditWidget() = delete;
    AirportTableEditWidget(QWidget *parent = nullptr);

    // TableEditWidget interface
    virtual void setupModelAndView() override;
    virtual void setupUI() override;
    virtual QString deleteErrorString(int rowId) override;
    virtual QString confirmDeleteString(int rowId) override;
    virtual EntryEditDialog *getEntryEditDialog(QWidget *parent) override;

    // table columns and header names

    const int COL_ROWID = 0;

    // used to display the Header Views and Fill the FilterComboBox
    const QStringList HEADER_NAMES = {
                                      tr("ICAO"),
                                      tr("IATA"),
                                      tr("Name"),
                                      tr("Latitude"),
                                      tr("Longitude"),
                                      tr("Country"),
                                      tr("Time Zone"),
    };

    // These are indexes into HEADER_NAMES
    const int FILTER_COLUMNS[4] = {0, 1, 2, 5};

    // The sql column names corresponding to the entries of the FilterComboBox index
    const static inline QStringList FILTER_COLUMN_NAMES = {
        OPL::AirportEntry::ICAO,
        OPL::AirportEntry::IATA,
        OPL::AirportEntry::NAME,
        OPL::AirportEntry::TZ_OLSON,
    };

private slots:
    virtual void filterTextChanged(const QString &filterString) override;
};

#endif // AIRPORTTABLEEDITWIDGET_H
