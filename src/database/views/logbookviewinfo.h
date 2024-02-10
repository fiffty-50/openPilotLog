#ifndef LOGBOOKVIEWINFO_H
#define LOGBOOKVIEWINFO_H

#include "src/opl.h"
#include <QtCore>

namespace OPL {


/*!
 * \brief The LogbookViewInfo class is a base class for classes that encapsulate information
 * about a LogbookView
 * \details In the logbook display, SQL views are used instead of raw table data, since in
 * some cases data is aggregated from different tables. Using views avoid
 */
class LogbookViewInfo : public QObject {
    Q_OBJECT // enable tr()
public:
    
    /*!
     * \brief Return the column in the view which contains the date of flight
     */
    static constexpr int getDateColumn(LogbookView view)
    {
        return DATE_COLUMNS.at(static_cast<int>(view));
    }

    /*!
     * \brief Return the column in the view which contains the aircrafts type
     */
    static constexpr int getTypeColumn(LogbookView view)
    {
        return TYPE_COLUMNS.at(static_cast<int>(view));
    }
    
    /*!
     * \brief Return the column(s) in the view which contain pilot names
     */
    static constexpr int getPicColumn(LogbookView view)
    {
        return PIC_COLUMNS.at(static_cast<int>(view));
    }
    
    /*!
     * \brief Return the column(s) in the view which contain Time entries
     */
    static constexpr std::vector<int> getTimeColumns(LogbookView view)
    {
        switch (view) {
        case LogbookView::Default:
            return { 3, 5, 6 };
        case LogbookView::DefaultWithSim:
            return { 3, 5, 6, 11 };
        case LogbookView::Easa:
            return { 3, 5, 8, 9, 10, 11, 15, 16, 17, 18, 19, 20 };
        case LogbookView::EasaWithSim:
            return { 3, 5, 8, 9, 10, 11, 15, 16, 17, 18, 19, 20, 22 };
        default:
            assert(((void)"View is not implemented", false));
            return { 0 };
        }
    }

    // translations need to be done at runtime
    static const QStringList getTableHeaders(LogbookView view)
    {
        switch (view) {
        case LogbookView::Default:
            return {
                QStringLiteral("flight_id"), // flight id column - hidden
                tr("Date of Flight"),
                tr("Dept"),
                tr("Time"),
                tr("Dest"),
                tr("Time"),
                tr("Total"),
                tr("Name PIC"),
                tr("Type"),
                tr("Registration"),
                tr("Flight Number"),
                tr("Remarks"),
            };
        case LogbookView::DefaultWithSim:
            return {
                QStringLiteral("flight_id"), // flight id column - hidden
                tr("Date of Flight"),
                tr("Dept"),
                tr("Time"),
                tr("Dest"),
                tr("Time"),
                tr("Total"),
                tr("Name PIC"),
                tr("Type"),
                tr("Registration"),
                tr("Sim Type"),
                tr("Time of Session"),
                tr("Remarks"),
            };
        case LogbookView::Easa:
            return {
                QStringLiteral("flight_id"), // flight id column - hidden
                tr("Date of Flight"),
                tr("Dept"),
                tr("Time"),
                tr("Dest"),
                tr("Time"),
                tr("Type"),
                tr("Registration"),
                tr("SP SE"),
                tr("SP ME"),
                tr("MP"),
                tr("Total"),
                tr("Name PIC"),
                tr("L/D"),
                tr("L/N"),
                tr("Night"),
                tr("IFR"),
                tr("PIC"),
                tr("SIC"),
                tr("Dual"),
                tr("FI"),
                tr("Remarks"),
            };
        case LogbookView::EasaWithSim:
            return {
                QStringLiteral("flight_id"), // flight id column - hidden
                tr("Date of Flight"),
                tr("Dept"),
                tr("Time"),
                tr("Dest"),
                tr("Time"),
                tr("Type"),
                tr("Registration"),
                tr("SP SE"),
                tr("SP ME"),
                tr("MP"),
                tr("Total"),
                tr("Name PIC"),
                tr("L/D"),
                tr("L/N"),
                tr("Night"),
                tr("IFR"),
                tr("PIC"),
                tr("SIC"),
                tr("Dual"),
                tr("FI"),
                tr("Sim Type"),
                tr("Time of Session"),
                tr("Remarks"),
            };
        default:
            assert(((void)"View is not implemented", false));
            return {};
        }
    }

private:

    static constexpr std::array DATE_COLUMNS {
        1, // Default
        1, // Default With Sim
        1, // Easa
        1, // Easa With Sim
        1, // Simulator Only
    };

    static constexpr std::array TYPE_COLUMNS {
        8, // Default
        8, // Default With Sim
        6, // Easa
        6, // Easa With Sim
        1, // Simulator Only
    };

    static constexpr std::array PIC_COLUMNS {
        7, // Default
        7, // Default With Sim
        12, // Easa
        12, // Easa With Sim
        -1, // Simulator Only
    };

};

} // namespace OPL
#endif // LOGBOOKVIEWINFO_H
