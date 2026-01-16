#include "aircrafttableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/aircraftentryeditdialog.h"

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
