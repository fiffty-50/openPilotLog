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
#include "pilottableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/opl.h"
#include <QGridLayout>

PilotTableEditWidget::PilotTableEditWidget(QWidget *parent) : TableEditWidget(Horizontal, parent) {}

void PilotTableEditWidget::setupModelAndView()
{
    TableEditWidget::setupModelAndView();

    // Hide the first entry (logbook owner)
    m_model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self
}

void PilotTableEditWidget::retranslateUi()
{
    // only need to set the table specific labels and combo box items
    m_addNewEntryPushButton->setText(tr("Add New Pilot"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Pilot"));
}

EntryEditDialog *PilotTableEditWidget::createEntryEditDialog()
{
    return new PilotEntryEditDialog(QString(), this);
}

QString PilotTableEditWidget::deleteErrorString(int pilotId)
{
    const QList<int> foreign_key_constraints =
        DB->getForeignKeyConstraints(pilotId, OPL::DbTable::Pilots);
    QList<OPL::FlightEntry> constrained_flights;
    for (const auto &row_id : foreign_key_constraints) {
        constrained_flights.append(DB->getFlightEntry(row_id));
    }

    // the error is a database error
    if (constrained_flights.isEmpty()) {
        return (tr("<br>Unable to delete.<br><br>The following error has ocurred:<br>%1")
                    .arg(DB->lastError.text()));
    }
    else {
        // the error is a foreign key constraint
        QString constrained_flights_string;
        for (int i = 0; i < constrained_flights.length(); i++) {
            constrained_flights_string.append(constrained_flights[i].getFlightSummary() +
                                              QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
            if (i > 10) {
                constrained_flights_string.append("<br>[...]<br>");
                break;
            }
        }
        return (
            tr("Unable to delete.<br><br>"
               "This is most likely the case because a flight exists with the Pilot "
               "you are trying to delete as PIC.<br><br>"
               "%1 flight(s) with this pilot have been found:<br><br><br><b><tt>"
               "%2"
               "</b></tt><br><br>You have to change or remove the conflicting flight(s) "
               "before removing this pilot from the database.<br><br>")
                .arg(QString::number(constrained_flights.length()), constrained_flights_string));
    }
}

QString PilotTableEditWidget::confirmDeleteString(int rowId)
{
    const auto entry = DB->getPilotEntry(rowId);
    return tr("You are deleting the following pilot:<br><br><b><tt>"
              "%1</b></tt><br><br>Are you sure?")
        .arg(entry.getName());
}

void PilotTableEditWidget::filterTextChanged(const QString &filterText)
{
    // overriding the base class method because the first entry (rowid == 1) is hidden in this
    // Widget
    if (filterText.isEmpty()) {
        m_model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self
        return;
    }

    auto getPilotFilterStatement = [&](const QString &filterColumn,
                                       const QString &filterText) -> QString {
        return getFilterStatement(filterColumn, filterText) + QLatin1String(" AND ") +
               OPL::PilotEntry::ROWID + QLatin1String(" > 1");
    };

    // Try to map filter combo box value to column
    const QString filterColumn =
        COLUMN_DATABASE_NAMES.value(m_filterSelectionComboBox->currentText());
    if (filterColumn.isEmpty()) {
        // search in all columns
        QString filter;
        const QString SQL_OR = QStringLiteral(" OR ");
        for (const auto &column : COLUMN_DATABASE_NAMES.values()) {
            filter.append(getPilotFilterStatement(column, filterText));
            filter.append(SQL_OR);
        }
        // remove last "or"
        filter.chop(SQL_OR.size());
        m_model->setFilter(filter);
    }
    else {
        // filter based on selected column
        m_model->setFilter(getPilotFilterStatement(filterColumn, filterText));
    }
}
