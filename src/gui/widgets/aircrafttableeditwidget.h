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
#ifndef AIRCRAFTTABLEEDITWIDGET_H
#define AIRCRAFTTABLEEDITWIDGET_H

#include "src/opl.h"
#include "tableeditwidget.h"

class AircraftTableEditWidget : public TableEditWidget {
    Q_OBJECT
  public:
    AircraftTableEditWidget() = delete;
    explicit AircraftTableEditWidget(QWidget *parent = nullptr);

    // TableEditWidget interface
    void setupModelAndView() override;
    QString deleteErrorString(int rowId) override;
    QString confirmDeleteString(int rowId) override;
    EntryEditDialog *createEntryEditDialog() override;

  private:
    void retranslateUi();
    void setupFilterComboBox();

    static constexpr int COL_ROWID           = 0;
    static constexpr int COL_MAKE            = 1;
    static constexpr int COL_MODEL           = 2;
    static constexpr int COL_VARIANT         = 3;
    static constexpr int COL_ICAO_DESIGNATOR = 4;
    static constexpr int COL_ENGINE_TYPE     = 5;
    static constexpr int COL_IS_MULTI_ENGINE = 6;
    static constexpr int COL_CLASS           = 7;
    static constexpr int COL_SUB_CLASS       = 8;
    static constexpr int COL_IS_MULTI_PILOT  = 9;
    static constexpr int COL_WAKE_CATEGORY   = 10;
    static constexpr int COL_TYPE_RATING     = 11;
    static constexpr int COL_REMARKS         = 12;

    const QString COL_HEADER_MAKE            = tr("Make");
    const QString COL_HEADER_MODEL           = tr("Model");
    const QString COL_HEADER_VARIANT         = tr("Variant");
    const QString COL_HEADER_ICAO_DESIGNATOR = tr("ICAO Designator");
    const QString COL_HEADER_ENGINE_TYPE     = tr("Engine Type");
    const QString COL_HEADER_IS_MULTI_ENGINE = tr("Engine Count");
    const QString COL_HEADER_CLASS           = tr("Class");
    const QString COL_HEADER_SUB_CLASS       = tr("Sub Class");
    const QString COL_HEADER_IS_MULTI_PILOT  = tr("Multi-Pilot");
    const QString COL_HEADER_WAKE_CATEGORY   = tr("Wake Category");
    const QString COL_HEADER_TYPE_RATING     = tr("Type Rating");
    const QString COL_HEADER_REMARKS         = tr("Remarks");

    const QMap<int, QString> COLUMN_HEADERS_MAP = {
        {COL_MAKE,            COL_HEADER_MAKE           },
        {COL_MODEL,           COL_HEADER_MODEL          },
        {COL_VARIANT,         COL_HEADER_VARIANT        },
        {COL_ICAO_DESIGNATOR, COL_HEADER_ICAO_DESIGNATOR},
        {COL_ENGINE_TYPE,     COL_HEADER_ENGINE_TYPE    },
        {COL_IS_MULTI_ENGINE, COL_HEADER_IS_MULTI_ENGINE},
        {COL_CLASS,           COL_HEADER_CLASS          },
        {COL_SUB_CLASS,       COL_HEADER_SUB_CLASS      },
        {COL_WAKE_CATEGORY,   COL_HEADER_WAKE_CATEGORY  },
        {COL_TYPE_RATING,     COL_HEADER_TYPE_RATING    },
        {COL_REMARKS,         COL_HEADER_REMARKS        },
    };

    const QList<int> HIDDEN_COLUMNS = {COL_ROWID, COL_IS_MULTI_PILOT};

    const QList<int> VISIBLE_COLUMNS = {
        COL_MAKE,          COL_MODEL,           COL_VARIANT, COL_ICAO_DESIGNATOR,
        COL_ENGINE_TYPE,   COL_IS_MULTI_ENGINE, COL_CLASS,   COL_SUB_CLASS,
        COL_WAKE_CATEGORY, COL_TYPE_RATING,     COL_REMARKS};

    const QMap<int, QString> *getColumnHeaderMap() const override { return &COLUMN_HEADERS_MAP; }
    const QList<int> *getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QList<int> *getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QString tableName() const override
    {
        return OPL::GLOBALS->getDbTableName(OPL::DbTable::v2AircraftTypes);
    }
};

#endif // AIRCRAFTTABLEEDITWIDGET_H
