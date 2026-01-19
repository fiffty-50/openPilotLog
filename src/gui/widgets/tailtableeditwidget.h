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
#ifndef TAILTABLEEDITWIDGET_H
#define TAILTABLEEDITWIDGET_H

#include "src/database/tailentry.h"
#include "tableeditwidget.h"

class TailTableEditWidget : public TableEditWidget {
    Q_OBJECT
  public:
    TailTableEditWidget() = delete;
    explicit TailTableEditWidget(QWidget *parent = nullptr);

    void retranslateUi() override;
    QString deleteErrorString(int rowId) override;
    QString confirmDeleteString(int rowId) override;
    EntryEditDialog *createEntryEditDialog() override;

  private:
    static constexpr int COL_ROWID        = 0;
    static constexpr int COL_REGISTRATION = 1;
    static constexpr int COL_TYPE         = 2;
    static constexpr int COL_COMPANY      = 3;

    const QString COLUMN_NAME_REGISTRATION = tr("Registration");
    const QString COLUMN_NAME_TYPE         = tr("Type");
    const QString COLUMN_NAME_COMPANY      = tr("Company");

    const QMap<int, QString> COLUMN_HEADERS_MAP = {
        {COL_REGISTRATION, COLUMN_NAME_REGISTRATION},
        {COL_TYPE,         COLUMN_NAME_TYPE        },
        {COL_COMPANY,      COLUMN_NAME_COMPANY     },
    };

    const QList<int> HIDDEN_COLUMNS = {
        COL_ROWID,
    };

    const QList<int> VISIBLE_COLUMNS = {
        COL_REGISTRATION,
        COL_TYPE,
        COL_COMPANY,
    };

    const QList<int> *getHiddenColumns() const override { return &HIDDEN_COLUMNS; }
    const QList<int> *getVisibleColumns() const override { return &VISIBLE_COLUMNS; }
    const QMap<int, QString> *getColumnHeaderMap() const override { return &COLUMN_HEADERS_MAP; }
    const QString tableName() const override
    {
        return OPL::GLOBALS->getDatabaseViewName(OPL::DatabaseView::Tails);
    }
};

#endif // TAILTABLEEDITWIDGET_H
