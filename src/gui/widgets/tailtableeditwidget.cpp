#include "tailtableeditwidget.h"
#include "src/database/database.h"
#include "src/gui/dialogues/tailentryeditdialog.h"

TailTableEditWidget::TailTableEditWidget(QWidget *parent)
    : TableEditWidget(Horizontal, parent)
{}

void TailTableEditWidget::setupModelAndView()
{
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Tails));
    m_model->select();
    m_model->setHeaderData(COL_REGISTRATION, Qt::Horizontal, COLUMN_NAME_REGISTRATION);
    m_model->setHeaderData(COL_TYPE, Qt::Horizontal, COLUMN_NAME_TYPE);
    m_model->setHeaderData(COL_COMPANY, Qt::Horizontal, COLUMN_NAME_COMPANY);

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

void TailTableEditWidget::setupUI()
{
    // the base class does most of the setup
    TableEditWidget::setupUI();

    // only need to set the table specific labels and combo box items
    m_addNewEntryPushButton->setText(tr("Add New Tail"));
    m_deleteEntryPushButton->setText(tr("Delete Selected Tail"));
    m_filterSelectionComboBox->addItems(FILTER_COLUMNS);
}

QString TailTableEditWidget::deleteErrorString(int rowId)
{
    QList<int> foreign_key_constraints = DB->getForeignKeyConstraints(rowId,
                                                                      OPL::DbTable::Tails);
    QList<OPL::FlightEntry> constrained_flights;
    for (const auto &row_id : std::as_const(foreign_key_constraints)) {
        constrained_flights.append(DB->getFlightEntry(row_id));
    }

    QMessageBox message_box(this);
    if (constrained_flights.isEmpty()) {
        // error is a database error
        return tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                  ).arg(DB->lastError.text());
    } else {
        QString constrained_flights_string;
        for (int i=0; i<constrained_flights.length(); i++) {
            constrained_flights_string.append(constrained_flights[i].getFlightSummary()
                                              + QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
            if (i>10) {
                constrained_flights_string.append(QLatin1String("<br>[...]<br>"));
                break;
            }
        }
        return (tr("Unable to delete.<br><br>"
                   "This is most likely the case because a flight exists with the aircraft "
                   "you are trying to delete.<br><br>"
                   "%1 flight(s) with this aircraft have been found:<br><br><br><b><tt>"
                   "%2"
                   "</b></tt><br><br>You have to change or remove the conflicting flight(s) "
                   "before removing this aircraft from the database.<br><br>"
                   ).arg(
                        QString::number(constrained_flights.length()),
                        constrained_flights_string)
                );
    }
}

QString TailTableEditWidget::confirmDeleteString(int rowId)
{
    const auto entry = DB->getTailEntry(rowId);
    return tr("You are deleting the following aircraft:<br><br><b><tt>"
              "%1 (%2)</b></tt><br><br>Are you sure?"
              ).arg(
              entry.getData().value(OPL::TailEntry::REGISTRATION).toString(),
              entry.getTypeString()
              );
}

EntryEditDialog *TailTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    QString empty;
    return new TailEntryEditDialog(empty, parent);
}

void TailTableEditWidget::filterTextChanged(const QString &filterString)
{
    if(filterString.isEmpty()) {
        m_model->setFilter(QString());
        return;
    }

    int i = m_filterSelectionComboBox->currentIndex();
    const QString filter =
        QLatin1Char('\"')
        + FILTER_COLUMN_NAMES.at(i)
        + QLatin1String("\" LIKE '%")
        + filterString
        + QLatin1String("%'");
    m_model->setFilter(filter);
}
