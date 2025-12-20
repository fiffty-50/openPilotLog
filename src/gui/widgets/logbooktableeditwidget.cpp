#include "logbooktableeditwidget.h"
#include "src/classes/settings.h"
#include "src/classes/styleddatedelegate.h"
#include "src/classes/styledpilotdelegate.h"
#include "src/classes/styledtimedelegate.h"
#include "src/classes/styledtypedelegate.h"
#include "src/database/database.h"
#include "src/database/views/logbookviewinfo.h"
#include "src/gui/dialogues/flightentryeditdialog.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/gui/dialogues/newsimdialog.h"

LogbookTableEditWidget::LogbookTableEditWidget(QWidget *parent)
    : TableEditWidget(Vertical, parent)
{}


// TableEditWidget implementation

void LogbookTableEditWidget::setupModelAndView()
{
    m_logbookView = Settings::getLogbookView();
    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getViewIdentifier(m_logbookView));
    m_model->select();

    const auto headers = OPL::LogbookViewInfo::getTableHeaders(m_logbookView);
    for(int i = 0; i < headers.size(); i++) {
        m_model->setHeaderData(i, Qt::Horizontal, headers[i]);
    }

    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    m_view->verticalHeader()->hide();
    m_view->setAlternatingRowColors(true);
    m_view->hideColumn(COL_ROWID);

    setupDelegates();
    m_view->resizeColumnsToContents();
}

void LogbookTableEditWidget::setupUI()
{
    TableEditWidget::setupUI();
    m_addNewEntryPushButton->setText(tr("Add new Flight"));
    m_deleteEntryPushButton->setText(tr("Delete selected Entry"));
    m_filterWidget->hide();
    m_stackedWidget->hide();

    m_format = Settings::getDisplayFormat();
}

QString LogbookTableEditWidget::deleteErrorString(int rowId)
{
    return tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
              ).arg(DB->lastError.text());
}

QString LogbookTableEditWidget::confirmDeleteString(int rowId)
{
    if(rowId > 0) {
    const auto selectedEntry = DB->getFlightEntry(rowId);
    return tr("The following flight will be deleted:<br><br><b><tt>"
               "%1<br></b></tt><br><br>"
               "Deleting flights is irreversible.<br>Do you want to proceed?"
              ).arg(selectedEntry.getFlightSummary());

    }

    return tr("Deleting entries is irreversible.<br>Do you want to proceed?");
}

EntryEditDialog *LogbookTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new NewFlightDialog(parent);
}

void LogbookTableEditWidget::filterTextChanged(const QString &filterString)
{}

void LogbookTableEditWidget::editEntryRequested(const QModelIndex &selectedIndex)
{
    showEditWidget();
    const auto idx = m_view->selectionModel()->currentIndex();
    const auto rowId = m_model->index(idx.row(), 0).data().toInt();
    if(rowId > 0) {
        //auto nfd = NewFlightDialog(rowId, this);
        auto dialog = FlightEntryEditDialog(rowId, this);
        m_stackedWidget->addWidget(&dialog);
        m_stackedWidget->setCurrentWidget(&dialog);
        dialog.exec();
    } else {
        auto nsd = NewSimDialog(rowId * -1, this);
        m_stackedWidget->addWidget(&nsd);
        m_stackedWidget->setCurrentWidget(&nsd);
        nsd.exec();
    }
    hideEditWidget();
}

void LogbookTableEditWidget::deleteEntryRequested()
{
    const QModelIndex selectedIndex = m_view->selectionModel()->currentIndex();
    if(!selectedIndex.isValid()) {
        WARN(tr("No entry selected."));
        return;
    }
    m_stackedWidget->hide();

    int rowId = m_model->index(selectedIndex.row(), 0).data().toInt();
    m_view->selectionModel()->reset();

    // get user confirmation
    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Confirm Deletion"));

    confirm.setText(confirmDeleteString(rowId));
    if (confirm.exec() == QMessageBox::Yes) {
        if(rowId > 0) {
            const auto selectedEntry = DB->getFlightEntry(rowId);
            if(!DB->remove(selectedEntry))
                WARN(deleteErrorString(rowId));
        } else {
            const auto selectedEntry = DB->getSimEntry(rowId * - 1);
            if(!DB->remove(selectedEntry))
                WARN(deleteErrorString(rowId));
        }
    }
}

// private implementations

void LogbookTableEditWidget::addSimulatorEntryRequested()
{
        showEditWidget();

        auto nsd = NewSimDialog(this);
        m_stackedWidget->addWidget(&nsd);
        m_stackedWidget->setCurrentWidget(&nsd);
        nsd.exec();

        hideEditWidget();
}

void LogbookTableEditWidget::viewSelectionChanged(SettingsWidget::SettingSignal widget)
{
    for(auto i = m_defaultDelegates.constBegin(); i != m_defaultDelegates.constEnd(); i++) {
        m_view->setItemDelegateForColumn(i.key(), i.value());
        // should probably delete the old custom delegate, Qt docs say the
        // view does not take ownership of the delegates so they might be leaking
        // https://doc.qt.io/qt-6/qabstractitemdelegate.html
    }

    if(widget == SettingsWidget::SettingSignal::LogbookWidget)
        setupModelAndView();
}

void LogbookTableEditWidget::setupDelegates()
{
    // minutes to hh:mm
    const auto timeDelegate = new StyledTimeDelegate(m_format, m_model);
    for(const auto col : OPL::LogbookViewInfo::getTimeColumns(m_logbookView)) {
        m_defaultDelegates.insert(col, m_view->itemDelegateForColumn(col));
        m_view->setItemDelegateForColumn(col, timeDelegate);
    }

    // julian day to Date Format
    const int dateCol = OPL::LogbookViewInfo::getDateColumn(m_logbookView);
    const auto dateDelegate = new StyledDateDelegate(Settings::getDisplayFormat(), m_model);
    m_defaultDelegates.insert(dateCol, m_view->itemDelegateForColumn(dateCol));
    m_view->setItemDelegateForColumn(dateCol, dateDelegate);

    // pilot_id to names
    const int pilCol = OPL::LogbookViewInfo::getPicColumn(m_logbookView);
    const auto pilotDelegate = new StyledPilotDelegate(m_model);
    m_defaultDelegates.insert(pilCol, m_view->itemDelegateForColumn(pilCol));
    m_view->setItemDelegateForColumn(pilCol, pilotDelegate);

    // tail_id to aircraft type and registration
    const int typeCol = OPL::LogbookViewInfo::getTypeColumn(m_logbookView);
    const auto typeDelegate = new StyledTypeDelegate(m_model);
    m_defaultDelegates.insert(typeCol, m_view->itemDelegateForColumn(typeCol));
    m_view->setItemDelegateForColumn(typeCol, typeDelegate);
}
