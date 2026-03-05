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
#ifndef APPROACHTYPESEDITWIDGET_H
#define APPROACHTYPESEDITWIDGET_H

#include "src/opl.h"
#include "tableeditwidget.h"
#include <QObject>

class ApproachTypesEditWidget : public TableEditWidget {
    Q_OBJECT
  public:
    ApproachTypesEditWidget() = delete;
    ApproachTypesEditWidget(QWidget *parent = nullptr);

    void retranslateUi() override;
    QString deleteErrorString(int rowId) override;
    QString confirmDeleteString(int rowId) override;
    EntryEditDialog *createEntryEditDialog() override;

  protected:
    const QList<int> *getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QList<int> *getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QMap<int, QString> *getColumnHeaderMap() const override { return &COLUMN_HEADER_MAP; }
    const QString tableName() const override
    {
        return OPL::GLOBALS->getDbTableName(OPL::DbTable::ApproachTypes);
    };

  private:
    static constexpr int COL_APPROACH_ID   = 0;
    static constexpr int COL_APPROACH_NAME = 1;

    const QString HEADER_NAME = tr("Approach");

    const QMap<int, QString> COLUMN_HEADER_MAP = {
        {COL_APPROACH_NAME, HEADER_NAME}
    };

    const QList<int> HIDDEN_COLUMNS  = {COL_APPROACH_ID};
    const QList<int> VISIBLE_COLUMNS = {COL_APPROACH_NAME};
};

#endif // APPROACHTYPESEDITWIDGET_H
