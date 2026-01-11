#ifndef AIRPORTTABLEEDITWIDGET_H
#define AIRPORTTABLEEDITWIDGET_H

#include "tableeditwidget.h"
#include <QObject>

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

private:
    // table columns and header names

    static constexpr int COL_ROWID = 0;
    static constexpr int COL_ICAO = 1;
    static constexpr int COL_IATA = 2;
    static constexpr int COL_TIMEZONE = 3;
    static constexpr int COL_AIRPORT_NAME = 4;

    const QString COL_HEADER_ICAO = tr("ICAO");
    const QString COL_HEADER_IATA = tr("IATA");
    const QString COL_HEADER_TIMEZONE = tr("Timezone");
    const QString COL_HEADER_NAME = tr("Name");

    // used to display the Header Views and Fill the FilterComboBox
    const QMap<int, QString> HEADER_NAMES = {
                        { COL_ICAO, 		 COL_HEADER_ICAO },
                        { COL_IATA, 		 COL_HEADER_IATA },
                        { COL_TIMEZONE, 	 COL_HEADER_TIMEZONE },
                        { COL_AIRPORT_NAME,  COL_HEADER_NAME },
    };

    // used to map filter combo box values to column names
    const QMap<QString, QString> COLUMN_DATABASE_NAMES = {
        {COL_HEADER_ICAO, 	  QStringLiteral("icao_code") },
        {COL_HEADER_IATA, 	  QStringLiteral("iata_code") },
        {COL_HEADER_TIMEZONE, QStringLiteral("timezone")},
        {COL_HEADER_NAME, 	  QStringLiteral("name")},
    };

private slots:
    virtual void filterTextChanged(const QString &filterString) override;
};

#endif // AIRPORTTABLEEDITWIDGET_H
