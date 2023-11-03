#include "airporttableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/newairportdialog.h"

AirportTableEditWidget::AirportTableEditWidget(QWidget *parent)
    : TableEditWidget{parent}
{}

void AirportTableEditWidget::setupModelAndView()
{
    model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Airports));
    model->select();

    for(int i = 0; i < HEADER_NAMES.size(); i++) {
        model->setHeaderData(i + 1, Qt::Horizontal, HEADER_NAMES.at(i));
    }

    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->resizeColumnsToContents();
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(COL_ROWID);

}

void AirportTableEditWidget::setupUI()
{
    // the base class does most of the setup
    TableEditWidget::setupUI();

    // only need to set the table specific labels and combo box items
    addNewEntryPushButton->setText(tr("Add New Airport"));
    deleteEntryPushButton->setText(tr("Delete Selected Airport"));
    for(const int i : FILTER_COLUMNS) {
        filterSelectionComboBox->addItem(HEADER_NAMES.at(i));
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
        return;
    }

    int i = filterSelectionComboBox->currentIndex();
    const QString filter =
        QLatin1Char('\"')
        + HEADER_NAMES.at(FILTER_COLUMNS[i])
        + QLatin1String("\" LIKE '%")
        + filterString
        + QLatin1String("%'");
    model->setFilter(filter);
}
