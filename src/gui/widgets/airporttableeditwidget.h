/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef AIRPORTTABLEEDITWIDGET_H
#define AIRPORTTABLEEDITWIDGET_H

#include "src/opl.h"
#include "tableeditwidget.h"
#include <QObject>

class AirportTableEditWidget : public TableEditWidget {
    Q_OBJECT
  public:
    AirportTableEditWidget() = delete;
    AirportTableEditWidget(QWidget *parent = nullptr);

    // TableEditWidget interface
    virtual void retranslateUi() override;
    virtual QString deleteErrorString(int rowId) override;
    virtual QString confirmDeleteString(int rowId) override;
    virtual EntryEditDialog *createEntryEditDialog() override;

  private:
    // table columns and header names

    static constexpr int COL_ROWID        = 0;
    static constexpr int COL_IATA         = 1;
    static constexpr int COL_ICAO         = 2;
    static constexpr int COL_TIMEZONE     = 3;
    static constexpr int COL_AIRPORT_NAME = 4;

    const QList<int> HIDDEN_COLUMNS  = {0};
    const QList<int> VISIBLE_COLUMNS = {1, 2, 3, 4};

    const QString COL_HEADER_ICAO     = tr("ICAO");
    const QString COL_HEADER_IATA     = tr("IATA");
    const QString COL_HEADER_TIMEZONE = tr("Timezone");
    const QString COL_HEADER_NAME     = tr("Name");

    // used to display the Header Views and Fill the FilterComboBox
    const QMap<int, QString> HEADER_NAMES = {
        {COL_ICAO,         COL_HEADER_ICAO    },
        {COL_IATA,         COL_HEADER_IATA    },
        {COL_TIMEZONE,     COL_HEADER_TIMEZONE},
        {COL_AIRPORT_NAME, COL_HEADER_NAME    },
    };

    // used to map filter combo box values to column names
    const QMap<QString, QString> COLUMN_DATABASE_NAMES = {
        {COL_HEADER_ICAO,     QStringLiteral("icao_code")},
        {COL_HEADER_IATA,     QStringLiteral("iata_code")},
        {COL_HEADER_TIMEZONE, QStringLiteral("timezone") },
        {COL_HEADER_NAME,     QStringLiteral("name")     },
    };

    const QList<int> *getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QList<int> *getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QMap<int, QString> *getColumnHeaderMap() const override { return &HEADER_NAMES; }
    const QString tableName() const override
    {
        return OPL::GLOBALS->getDatabaseViewName(OPL::DatabaseView::Airports);
    }
};

#endif // AIRPORTTABLEEDITWIDGET_H
