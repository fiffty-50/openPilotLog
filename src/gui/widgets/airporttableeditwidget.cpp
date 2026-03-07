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
#include "airporttableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/airportentryeditdialog.h"

AirportTableEditWidget::AirportTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{
}

void AirportTableEditWidget::retranslateUi()
{
    m_addNewEntryPushButton->setText(tr("Add New Airport"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Airport"));
}

QString AirportTableEditWidget::deleteErrorString(int rowId)
{
    return tr("<br>Unable to delete.<br><br>The following error has ocurred: %1")
        .arg(DB->lastErrorText());
}

QString AirportTableEditWidget::confirmDeleteString(int rowId)
{
    const auto entry = DB->getAirportEntry(rowId);
    return tr("The following airport will be deleted:<br><br><b><tt>"
              "%1<br></b></tt>"
              "Deleting airports is irreversible.<br>Do you want to proceed?")
        .arg(entry.getAirportName());
}

EntryEditDialog *AirportTableEditWidget::createEntryEditDialog()
{
    return new AirportEntryEditDialog(this);
}
