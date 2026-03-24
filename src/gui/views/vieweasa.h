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
        else {
            view->setItemDelegateForColumn(COL_SIM_DURATION, time_delegate);
        }

        view->resizeColumnsToContents();
    }
    const QMap<int, QString> *getColumnHeaderMap() const override
    {
        if (m_hideSimulator)
            return &S_HEADER_MAP;
        else
            return &S_HEADER_MAP_SIM;
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

    static const inline QString HEADER_DATE         = QStringLiteral("Date of Flight");
    static const inline QString HEADER_DEPT         = QStringLiteral("Dept");
    static const inline QString HEADER_TIME_OFF     = QStringLiteral("Time");
    static const inline QString HEADER_DEST         = QStringLiteral("Dest");
    static const inline QString HEADER_TIME_ON      = HEADER_TIME_OFF;
    static const inline QString HEADER_TYPE         = QStringLiteral("Type");
    static const inline QString HEADER_TAIL         = QStringLiteral("Registration");
    static const inline QString HEADER_TIME_SPSE    = QStringLiteral("SP SE");
    static const inline QString HEADER_TIME_SPME    = QStringLiteral("SP ME");
    static const inline QString HEADER_TIME_MP      = QStringLiteral("MP");
    static const inline QString HEADER_TIME_TOTAL   = QStringLiteral("Total");
    static const inline QString HEADER_PIC          = QStringLiteral("Name PIC");
    static const inline QString HEADER_LDG_DAY      = QStringLiteral("L/D");
    static const inline QString HEADER_LDG_NIGHT    = QStringLiteral("L/N");
    static const inline QString HEADER_TIME_NIGHT   = QStringLiteral("Night");
    static const inline QString HEADER_TIME_IFR     = QStringLiteral("IFR");
    static const inline QString HEADER_TIME_PIC     = QStringLiteral("PIC");
    static const inline QString HEADER_TIME_SIC     = QStringLiteral("SIC");
    static const inline QString HEADER_TIME_DUAL    = QStringLiteral("DUAL");
    static const inline QString HEADER_TIME_FI      = QStringLiteral("FI");
    static const inline QString HEADER_SIM_TYPE     = QStringLiteral("Sim Type");
    static const inline QString HEADER_SIM_DURATION = QStringLiteral("Sim Duration");
    static const inline QString HEADER_REMARKS      = QStringLiteral("Remarks");

    // This needs to include invisible columns so that we can easily iterate through all headers
    static const inline QStringList S_HEADER_NAMES = {
        HEADER_EVENT_ID,     HEADER_EVENT_TYPE, HEADER_DATE,      HEADER_FOREIGN_ID,
        HEADER_DEPT,         HEADER_TIME_OFF,   HEADER_DEST,      HEADER_TIME_ON,
        HEADER_TYPE,         HEADER_TAIL,       HEADER_TIME_SPSE, HEADER_TIME_SPME,
        HEADER_TIME_MP,      HEADER_TIME_TOTAL, HEADER_PIC,       HEADER_LDG_DAY,
        HEADER_LDG_NIGHT,    HEADER_TIME_NIGHT, HEADER_TIME_IFR,  HEADER_TIME_PIC,
        HEADER_TIME_SIC,     HEADER_TIME_DUAL,  HEADER_TIME_FI,   HEADER_SIM_TYPE,
        HEADER_SIM_DURATION, HEADER_REMARKS};

    static const inline QMap<int, QString> S_HEADER_MAP{
        {COLUMN_DATE_JD,    HEADER_DATE      },
        {COL_DEPT_ID,       HEADER_DEPT      },
        {COL_TIME_OFF_MS,   HEADER_TIME_OFF  },
        {COL_DEST_ID,       HEADER_DEST      },
        {COL_TIME_ON_MS,    HEADER_TIME_ON   },
        {COL_TYPE_ID,       HEADER_TYPE      },
        {COL_TAIL_ID,       HEADER_TAIL      },
        {COL_SP_SE,         HEADER_TIME_SPSE },
        {COL_SP_ME,         HEADER_TIME_SPME },
        {COL_MP,            HEADER_TIME_MP   },
        {COL_TIME_TOTAL_MS, HEADER_TIME_TOTAL},
        {COL_PIC_ID,        HEADER_PIC       },
        {COL_LDG_DAY,       HEADER_LDG_DAY   },
        {COL_LDG_NIGHT,     HEADER_LDG_NIGHT },
        {COL_T_NIGHT,       HEADER_TIME_NIGHT},
        {COL_T_IFR,         HEADER_TIME_IFR  },
        {COL_T_PIC,         HEADER_TIME_PIC  },
        {COL_T_SIC,         HEADER_TIME_SIC  },
        {COL_T_DUAL,        HEADER_TIME_DUAL },
        {COL_T_FI,          HEADER_TIME_FI   },
        {COL_REMARKS,       HEADER_REMARKS   },
    };

    static const inline QMap<int, QString> S_HEADER_MAP_SIM{
        {COLUMN_DATE_JD,    HEADER_DATE        },
        {COL_DEPT_ID,       HEADER_DEPT        },
        {COL_TIME_OFF_MS,   HEADER_TIME_OFF    },
        {COL_DEST_ID,       HEADER_DEST        },
        {COL_TIME_ON_MS,    HEADER_TIME_ON     },
        {COL_TYPE_ID,       HEADER_TYPE        },
        {COL_TAIL_ID,       HEADER_TAIL        },
        {COL_SP_SE,         HEADER_TIME_SPSE   },
        {COL_SP_ME,         HEADER_TIME_SPME   },
        {COL_MP,            HEADER_TIME_MP     },
        {COL_TIME_TOTAL_MS, HEADER_TIME_TOTAL  },
        {COL_PIC_ID,        HEADER_PIC         },
        {COL_LDG_DAY,       HEADER_LDG_DAY     },
        {COL_LDG_NIGHT,     HEADER_LDG_NIGHT   },
        {COL_T_NIGHT,       HEADER_TIME_NIGHT  },
        {COL_T_IFR,         HEADER_TIME_IFR    },
        {COL_T_PIC,         HEADER_TIME_PIC    },
        {COL_T_SIC,         HEADER_TIME_SIC    },
        {COL_T_DUAL,        HEADER_TIME_DUAL   },
        {COL_T_FI,          HEADER_TIME_FI     },
        {COL_SIM_TYPE,      HEADER_SIM_TYPE    },
        {COL_SIM_DURATION,  HEADER_SIM_DURATION},
        {COL_REMARKS,       HEADER_REMARKS     },
    };

    const QList<int> *getFlightSummaryColumns() override { return &S_FLIGHT_SUMMARY_COLUMNS; }
    const QList<int> *getSimSummaryColumns() override { return &S_SIM_SUMMARY_COLUMNS; }

    const static inline QList<int> S_SIM_SUMMARY_COLUMNS    = {COLUMN_DATE_JD, COL_SIM_TYPE,
                                                               COL_SIM_DURATION, COL_REMARKS};
    const static inline QList<int> S_FLIGHT_SUMMARY_COLUMNS = {
        COLUMN_DATE_JD, COL_DEPT_ID, COL_TIME_OFF_MS, COL_TIME_ON_MS, COL_TIME_TOTAL_MS};

    const static inline QList<int> S_VISIBLE_COLUMNS     = S_HEADER_MAP.keys();
    const static inline QList<int> S_VISIBLE_COLUMNS_SIM = S_HEADER_MAP_SIM.keys();

  protected:
    ViewEasa(bool hideSimulator) : m_hideSimulator(hideSimulator) {};
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
