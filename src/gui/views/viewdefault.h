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
#include <ctime>

class ViewDefault : public LogbookViewInfo {
  public:
    ViewDefault() : m_hideSimulator(true) {}

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
            {COL_SIM_DURATION,  time_delegate   },
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
        else {
            view->setItemDelegateForColumn(COL_SIM_DURATION, time_delegate);
        }

        view->resizeColumnsToContents();
    }

    const QList<int> *getVisibleColumns() const override
    {
        if (m_hideSimulator)
            return &S_VISIBLE_COLUMNS;
        else
            return &S_VISIBLE_COLUMNS_SIM;
    }

  private:
    bool m_hideSimulator;

    static constexpr int COL_DEPT_ID       = 4;
    static constexpr int COL_TIME_OFF_MS   = 5;
    static constexpr int COL_DEST_ID       = 6;
    static constexpr int COL_TIME_ON_MS    = 7;
    static constexpr int COL_TIME_TOTAL_MS = 8;
    static constexpr int COL_PIC_ID        = 9;
    static constexpr int COL_TYPE_ID       = 10;
    static constexpr int COL_TAIL_ID       = 11;
    static constexpr int COL_FLIGHT_NR     = 12;
    static constexpr int COL_SIM_TYPE      = 13;
    static constexpr int COL_SIM_DURATION  = 14;
    static constexpr int COL_REMARKS       = 15;

    const static inline QString HEADER_DATE         = QObject::tr("Date of Flight");
    const static inline QString HEADER_DEPT         = QObject::tr("Dept");
    const static inline QString HEADER_TIME_OFF     = QObject::tr("Time");
    const static inline QString HEADER_DEST         = QObject::tr("Dest");
    const static inline QString HEADER_TIME_ON      = QObject::tr("Time");
    const static inline QString HEADER_TIME_TOTAL   = QObject::tr("Total");
    const static inline QString HEADER_PIC          = QObject::tr("PIC");
    const static inline QString HEADER_TYPE         = QObject::tr("Type");
    const static inline QString HEADER_TAIL         = QObject::tr("Registration");
    const static inline QString HEADER_FLIGHTT_NO   = QObject::tr("Flight #");
    const static inline QString HEADER_SIM_TYPE     = QObject::tr("Sim Type");
    const static inline QString HEADER_SIM_DURATION = QObject::tr("Sim Duration");
    const static inline QString HEADER_REMARKS      = QObject::tr("Remarks");

    const static inline QMap<int, QString> S_HEADER_MAP = {
        {2,  HEADER_DATE      },
        {4,  HEADER_DEPT      },
        {5,  HEADER_TIME_OFF  },
        {6,  HEADER_DEST      },
        {7,  HEADER_TIME_ON   },
        {8,  HEADER_TIME_TOTAL},
        {9,  HEADER_PIC       },
        {10, HEADER_TYPE      },
        {11, HEADER_TAIL      },
        {12, HEADER_FLIGHTT_NO},
        {15, HEADER_REMARKS   },
    };

    const static inline QMap<int, QString> S_HEADER_MAP_SIM = {
        {2,  HEADER_DATE        },
        {4,  HEADER_DEPT        },
        {5,  HEADER_TIME_OFF    },
        {6,  HEADER_DEST        },
        {7,  HEADER_TIME_ON     },
        {8,  HEADER_TIME_TOTAL  },
        {9,  HEADER_PIC         },
        {10, HEADER_TYPE        },
        {11, HEADER_TAIL        },
        {12, HEADER_FLIGHTT_NO  },
        {13, HEADER_SIM_TYPE    },
        {14, HEADER_SIM_DURATION},
        {15, HEADER_REMARKS     },
    };

    static const inline QStringList S_HEADER_NAMES = {
        HEADER_EVENT_ID,   HEADER_EVENT_TYPE, HEADER_DATE,         HEADER_FOREIGN_ID,
        HEADER_DEPT,       HEADER_TIME_OFF,   HEADER_DEST,         HEADER_TIME_ON,
        HEADER_TIME_TOTAL, HEADER_PIC,        HEADER_TYPE,         HEADER_TAIL,
        HEADER_FLIGHTT_NO, HEADER_SIM_TYPE,   HEADER_SIM_DURATION, HEADER_REMARKS};

    const QList<int> *getFlightSummaryColumns() override { return &S_FLIGHT_SUMMARY_COLUMNS; }
    const QList<int> *getSimSummaryColumns() override { return &S_SIM_SUMMARY_COLUMNS; }
    const QMap<int, QString> *getColumnHeaderMap() const override
    {
        if (m_hideSimulator)
            return &S_HEADER_MAP;
        else
            return &S_HEADER_MAP_SIM;
    }

    static const inline QList<int> S_FLIGHT_SUMMARY_COLUMNS = {
        COLUMN_DATE_JD, COL_DEPT_ID, COL_TIME_OFF_MS, COL_TIME_ON_MS, COL_TIME_TOTAL_MS};
    static const inline QList<int> S_SIM_SUMMARY_COLUMNS = {COLUMN_DATE_JD, COL_SIM_TYPE,
                                                            COL_SIM_DURATION, COL_REMARKS};

    static const inline QList<int> S_VISIBLE_COLUMNS_SIM = S_HEADER_MAP.keys();
    // static const inline QList<int> S_VISIBLE_COLUMNS_SIM = {
    //     COL_DEPT_ID, COL_TIME_OFF_MS, COL_DEST_ID, COL_TIME_ON_MS, COL_TIME_TOTAL_MS,
    //     COL_PIC_ID,  COL_TYPE_ID,     COL_TAIL_ID, COL_FLIGHT_NR,  COL_REMARKS,
    // };

    static const inline QList<int> S_VISIBLE_COLUMNS = S_HEADER_MAP_SIM.keys();
    // static const inline QList<int> S_VISIBLE_COLUMNS = {
    //     COL_DEPT_ID, COL_TIME_OFF_MS, COL_DEST_ID,   COL_TIME_ON_MS, COL_TIME_TOTAL_MS,
    //     COL_PIC_ID, COL_TYPE_ID, COL_TAIL_ID,     COL_FLIGHT_NR, COL_SIM_TYPE, COL_SIM_DURATION,
    //     COL_REMARKS,
    // };

  protected:
    ViewDefault(bool hideSimulator) : m_hideSimulator(hideSimulator) {}
};

class ViewDefaultWithSim : public ViewDefault {
  public:
    ViewDefaultWithSim() : ViewDefault(false) {}
    QString databaseName() override
    {
        return OPL::GLOBALS->getLogbookViewName(OPL::LogbookView::DefaultWithSim);
    }
};
#endif // VIEWDEFAULT_H
