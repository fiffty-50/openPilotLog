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
#ifndef PILOTINFO_H
#define PILOTINFO_H
#include <QObject>
#include <QSqlTableModel>

/*!
 * \class PilotsInfo
 * \brief Provides access to pilot information from the database.
 *
 * \details
 * PilotsInfo manages pilot data using a QSqlTableModel and provides
 * lookup by pilot_id or name.
 *
 * Access to a global instance is provided via:
 * \code
 * Q_GLOBAL_STATIC(PilotsInfo, pilotsData)
 * \endcode
 */
class PilotsInfo : public QObject {
    Q_OBJECT
  public:
    explicit PilotsInfo(QObject *parent = nullptr);

    void init() { refreshIndices(); }

    /*!
     * \brief Returns whether a row id exists in the database
     */
    bool exists(int pilot_id) const;

    /*!
     * \brief Returns the pilot name for the given pilot ID.
     */
    QString name(int pilotId) const;

    /*!
     * \brief Returns the alias for the given pilot ID.
     */
    QString alias(int pilotId) const;

    /*!
     * \brief Returns the company for the given pilot ID.
     */
    QString company(int pilotId) const;

    /*!
     * \brief Returns the phone number for the given pilot ID.
     */
    QString phone(int pilotId) const;

    /*!
     * \brief Returns the email address for the given pilot ID.
     */
    QString email(int pilotId) const;

    /*!
     * \brief Returns the remarks for the given pilot ID.
     */
    QString remarks(int pilotId) const;

    /*!
     * \brief Returns the pilot ID for the given pilot name.
     */
    int idFromName(const QString &name) const;

    const QMap<QString, int> &nameMap() const { return m_nameToPilotIdMap; }

  private:
    QSqlTableModel *m_model = nullptr;

    // Maps
    QHash<int, int> m_pilotIdToRow;      // pilot_id → row
    QHash<QString, int> m_nameToPilotId; // name → pilot_id
    QMap<QString, int> m_nameToPilotIdMap;

    static constexpr int COLUMN_PILOT_ID = 0;
    static constexpr int COLUMN_NAME     = 1;
    static constexpr int COLUMN_ALIAS    = 2;
    static constexpr int COLUMN_EMPLOYEE = 3;
    static constexpr int COLUMN_COMPANY  = 4;
    static constexpr int COLUMN_PHONE    = 5;
    static constexpr int COLUMN_EMAIL    = 6;
    static constexpr int COLUMN_REMARKS  = 7;

    static inline std::optional<QString> S_OWNER_ALIAS;

    void refreshIndices();
    QString dataForPilotId(int pilotId, int column) const;
};

Q_GLOBAL_STATIC(PilotsInfo, pilotsData)

#endif // PILOTINFO_H
