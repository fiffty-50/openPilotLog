#ifndef LOGBOOKVIEWINFO_H
#define LOGBOOKVIEWINFO_H

#include "src/classes/settings.h"
#include "src/gui/styleddelegates/styledairportdelegate.h"
#include "src/gui/styleddelegates/styleddatedelegate.h"
#include "src/gui/styleddelegates/styledpilotdelegate.h"
#include "src/gui/styleddelegates/styledregistrationdelegate.h"
#include "src/gui/styleddelegates/styledtimedelegate.h"
#include "src/gui/styleddelegates/styledtypedelegate.h"
#include <QHash>
#include <QTableView>
#include <QSqlTableModel>
#include <QHeaderView>

/*!
 * \brief The LogbookView class holds constants that are needed to set up a QTableView
 * \details Logbook views are SQL views that aggregate data from the database and present
 * them in the GUI. Since the views hold raw data, special QSyledItemDelegate instances
 * are needed to display user-friendly data. Furthermore, not all columns should be visible
 * to the user. This class provides static functions to
 * - set custom delegates
 * - set localised user-friendly header names
 * - hide columns that are not to be shown
 * - format the QTableView to a common layout
 */
class LogbookView {

  public:
    LogbookView() = delete;
    const static inline void setupView(OPL::LogbookView view_name, QSqlTableModel* model, QTableView *view) {
        // layout setup
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setSelectionBehavior(QAbstractItemView::SelectRows);
        view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
        view->verticalHeader()->hide();
        view->setAlternatingRowColors(true);

        // set delegates and hide columns per view
        switch (view_name) {
        case OPL::LogbookView::Default:
            setupDefaultView(model, view);
            return;
        case OPL::LogbookView::DefaultWithSim:
            defaultWithSimDelegateMap(model, view);
            return;
        case OPL::LogbookView::Easa:
            easaDelegateMap(model, view);
            return;
        case OPL::LogbookView::EasaWithSim:
            easaWithSimDelegateMap(model, view);
            return;
        case OPL::LogbookView::SimulatorOnly:
            SimulatorOnlyDelegateMap(model, view);
            return;
            break;
        default:
            return;
        }

    }

  private:

    const static inline QList<int> S_DEFAULT_COLS_TO_HIDE = {0, 1};
    const static inline QStringList S_DEFAULT_HEADER_NAMES = {
        QStringLiteral("event_id"), // event id column - hidden
        QStringLiteral("flight_id"), // flight id column - hidden
        QObject::tr("Date of Flight"),
        QObject::tr("Dept"),
        QObject::tr("Time"),
        QObject::tr("Dest"),
        QObject::tr("Time"),
        QObject::tr("Total"),
        QObject::tr("Name PIC"),
        QObject::tr("Type"),
        QObject::tr("Registration"),
        QObject::tr("Flight Number"),
        QObject::tr("Remarks"),
    };

    const static inline void setupDefaultView(QSqlTableModel *model, QTableView *view) {
        auto date_delegate = new StyledDateDelegate(Settings::getDateFormatString(), view);
        auto time_delegate = new StyledTimeDelegate(Settings::getTimeFormatString(), view);
        auto pilot_delegate = new StyledPilotDelegate(view);
        auto type_delegate = new StyledTypeDelegate(view);
        auto tail_delegate = new StyledRegistrationDelegate(view);
        auto airport_delegate = new StyledAirportDelegate(StyledAirportDelegate::Icao, view); // TODO style setting

        const static QHash<int, QStyledItemDelegate *> s_delegate_map = {
            { 2, date_delegate    },
            { 3, airport_delegate },
            { 4, time_delegate    },
            { 5, airport_delegate },
            { 6, time_delegate    },
            { 8, pilot_delegate   },
            { 9, type_delegate    },
            { 10, tail_delegate   },
            };

        for(auto it = s_delegate_map.cbegin(); it != s_delegate_map.cend(); ++it) {
            view->setItemDelegateForColumn(it.key(), it.value());
        }

        for(const auto & col : S_DEFAULT_COLS_TO_HIDE) {
            view->hideColumn(col);
        }

        for (int i = 0; i < S_DEFAULT_HEADER_NAMES.size(); i++) {
            model->setHeaderData(i, Qt::Horizontal, S_DEFAULT_HEADER_NAMES[i]);
        }

        view->resizeColumnsToContents();
    };

    const static inline QHash<int, QStyledItemDelegate *> defaultWithSimDelegateMap(QSqlTableModel *model, QTableView *view) { assert(false); }
    const static inline QHash<int, QStyledItemDelegate *> easaDelegateMap(QSqlTableModel *model, QTableView *view) { assert(false); }
    const static inline QHash<int, QStyledItemDelegate *> easaWithSimDelegateMap(QSqlTableModel *model, QTableView *view) { assert(false); }
    const static inline QHash<int, QStyledItemDelegate *> SimulatorOnlyDelegateMap(QSqlTableModel *model, QTableView *view) { assert(false); }
};

#endif // LOGBOOKVIEWINFO_H
