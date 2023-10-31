#include "pilottableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/opl.h"
#include <QGridLayout>

PilotTableEditWidget::PilotTableEditWidget(QWidget *parent)
    : TableEditWidget(parent)
{}

void PilotTableEditWidget::setupModelAndView()
{
    model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Pilots));
    model->select();
    model->setHeaderData(COL_LASTNAME, Qt::Horizontal, tr("Last Name"));
    model->setHeaderData(COL_FIRSTNAME, Qt::Horizontal, tr("First Name"));
    model->setHeaderData(COL_COMPANY, Qt::Horizontal, tr("Company"));
    model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self

    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->resizeColumnsToContents();
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    for(const int i : COLS_TO_HIDE)
        view->hideColumn(i);

}

void PilotTableEditWidget::setupUI()
{
    TableEditWidget::setupUI();
    addNewEntryPushButton->setText(tr("Add New Pilot"));
    deleteEntryPushButton->setText(tr("Delete Selected Pilot"));
    filterSelectionComboBox->addItems({""});
}

EntryEditDialog *PilotTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new NewPilotDialog(parent);
}

QString PilotTableEditWidget::deleteErrorString(int pilotId)
{
    const QList<int> foreign_key_constraints = DB->getForeignKeyConstraints(pilotId,
                                                                            OPL::DbTable::Pilots);
    QList<OPL::FlightEntry> constrained_flights;
    for (const auto &row_id : foreign_key_constraints) {
        constrained_flights.append(DB->getFlightEntry(row_id));
    }

    // the error is a database error
    if (constrained_flights.isEmpty()) {
        return(tr("<br>Unable to delete.<br><br>The following error has ocurred:<br>%1"
                ).arg(DB->lastError.text()));
    } else {
        // the error is a foreign key constraint
        QString constrained_flights_string;
        for (int i=0; i<constrained_flights.length(); i++) {
            constrained_flights_string.append(constrained_flights[i].getFlightSummary() + QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
            if (i>10) {
                constrained_flights_string.append("<br>[...]<br>");
                break;
            }
        }
        return(tr("Unable to delete.<br><br>"
                "This is most likely the case because a flight exists with the Pilot "
                "you are trying to delete as PIC.<br><br>"
                "%1 flight(s) with this pilot have been found:<br><br><br><b><tt>"
                "%2"
                "</b></tt><br><br>You have to change or remove the conflicting flight(s) "
                "before removing this pilot from the database.<br><br>"
                ).arg(QString::number(constrained_flights.length()),
                      constrained_flights_string));
    }
}

QString PilotTableEditWidget::confirmDeleteString(int rowId)
{
    const auto entry = DB->getPilotEntry(rowId);
    return tr("You are deleting the following pilot:<br><br><b><tt>"
           "%1, %2</b></tt><br><br>Are you sure?").arg(entry.getLastName(), entry.getFirstName());
}


