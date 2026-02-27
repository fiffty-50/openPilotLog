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
#ifndef LOGBOOKVIEW_H
#define LOGBOOKVIEW_H

#include "src/classes/settings.h"
#include "src/gui/styleddelegates/styleddatedelegate.h"
#include "src/opl.h"
#include <QHash>
#include <QHeaderView>
#include <QModelIndex>
#include <QObject>
#include <QSqlTableModel>
#include <QTableView>

/*!
 * \brief The LogbookView class holds constants that are needed to set up a QTableView
 * \details Logbook views are SQL views that aggregate data from the database and present
 * them in the GUI. Since the views hold raw data, special QSyledItemDelegate instances
 * are needed to display user-friendly data. The LogbookView class is a base class
 * for derived classes that set up a QTableView based on a SQL view retreived from the
 * database.
 * */
class LogbookViewInfo {

  public:
    LogbookViewInfo()          = default;
    virtual ~LogbookViewInfo() = default;

    /*!
     * \brief returns the name of the view in the database
     * \return
     */
    virtual QString databaseName() = 0;

    /*!
     * \brief Set up a QTableView in accordance with the selected database view
     * \details Sets up the view by:
     * <ul>
     * <li> Setting custom delegates for db->user-friendly formatting </li>
     * <li> Setting user-friendly header names </li>
     * <li> hiding non-user columns </li>
     * </ul>
     */
    virtual void setupView(QSqlTableModel *model, QTableView *view) = 0;

    /*!
     * \brief returns the OPL::LogEvent type for the given QModelIndex
     */
    virtual OPL::LogEvent eventType(const QModelIndex &index)
    {
        if (index.sibling(index.row(), COLUMN_EVENT_TYPE).data().toString() == S_FLIGHT_STRING)
            return OPL::LogEvent::Flight;
        else
            return OPL::LogEvent::Sim;
    }

    int eventId(const QModelIndex &index)
    {
        if (!index.isValid()) return -1;
        return index.sibling(index.row(), COLUMN_LOG_EVENT_ID).data().toInt();
    }

    int foreignId(const QModelIndex &index)
    {
        if (!index.isValid()) return -1;
        return index.sibling(index.row(), COLUMN_FOREIGN_ID).data().toInt();
    }

    /*!
     * \brief Encapsulates the event type, event id, and foreign id (flight_id or simulator_id)
     */
    struct EventInfo {
        OPL::LogEvent type;
        int eventId;
        int foreignId;
    };

    /*!
     * \brief Returns the event_type, event_id and foreign id (FLT or SIM id)
     */
    EventInfo eventInfo(const QModelIndex &index)
    {
        return {eventType(index), eventId(index), foreignId(index)};
    }

    /*!
     * \brief returns a summary for the event at the given QModelIndex
     */
    QString getSummaryString(const QModelIndex &index)
    {
        if (!index.isValid()) return {};
        int row = index.row();

        static constexpr auto space = QLatin1Char(' ');
        if (index.sibling(row, COLUMN_EVENT_TYPE).data().toString() == S_FLIGHT_STRING) {
            // is flight event
            QString summary = S_FLIGHT_STRING + space;
            for (const auto &col : getFlightSummaryColumns()) {
                summary.append(index.sibling(row, col).data().toString() + space); // Date
            }
            return summary;
        }
        else {
            // is sim event
            QString summary = S_SIM_STRING + space;
            for (const auto &col : getSimSummaryColumns()) {
                summary.append(index.sibling(row, col).data().toString() + space); // Date
            }
            return summary;
        }
    }

  protected:
    /*!
     * \brief Does the basic setup of the QTableView. Should be called in each derived class.
     */
    const static inline void init(QSqlTableModel *model, QTableView *view)
    {
        // layout setup
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setSelectionBehavior(QAbstractItemView::SelectRows);
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
        view->verticalHeader()->hide();
        view->setAlternatingRowColors(true);

        // set the default date delegate
        auto date_delegate = new StyledDateDelegate(Settings::getDateFormatString(), view);
        view->setItemDelegateForColumn(COLUMN_DATE_JD, date_delegate);

        // hide event id and type columns
        for (const auto &col : COLUMNS_TO_HIDE) {
            view->hideColumn(col);
        }
    }

    /*!
     * \brief return columns for Date, Departure, Time Off, Destination, Time On
     */
    virtual QList<int> getFlightSummaryColumns() = 0;

    /*!
     * \brief return columns for Sim Type, Duration
     */
    virtual QList<int> getSimSummaryColumns() = 0;

    const static inline QString S_FLIGHT_STRING = QStringLiteral("FLT");
    const static inline QString S_SIM_STRING    = QStringLiteral("SIM");

    // These are mandatory in every view
    static constexpr int COLUMN_LOG_EVENT_ID            = 0;
    static constexpr int COLUMN_EVENT_TYPE              = 1;
    static constexpr int COLUMN_DATE_JD                 = 2;
    static constexpr int COLUMN_FOREIGN_ID              = 3;
    static constexpr std::array<int, 4> COLUMNS_TO_HIDE = {
        COLUMN_LOG_EVENT_ID,
        COLUMN_EVENT_TYPE,
        COLUMN_FOREIGN_ID,
    };
};

#endif // LOGBOOKVIEW_H
