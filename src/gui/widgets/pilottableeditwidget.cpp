#include "pilottableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/opl.h"
#include <QGridLayout>

PilotTableEditWidget::PilotTableEditWidget(QWidget *parent) : TableEditWidget(Horizontal, parent) {}

void PilotTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::v2Pilots));
    m_model->select();

    for (auto it = DISPLAY_COLUMNS.cbegin(); it != DISPLAY_COLUMNS.cend(); ++it) {
        m_model->setHeaderData(it.key(), Qt::Horizontal, it.value());
    }
    m_model->setFilter(QStringLiteral("%1 > 1").arg(OPL::PilotEntry::ROWID)); // hide self

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->resizeColumnsToContents();
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    for (const auto &i : HIDDEN_COLUMNS)
        m_view->hideColumn(i);
}

void PilotTableEditWidget::setupUI()
{
    // the base class does most of the setup
    TableEditWidget::setupUI();

    // only need to set the table specific labels and combo box items
    m_addNewEntryPushButton->setText(tr("Add New Pilot"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Pilot"));
}

EntryEditDialog *PilotTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new PilotEntryEditDialog(QString(), parent);
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
