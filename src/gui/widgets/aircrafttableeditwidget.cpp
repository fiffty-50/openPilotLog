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
#include "aircrafttableeditwidget.h"
#include "src/classes/styledenginecountdelegate.h"
#include "src/database/database.h"
#include "src/gui/dialogues/aircraftentryeditdialog.h"

AircraftTableEditWidget::AircraftTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent) {}

void AircraftTableEditWidget::setupModelAndView() {
  m_model = new QSqlTableModel(this, DB->database());
  m_model->setTable(
      OPL::GLOBALS->getDbTableName(OPL::DbTable::v2AircraftTypes));
  m_model->select();

  m_view->setModel(m_model);
  m_view->setSelectionMode(QAbstractItemView::SingleSelection);
  m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
  m_view->resizeColumnsToContents();
  m_view->verticalHeader()->hide();
  m_view->setAlternatingRowColors(true);
  for (auto it = COLUMN_HEADERS_MAP.cbegin(); it != COLUMN_HEADERS_MAP.cend();
       ++it) {
    m_model->setHeaderData(it.key(), Qt::Horizontal, it.value());
  }
  for (const auto &col : HIDDEN_COLUMNS) {
    m_view->hideColumn(col);
  }

  // Install a custom delegate for the engine count column
  m_view->setItemDelegateForColumn(COL_IS_MULTI_ENGINE,
                                   new StyledEngineCountDelegate(m_model));
}

void AircraftTableEditWidget::setupUI() {
  TableEditWidget::setupUI();
  retranslateUi();
}

void AircraftTableEditWidget::retranslateUi() {
  m_addNewEntryPushButton->setText(tr("Add New Aircraft Type"));
  m_deleteEntryPushButton->setText(tr("Delete Selected Aircraft Type"));
}

QString AircraftTableEditWidget::deleteErrorString(int rowId) {
  const auto toDelete = DB->getAircraftEntry(rowId);
  const auto foreign_key_constraints =
      DB->getForeignKeyConstraints(rowId, OPL::DbTable::v2AircraftTypes);
  if (!foreign_key_constraints.isEmpty()) {
    QList<OPL::TailEntry> constrained_tails;
    for (const auto &fk_row_id : foreign_key_constraints) {
      constrained_tails.append(DB->getTailEntry(fk_row_id));
    }
    QString error_string = tr("Cannot delete aircraft type <b>'%1'</b> because "
                              "it is used by the following tail(s):<br>")
                               .arg(OPL::AircraftEntry::getTypeString(rowId));
    for (const auto &tail : constrained_tails) {
      error_string.append(
          QStringLiteral("<br> - %1").arg(tail.getRegistration()));
    }
    return error_string;
  } else {
    return tr("Unable to delete aircraft type '%1'. The following error has "
              "occurred.<br><br>%2")
        .arg(OPL::AircraftEntry::getTypeString(rowId), DB->lastError.text());
  }
}

QString AircraftTableEditWidget::confirmDeleteString(int rowId) {
  const auto toDelete = DB->getAircraftEntry(rowId);
  DEB << "Deleting: " << toDelete << " with row ID:" << rowId;
  return tr("You are deleting the following aircraft type:<br><br><b><tt>"
            "%1</b></tt><br><br>Are you sure?")
      .arg(toDelete.getTypeString(rowId));
}

EntryEditDialog *AircraftTableEditWidget::getEntryEditDialog(QWidget *parent) {
  return new AircraftEntryEditDialog(this);
}