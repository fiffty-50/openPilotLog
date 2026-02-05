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
#ifndef AIRPORTINFO_H
#define AIRPORTINFO_H

#include <QObject>
#include <qglobalstatic.h>
#include <qsqltablemodel.h>
/*!
 * \class AirportInfo
 * \brief Provides access to airport information from the database.
 *
 * \details
 * The AirportInfo class manages airport data using a QSqlTableModel and
 * provides lookup methods by airport_id, ICAO code, or IATA code.
 * Access to a global instance is provided via the Q_GLOBAL_STATIC macro
 * as \c airportData:
 * \code
 * Q_GLOBAL_STATIC(AirportInfo, airportData)
 * \endcode
 */
class AirportInfo : public QObject {
    Q_OBJECT
  public:
    explicit AirportInfo(QObject *parent = nullptr);

    void init() { refreshIndices(); }

    /*!
     * \brief Returns the airport name for the given row ID.
     */
    QString nameFromRowId(int airport_id);

    /*!
     * \brief Returns whether an airport id contains in the database.
     */
    bool contains(int airport_id) const;

    /*!
     * \brief Returns the airport name for the given ICAO code.
     */
    QString nameFromIcao(const QString &icao);

    /*!
     * \brief Returns the airport name for the given IATA code.
     */
    QString nameFromIata(const QString &iata);

    /*!
     * \brief Returns the airport_id for the given ICAO code.
     */
    int idFromIcao(const QString &icao);

    /*!
     * \brief Returns the airport_id for the given IATA code.
     */
    int idFromIata(const QString &iata);

    /*!
     * \brief Returns the ICAO code for the given row ID.
     */
    QString icao(int airport_id);

    /*!
     * \brief Returns the ICAO code for the airport with the given IATA code.
     */
    QString icao(const QString &iata);

    /*!
     * \brief Returns the IATA code for the given row ID.
     */
    QString iata(int airport_id);

    /*!
     * \brief Returns the IATA code for the airport with the given ICAO code.
     */
    QString iata(const QString &icao);

    const QMap<QString, int>& allCodesMap() const { return m_allCodesToAirportId; }

  private:
    QSqlTableModel *m_model = nullptr;

    // Maps for fast lookup
    QHash<int, int> m_airportIdToRow; // airport_id → row in model
    QHash<QString, int> m_icaoToAirportId;
    QHash<QString, int> m_iataToAirportId;
    QMap<QString, int> m_allCodesToAirportId;

    const static inline QString FIELD_IATA  = QStringLiteral("iata_code");
    const static inline QString FIELD_ICAO  = QStringLiteral("icao_code");
    const static inline QString FIELD_NAMES = QStringLiteral("name");

    static constexpr int COLUMN_AIRPORT_ID = 0;
    static constexpr int COLUMN_IATA       = 1;
    static constexpr int COLUMN_ICAO       = 2;
    static constexpr int COLUMN_NAMES      = 4;

    void refreshIndices();

    QString dataForAirportId(int airportId, int column) const;
};

Q_GLOBAL_STATIC(AirportInfo, airportData)

#endif // AIRPORTINFO_H
