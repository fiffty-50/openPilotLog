#include "pilottableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/opl.h"
#include <QGridLayout>

PilotTableEditWidget::PilotTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{}

void PilotTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Pilots));
    m_model->select();
    m_model->setHeaderData(COL_LASTNAME, Qt::Horizontal, tr("Last Name"));
    m_model->setHeaderData(COL_FIRSTNAME, Qt::Horizontal, tr("First Name"));
    m_model->setHeaderData(COL_COMPANY, Qt::Horizontal, tr("Company"));
    m_model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->resizeColumnsToContents();
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    for(const int i : COLS_TO_HIDE)
        m_view->hideColumn(i);

}

void PilotTableEditWidget::setupUI()
{
    // the base class does most of the setup
    TableEditWidget::setupUI();

    // only need to set the table specific labels and combo box items
    m_addNewEntryPushButton->setText(tr("Add New Pilot"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Pilot"));
    m_filterSelectionComboBox->addItems(FILTER_COLUMNS);
}

EntryEditDialog *PilotTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new NewPilotDialog(QString(), parent);
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

void PilotTableEditWidget::filterTextChanged(const QString &filterText)
{
    if(filterText.isEmpty()) {
        m_model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self
        return;
    }

    int i = m_filterSelectionComboBox->currentIndex();
    const QString filter =
        QLatin1Char('\"')
        + FILTER_COLUMN_NAMES.at(i)
        + QLatin1String("\" LIKE '%")
        + filterText
        + QLatin1String("%' AND ")
        + OPL::PilotEntry::ROWID
        + QLatin1String(" > 1");
    m_model->setFilter(filter);
}


