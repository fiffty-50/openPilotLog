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
#ifndef PILOTTABLEEDITWIDGET_H
#define PILOTTABLEEDITWIDGET_H

#include "src/database/pilotentry.h"
#include "tableeditwidget.h"

class PilotTableEditWidget : public TableEditWidget {
    Q_OBJECT
  public:
    PilotTableEditWidget(QWidget *parent = nullptr);

    virtual void setupModelAndView() override;
    virtual void retranslateUi() override;
    virtual EntryEditDialog *createEntryEditDialog() override;

  private:
    static constexpr int COL_ROWID       = 0;
    static constexpr int COL_NAME        = 1;
    static constexpr int COL_ALIAS       = 2;
    static constexpr int COL_EMPLOYEE_ID = 3;
    static constexpr int COL_COMPANY     = 4;
    const QList<int> HIDDEN_COLUMNS      = {0, 5, 6, 7};
    const QList<int> VISIBLE_COLUMNS     = {1, 2, 3, 4};

    const QString COL_HEADER_NAME        = tr("Name");
    const QString COL_HEADER_ALIAS       = tr("Alias");
    const QString COL_HEADER_COMPANY     = tr("Company");
    const QString COL_HEADER_EMPLOYEE_ID = tr("Employee ID");

    const QMap<int, QString> DISPLAY_COLUMNS = {
        {COL_NAME,        COL_HEADER_NAME       },
        {COL_ALIAS,       COL_HEADER_ALIAS      },
        {COL_COMPANY,     COL_HEADER_COMPANY    },
        {COL_EMPLOYEE_ID, COL_HEADER_EMPLOYEE_ID},
    };

    const QMap<QString, QString> COLUMN_DATABASE_NAMES = {
        {COL_HEADER_NAME,        OPL::PilotEntry::NAME      },
        {COL_HEADER_ALIAS,       OPL::PilotEntry::ALIAS     },
        {COL_HEADER_COMPANY,     OPL::PilotEntry::COMPANY   },
        {COL_HEADER_EMPLOYEE_ID, OPL::PilotEntry::EMPLOYEEID},
    };

    const QMap<int, QString> *getColumnHeaderMap() const override { return &DISPLAY_COLUMNS; }
    const QList<int> *getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QList<int> *getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QString tableName() const override
    {
        return OPL::GLOBALS->getDbTableName(OPL::DbTable::Pilots);
    }

    /*!
     * \brief Informs the user that deleting a Pilot has been unsuccessful
     *
     * \details Normally, when one of these entries can not be deleted, it is because of
     * a [foreign key constraint](https://sqlite.org/foreignkeys.html), meaning that a flight
     * is associated with the Pilot that was supposed to be deleted as Pilot-in-command.
     *
     * This function is used to inform the user and give hints on how to solve the problem.
     */
    virtual QString deleteErrorString(int pilotId) override;

    virtual QString confirmDeleteString(int rowId) override;

  private slots:
    virtual void filterTextChanged(const QString &filterText) override;
};

#endif // PILOTTABLEEDITWIDGET_H
