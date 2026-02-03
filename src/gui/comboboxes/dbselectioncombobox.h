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
#ifndef DBSELECTIONCOMBOBOX_H
#define DBSELECTIONCOMBOBOX_H

#include "src/opl.h"
#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include <QObject>
#include <QSqlRecord>
#include <QStringListModel>

/*!
 * \brief The DbSelectionComboBox class extends QComboBox with logic for selecting values from the
 * database.
 * \details This combo box uses a QSqlQueryModel <value, row_id> where the data value of each
 * user-facing string contains the row_id of the associated database entry. It also includes a
 * DiacriticIgnoringCompleter to enable searching the available values with or without a '-'
 * character.
 *
 * When an entry is made that is not contained in the database, newValueEntered is emitted.
 *
 * The DbSelectionComboBox is connected to the OPL::Database::databaseUpdated signal and refreshes
 * itself when a database change is signalled.
 */
class DbSelectionComboBox : public QComboBox {
    Q_OBJECT
  public:
    DbSelectionComboBox(OPL::DbTable table, QWidget *parent = nullptr);
    OPL::DbTable table() const { return m_table; }

  signals:
    /*!
     *\brief When the user enters a value not contained in the model newValueEntered is
     * emitted.
     */
    void newValueEntered(DbSelectionComboBox *caller);

  private slots:
    void on_editingFinished();

  private:
    void connectSlots();
    bool completionIsAvailable();

    // value -> row_id
    QMap<QString, int> m_map;

  protected:
    virtual QString getQuery() const = 0;
    void refresh();
    OPL::DbTable m_table;
};

#endif // DBSELECTIONCOMBOBOX_H
