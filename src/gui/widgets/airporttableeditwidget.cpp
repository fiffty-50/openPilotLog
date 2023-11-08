#include "airporttableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/newairportdialog.h"

AirportTableEditWidget::AirportTableEditWidget(QWidget *parent)
    : TableEditWidget(Vertical, parent)
{}

void AirportTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Airports));
    m_model->select();

    for(int i = 0; i < HEADER_NAMES.size(); i++) {
        m_model->setHeaderData(i + 1, Qt::Horizontal, HEADER_NAMES.at(i));
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
    for(const int i : FILTER_COLUMNS) {
        m_filterSelectionComboBox->addItem(HEADER_NAMES.at(i));
    }
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
              ).arg(entry.getAirportDescriptor());
}

EntryEditDialog *AirportTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new NewAirportDialog(parent);
}

void AirportTableEditWidget::filterTextChanged(const QString &filterString)
{
    if(filterString.isEmpty()) {
        m_model->setFilter(QString());
        return;
    }

    int i = m_filterSelectionComboBox->currentIndex();
    const QString filter =
        QLatin1Char('\"')
        + HEADER_NAMES.at(FILTER_COLUMNS[i])
        + QLatin1String("\" LIKE '%")
        + filterString
        + QLatin1String("%'");
    m_model->setFilter(filter);
}
