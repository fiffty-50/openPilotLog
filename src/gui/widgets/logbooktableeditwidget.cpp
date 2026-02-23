#include "logbooktableeditwidget.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/gui/views/logbookviewinfo.h"
#include "src/gui/dialogues/flightlogentryeditdialog.h"
#include "src/gui/dialogues/simentryeditdialog.h"

LogbookTableEditWidget::LogbookTableEditWidget(QWidget *parent) : TableEditWidget(Vertical, parent)
{
}

// TableEditWidget implementation

void LogbookTableEditWidget::setupModelAndView()
{
    m_logbookView = Settings::getLogbookView();
    LOG << "Loading Logbook View: " << OPL::GLOBALS->getLogbookViewName(m_logbookView);

    m_model = new QSqlTableModel(this, DB->database());
    m_model->setTable(OPL::GLOBALS->getLogbookViewName(m_logbookView));
    m_model->select();
    m_view->setModel(m_model);

    LogbookView::setupView(m_logbookView, m_model, m_view);
}

void LogbookTableEditWidget::setupUI()
{
    DEB << "setup ui";
    TableEditWidget::setupUI();
    m_addNewEntryPushButton->setText(tr("Add new Flight"));
    m_deleteEntryPushButton->setText(tr("Delete selected Entry"));
    m_filterWidget->hide();
    m_stackedWidget->hide();

    m_format = OPL::DateTimeFormat();
}

QString LogbookTableEditWidget::deleteErrorString(int rowId)
{
    return tr("<br>Unable to delete.<br><br>The following error has ocurred: %1")
        .arg(DB->lastError.text());
}

QString LogbookTableEditWidget::confirmDeleteString(int rowId)
{
    return "UNIMPLEMENTED";
    // if (rowId > 0) {
    //     const auto selectedEntry = DB->getFlightEntry(rowId);
    //     return tr("The following flight will be deleted:<br><br><b><tt>"
    //               "%1<br></b></tt><br><br>"
    //               "Deleting flights is irreversible.<br>Do you want to proceed?")
    //         .arg(selectedEntry.getFlightSummary());
    // }

    // return tr("Deleting entries is irreversible.<br>Do you want to proceed?");
}

EntryEditDialog *LogbookTableEditWidget::createEntryEditDialog()
{
    return new OPL::FlightLogEntryEditDialog(this);
}

void LogbookTableEditWidget::filterTextChanged(const QString &filterString) {}

void LogbookTableEditWidget::deleteEntryRequested()
{
    const QModelIndex selectedIndex = m_view->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
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
        DEB << "UNIMPLEMENTED.";
        // if (rowId > 0) {
        //     const auto selectedEntry = DB->getFlightEntry(rowId);
        //     if (!DB->remove(selectedEntry)) WARN(deleteErrorString(rowId));
        // }
        // else {
        //     // const auto selectedEntry = DB->getSimEntry(rowId * -1);
        //     // if (!DB->remove(selectedEntry)) WARN(deleteErrorString(rowId));
        // }
    }
}

// private implementations

void LogbookTableEditWidget::addSimulatorEntryRequested()
{
    showEditWidget();

    auto nsd = SimEntryEditDialog(this);
    m_stackedWidget->addWidget(&nsd);
    m_stackedWidget->setCurrentWidget(&nsd);
    nsd.exec();

    hideEditWidget();
}

void LogbookTableEditWidget::viewSelectionChanged(SettingsWidget::SettingSignal widget)
{
    if (widget == SettingsWidget::SettingSignal::LogbookWidget) setupModelAndView();
}
