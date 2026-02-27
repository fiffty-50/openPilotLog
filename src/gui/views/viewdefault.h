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
#ifndef VIEWDEFAULT_H
#define VIEWDEFAULT_H

#include "src/classes/settings.h"
#include "src/gui/styleddelegates/styledairportdelegate.h"
#include "src/gui/styleddelegates/styledpilotdelegate.h"
#include "src/gui/styleddelegates/styledregistrationdelegate.h"
#include "src/gui/styleddelegates/styledtimedelegate.h"
#include "src/gui/styleddelegates/styledtypedelegate.h"
#include "src/gui/views/logbookview.h"
#include <QModelIndex>
#include <QObject>
#include <QTableView>

class ViewDefault : public LogbookViewInfo {
  public:
    QString databaseName() override
    {
        return OPL::GLOBALS->getLogbookViewName(OPL::LogbookView::Default);
    }
    void setupView(QSqlTableModel *model, QTableView *view) override
    {
        LogbookViewInfo::init(model, view);
        auto time_delegate  = new StyledTimeDelegate(Settings::getTimeFormatString(), view);
        auto pilot_delegate = new StyledPilotDelegate(view);
        auto type_delegate  = new StyledTypeDelegate(view);
        auto tail_delegate  = new StyledRegistrationDelegate(view);
        auto airport_delegate =
            new StyledAirportDelegate(StyledAirportDelegate::Icao, view); // TODO style setting

        const static QHash<int, QStyledItemDelegate *> s_delegate_map = {
            {COL_DEPT_ID,       airport_delegate},
            {COL_TIME_OFF_MS,   time_delegate   },
            {COL_DEST_ID,       airport_delegate},
            {COL_TIME_ON_MS,    time_delegate   },
            {COL_TIME_TOTAL_MS, time_delegate   },
            {COL_PIC_ID,        pilot_delegate  },
            {COL_TYPE_ID,       type_delegate   },
            {COL_TAIL_ID,       tail_delegate   },
        };

        for (auto it = s_delegate_map.cbegin(); it != s_delegate_map.cend(); ++it) {
            view->setItemDelegateForColumn(it.key(), it.value());
        }

        for (int i = 0; i < S_HEADER_NAMES.size(); i++) {
            model->setHeaderData(i, Qt::Horizontal, S_HEADER_NAMES[i]);
        }

        view->resizeColumnsToContents();
    }

  protected:
    QList<int> getFlightSummaryColumns() override
    {
        return {COLUMN_DATE_JD, COL_DEPT_ID, COL_TIME_OFF_MS, COL_TIME_ON_MS, COL_TIME_TOTAL_MS};
    }
    QList<int> getSimSummaryColumns() override { return {}; } // no SIM data in this view

  private:
    static const inline QStringList S_HEADER_NAMES = {
        QStringLiteral("event_id"),    // event id column - hidden
        QStringLiteral("event_type"),  // event type - hidden
        QObject::tr("Date of Flight"), //
        QStringLiteral("flight_id"),   // flight id column - hidden
        QObject::tr("Dept"),           QObject::tr("Time"),         QObject::tr("Dest"),
        QObject::tr("Time"),           QObject::tr("Total"),        QObject::tr("Name PIC"),
        QObject::tr("Type"),           QObject::tr("Registration"), QObject::tr("Flight Number"),
        QObject::tr("Remarks"),
    };

    static constexpr int COL_DEPT_ID       = 4;
    static constexpr int COL_TIME_OFF_MS   = 5;
    static constexpr int COL_DEST_ID       = 6;
    static constexpr int COL_TIME_ON_MS    = 7;
    static constexpr int COL_TIME_TOTAL_MS = 8;
    static constexpr int COL_PIC_ID        = 9;
    static constexpr int COL_TYPE_ID       = 10;
    static constexpr int COL_TAIL_ID       = 11;
    static constexpr int COL_FLIGHT_NR     = 12;
    static constexpr int COL_REMARKS       = 13;
};

#endif // VIEWDEFAULT_H
