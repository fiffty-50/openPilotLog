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
#ifndef APPROACHTYPEINFO_H
#define APPROACHTYPEINFO_H

#include <QGlobalStatic>
#include <QHash>
#include <QMap>
#include <QObject>

/*!
 * \class ApproachTypeInfo
 * \brief Provides access to approach type information from the database.
 *
 * \details
 * The ApproachTypeInfo class manages approach data using a QSqlTableModel and
 * provides lookup methods by apprach name or apprach id.
 * Access to a global instance is provided via the Q_GLOBAL_STATIC macro
 * as \c approachData:
 * \code
 * Q_GLOBAL_STATIC(ApproachTypeInfo, approachData)
 * \endcode
 */
class ApproachTypeInfo : public QObject {
    Q_OBJECT
  public:
    explicit ApproachTypeInfo(QObject *parent = nullptr);

    void init() { refreshIndices(); }

    const QMap<QString, int> &approachNamesMap() const { return m_approachNameMap; }

    /*!
     * \brief Returns the approach_id for a given approach name
     */
    int idFromName(const QString &approach_name) const;

    /*!
     * \brief Returns the approach name for a given approach_id
     */
    QString nameFromId(int approach_id) const;

  private:
    void refreshIndices();
    QMap<QString, int> m_approachNameMap;
    QHash<int, QString> m_approachTypeIdMap;

    const static inline QString SELECT_ALL = QStringLiteral("SELECT * FROM %1");
    static constexpr int COLUMN_TYPE_ID       = 0;
    static constexpr int COLUMN_APPROACH_NAME = 1;
};

Q_GLOBAL_STATIC(ApproachTypeInfo, approachData)

#endif // APPROACHTYPEINFO_H
