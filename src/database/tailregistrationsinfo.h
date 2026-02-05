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
#ifndef TAILREGISTRATIONSINFO_H
#define TAILREGISTRATIONSINFO_H

#include <QHash>
#include <QMap>
#include <QObject>

/*!
 * \class TailRegistrationsInfo
 * \brief Provides lookup for aircraft tail registrations.
 *
 * \details
 * TailRegistrationsInfo loads aircraft tail registrations from the database
 * and provides fast lookup between tail IDs (ROWID) and registration strings.
 *
 * Access to a global instance is provided via:
 * \code
 * Q_GLOBAL_STATIC(TailRegistrationsInfo, tailsData)
 * \endcode
 */
class TailRegistrationsInfo : public QObject {
    Q_OBJECT
  public:
    explicit TailRegistrationsInfo(QObject *parent = nullptr);

    void init() { refresh(); }

    /*!
     * \brief Returns whether a row id exists in the database
     */
    bool exists(int tail_id) const;

    /*!
     * \brief Returns the tail ID for the given registration.
     */
    int tailId(const QString &registration) const;

    /*!
     * \brief Returns the registration for the given tail ID.
     */
    QString registration(int tailId) const;

    const QMap<QString, int>& registrationsMap() const { return m_registrationMap; }

  private:
    QHash<QString, int> m_registrationToId;
    QHash<int, QString> m_idToRegistration;
    QMap<QString, int> m_registrationMap;


    void refresh();
};

Q_GLOBAL_STATIC(TailRegistrationsInfo, tailsData)

#endif // TAILREGISTRATIONSINFO_H
