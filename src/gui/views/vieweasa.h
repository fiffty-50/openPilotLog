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
#ifndef VIEWEASA_H
#define VIEWEASA_H

#include "src/classes/settings.h"
#include "src/gui/styleddelegates/styledairportdelegate.h"
#include "src/gui/styleddelegates/styledcountdelegate.h"
#include "src/gui/styleddelegates/styledpilotdelegate.h"
#include "src/gui/styleddelegates/styledregistrationdelegate.h"
#include "src/gui/styleddelegates/styledtimedelegate.h"
#include "src/gui/styleddelegates/styledtypedelegate.h"
#include "src/gui/views/logbookview.h"
#include <QModelIndex>
#include <QObject>
#include <QTableView>

class ViewEasa : public LogbookViewInfo {
  public:
    ViewEasa() : m_hideSimulator(true) {}
    QString databaseName() override
    {
        return OPL::GLOBALS->getLogbookViewName(OPL::LogbookView::Easa);
    }

    void setupView(QSqlTableModel *model, QTableView *view) override
    {
        LogbookViewInfo::init(model, view);
        auto time_delegate  = new StyledTimeDelegate(Settings::getTimeFormatString(), view);
        auto pilot_delegate = new StyledPilotDelegate(view);
        auto type_delegate  = new StyledTypeDelegate(view);
        auto tail_delegate  = new StyledRegistrationDelegate(view);
        auto count_delegate = new StyledCountDelegate(view);
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
            {COL_T_PIC,         time_delegate   },
            {COL_T_SIC,         time_delegate   },
            {COL_T_DUAL,        time_delegate   },
            {COL_T_FI,          time_delegate   },
            {COL_T_NIGHT,       time_delegate   },
            {COL_T_IFR,         time_delegate   },
            {COL_SP_SE,         time_delegate   },
            {COL_SP_ME,         time_delegate   },
            {COL_MP,            time_delegate   },
            {COL_TIME_TOTAL_MS, time_delegate   },
            {COL_LDG_DAY,       count_delegate  },
            {COL_LDG_NIGHT,     count_delegate  },
        };

        for (auto it = s_delegate_map.cbegin(); it != s_delegate_map.cend(); ++it) {
            view->setItemDelegateForColumn(it.key(), it.value());
        }

        for (int i = 0; i < S_HEADER_NAMES.size(); i++) {
            model->setHeaderData(i, Qt::Horizontal, S_HEADER_NAMES[i]);
        }

        if (m_hideSimulator) {
            view->hideColumn(COL_SIM_TYPE);
            view->hideColumn(COL_SIM_DURATION);
        }

        view->resizeColumnsToContents();
    }

  protected:
    bool m_hideSimulator;
    ViewEasa(bool hideSimulator) : m_hideSimulator(hideSimulator) {};
    QList<int> getFlightSummaryColumns() override
    {
        return {COLUMN_DATE_JD, COL_DEPT_ID, COL_TIME_OFF_MS, COL_TIME_ON_MS, COL_TIME_TOTAL_MS};
    }
    QList<int> getSimSummaryColumns() override
    {
        return {COLUMN_DATE_JD, COL_SIM_TYPE, COL_SIM_DURATION, COL_REMARKS};
    }

  private:
    static const inline QStringList S_HEADER_NAMES = {
        QStringLiteral("event_id"),   // event id column - hidden
        QStringLiteral("event_type"), // event type - hidden
        QObject::tr("Date of Flight"),
        QStringLiteral("flight_id"), // flight id column - hidden
        QObject::tr("Dept"),
        QObject::tr("Time"),
        QObject::tr("Dest"),
        QObject::tr("Time"),
        QObject::tr("Type"),
        QObject::tr("Registration"),
        QObject::tr("SP SE"),
        QObject::tr("SP ME"),
        QObject::tr("MP"),
        QObject::tr("Total"),
        QObject::tr("Name PIC"),
        QObject::tr("L/D"),
        QObject::tr("L/N"),
        QObject::tr("Night"),
        QObject::tr("IFR"),
        QObject::tr("PIC"),
        QObject::tr("SIC"),
        QObject::tr("DUAL"),
        QObject::tr("FI"),
        QObject::tr("Sim Type"),
        QObject::tr("Sim Duration"),
        QObject::tr("Remarks"),
    };

    static constexpr int COL_DEPT_ID       = 4;
    static constexpr int COL_TIME_OFF_MS   = 5;
    static constexpr int COL_DEST_ID       = 6;
    static constexpr int COL_TIME_ON_MS    = 7;
    static constexpr int COL_TYPE_ID       = 8;
    static constexpr int COL_TAIL_ID       = 9;
    static constexpr int COL_SP_SE         = 10;
    static constexpr int COL_SP_ME         = 11;
    static constexpr int COL_MP            = 12;
    static constexpr int COL_TIME_TOTAL_MS = 13;
    static constexpr int COL_PIC_ID        = 14;
    static constexpr int COL_LDG_DAY       = 15;
    static constexpr int COL_LDG_NIGHT     = 16;
    static constexpr int COL_T_NIGHT       = 17;
    static constexpr int COL_T_IFR         = 18;
    static constexpr int COL_T_PIC         = 19;
    static constexpr int COL_T_SIC         = 20;
    static constexpr int COL_T_DUAL        = 21;
    static constexpr int COL_T_FI          = 22;
    static constexpr int COL_SIM_TYPE      = 23;
    static constexpr int COL_SIM_DURATION  = 24;
    static constexpr int COL_REMARKS       = 25;
};

class ViewEasaWithSim : public ViewEasa {
  public:
    ViewEasaWithSim() : ViewEasa(false) {}
    QString databaseName() override
    {
        return OPL::GLOBALS->getLogbookViewName(OPL::LogbookView::EasaWithSim);
    }
};

#endif // VIEWEASA_H
