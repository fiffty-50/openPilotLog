#include "airporttableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/airportentryeditdialog.h"

AirportTableEditWidget::AirportTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{}

void AirportTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDatabaseViewName(OPL::DatabaseView::Airports));
    m_model->select();

    for(auto it = HEADER_NAMES.cbegin(); it != HEADER_NAMES.cend(); ++it) {
        m_model->setHeaderData(it.key(), Qt::Horizontal, it.value());
    }

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->resizeColumnsToContents();
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    m_view->hideColumn(COL_ROWID);

}

void AirportTableEditWidget::setupUI()
{
    // the base class does most of the setup
    TableEditWidget::setupUI();

    // only need to set the table specific labels and combo box items
    m_addNewEntryPushButton->setText(tr("Add New Airport"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Airport"));
}

QString AirportTableEditWidget::deleteErrorString(int rowId)
{
    return tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
              ).arg(DB->lastError.text());
}

QString AirportTableEditWidget::confirmDeleteString(int rowId)
{
    const auto entry = DB->getAirportEntry(rowId);
    return tr("The following airport will be deleted:<br><br><b><tt>"
              "%1<br></b></tt>"
              "Deleting airports is irreversible.<br>Do you want to proceed?"
              ).arg(entry.getAirportName());
}

EntryEditDialog *AirportTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new AirportEntryEditDialog(parent);
}
