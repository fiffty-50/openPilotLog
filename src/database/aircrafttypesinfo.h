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
#ifndef AIRCRAFTTYPESINFO_H
#define AIRCRAFTTYPESINFO_H

#include <QHash>
#include <QObject>

/*!
 * \class AircraftTypesInfo
 * \brief Provides lookup for aircraft type identifiers.
 *
 * \details
 * AircraftTypesInfo loads aircraft type identifiers from the database using
 * a synthesized display string in the form:
 *
 *   "make model"
 *   "make model-variant" (if variant is not null)
 *
 * depending on whether a variant exists.
 *
 * The class provides fast lookup between aircraft type IDs (ROWID)
 * and their human-readable identifiers.
 *
 * Access to a global instance is provided via:
 * \code
 * Q_GLOBAL_STATIC(AircraftTypesInfo, aircraftTypesData)
 * \endcode
 */
class AircraftTypesInfo : public QObject {
    Q_OBJECT
  public:
    explicit AircraftTypesInfo(QObject *parent = nullptr);

    /*!
     * \brief Returns the aircraft type ID for the given identifier string.
     */
    int id(const QString &type_string);

    /*!
     * \brief Returns the identifier string for the given aircraft type ID.
     */
    QString typeString(int type_id);

  private:
    QHash<QString, int> m_identToId;
    QHash<int, QString> m_idToIdent;

    void refresh();
};

Q_GLOBAL_STATIC(AircraftTypesInfo, aircraftTypesData)

#endif // AIRCRAFTTYPESINFO_H
