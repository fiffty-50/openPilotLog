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
#include "approachtypeseditwidget.h"
#include "src/database/cache/approachtypeinfo.h"
#include "src/database/database.h"
#include "src/gui/dialogues/approachentryeditdialog.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/widgets/tableeditwidget.h"
#include <qabstractitemmodel.h>

ApproachTypesEditWidget::ApproachTypesEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{
}

void ApproachTypesEditWidget::retranslateUi()
{
    m_addNewEntryPushButton->setText(tr("Add New Approach Type"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Approach Type"));
}

QString ApproachTypesEditWidget::deleteErrorString(int row_id) { return DB->lastErrorText(); }

QString ApproachTypesEditWidget::confirmDeleteString(int row_id)
{
    QString approach_name = approachData->nameFromId(row_id);

    DEB << "Deleting: " << approach_name << " with row ID:" << row_id;
    return tr("You are deleting the following approach type:<br><br><b><tt>"
              "%1</b></tt><br><br>Are you sure?")
        .arg(approach_name);
}

EntryEditDialog *ApproachTypesEditWidget::createEntryEditDialog() { return new ApproachEntryEditDialog(this); }
