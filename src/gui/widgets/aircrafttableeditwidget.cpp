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
#include "src/database/database.h"
#include "src/gui/dialogues/aircraftentryeditdialog.h"
#include "src/classes/styledenginecountdelegate.h"

AircraftTableEditWidget::AircraftTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{}

void AircraftTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::v2AircraftTypes));
    m_model->select();

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->resizeColumnsToContents();
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    for(auto it = COLUMN_HEADERS_MAP.cbegin(); it != COLUMN_HEADERS_MAP.cend(); ++it) {
        m_model->setHeaderData(it.key(), Qt::Horizontal, it.value());
    }
    m_view->hideColumn(COL_ROWID);

    // Install a custom delegate for the engine count column
    m_view->setItemDelegateForColumn(COL_ENGINE_COUNT, new StyledEngineCountDelegate(m_model));

}

void AircraftTableEditWidget::setupUI()
{
    TableEditWidget::setupUI();
    retranslateUi();
}

void AircraftTableEditWidget::retranslateUi()
{
    m_addNewEntryPushButton->setText(tr("Add New Aircraft Type"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Aircraft Type"));
    m_filterSelectionComboBox->addItems(FILTER_COLUMNS);
}

QString AircraftTableEditWidget::deleteErrorString(int rowId)
{
    DEB << "Unimplemented";
    return {};
}

QString AircraftTableEditWidget::confirmDeleteString(int rowId)
{
    DEB << "Unimplemented";
    return {};
}

EntryEditDialog *AircraftTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new AircraftEntryEditDialog(this);
}


void AircraftTableEditWidget::filterTextChanged(const QString &filterString)
{
    DEB << "Unimplemented";
}
