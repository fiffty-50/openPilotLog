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
#include "tailtableeditwidget.h"

#include "src/database/database.h"
#include "src/gui/dialogues/tailentryeditdialog.h"

TailTableEditWidget::TailTableEditWidget(QWidget *parent) : TableEditWidget(Horizontal, parent) {}

void TailTableEditWidget::retranslateUi()
{
    m_addNewEntryPushButton->setText(tr("Add New Tail"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Tail"));
}

QString TailTableEditWidget::deleteErrorString(int rowId)
{
    return "UNIMPLEMENTED";
    // QList<int> foreign_key_constraints = DB->getForeignKeyConstraints(rowId, OPL::DbTable::v2AircraftTails);
    // QList<OPL::FlightEntry> constrained_flights;
    // for (const auto &row_id : std::as_const(foreign_key_constraints)) {
    //     constrained_flights.append(DB->getFlightEntry(row_id));
    // }

    // QMessageBox message_box(this);
    // if (constrained_flights.isEmpty()) {
    //     // error is a database error
    //     return tr("<br>Unable to delete.<br><br>The following error has "
    //               "ocurred: "
    //               "%1")
    //         .arg(DB->lastErrorText());
    // }
    // else {
    //     QString constrained_flights_string;
    //     for (int i = 0; i < constrained_flights.length(); i++) {
    //         constrained_flights_string.append(constrained_flights[i].getFlightSummary() +
    //                                           QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
    //         if (i > 10) {
    //             constrained_flights_string.append(QLatin1String("<br>[...]<br>"));
    //             break;
    //         }
    //     }
    //     return (
    //         tr("Unable to delete.<br><br>"
    //            "This is most likely the case because a flight exists with the "
    //            "aircraft "
    //            "you are trying to delete.<br><br>"
    //            "%1 flight(s) with this aircraft have been "
    //            "found:<br><br><br><b><tt>"
    //            "%2"
    //            "</b></tt><br><br>You have to change or remove the conflicting "
    //            "flight(s) "
    //            "before removing this aircraft from the database.<br><br>")
    //             .arg(QString::number(constrained_flights.length()), constrained_flights_string));
    // }
}

QString TailTableEditWidget::confirmDeleteString(int rowId)
{
    const auto tailEntry     = DB->getTailEntry(rowId);
    const QString typeString = OPL::AircraftEntry::getTypeString(tailEntry.getRowId());

    return tr("You are deleting the following aircraft:<br><br><b><tt>"
              "%1 (%2)</b></tt><br><br>Are you sure?")
        .arg(tailEntry.getRegistration(), typeString);
}

EntryEditDialog *TailTableEditWidget::createEntryEditDialog()
{
    return new TailEntryEditDialog(QString(), this);
}
