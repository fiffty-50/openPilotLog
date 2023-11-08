#include "logbooktableeditwidget.h"
#include "src/classes/settings.h"
#include "src/classes/styleddatedelegate.h"
#include "src/classes/styledpilotdelegate.h"
#include "src/classes/styledregistrationdelegate.h"
#include "src/classes/styledtimedelegate.h"
#include "src/classes/styledtypedelegate.h"
#include "src/database/database.h"
#include "src/database/views/logbookviewinfo.h"
#include "src/gui/dialogues/newflightdialog.h"

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
    m_deleteEntryPushButton->setText(tr("Delete selected Flight"));
    TODO << "Set up filters";
}

QString LogbookTableEditWidget::deleteErrorString(int rowId)
{
    return "Not implemented";
}

QString LogbookTableEditWidget::confirmDeleteString(int rowId)
{
    return "Not implemented";
}

EntryEditDialog *LogbookTableEditWidget::getEntryEditDialog(QWidget *parent)
{
    return new NewFlightDialog(parent);
}

void LogbookTableEditWidget::filterTextChanged(const QString &filterString)
{
    LOG << "Not implemented.";
}

// private implementations

void LogbookTableEditWidget::setupDelegates()
{
    // minutes to hh:mm
    const auto timeDelegate = new StyledTimeDelegate(m_model);
    m_view->setItemDelegateForColumn(-1, timeDelegate); // shut up ctidy
    for(const auto col : OPL::LogbookViewInfo::getTimeColumns(m_logbookView)) {
        m_view->setItemDelegateForColumn(col, timeDelegate);
    }

    // julian day to Date Format
    const auto dateDelegate = new StyledDateDelegate(Settings::getDateFormat(), m_model);
    m_view->setItemDelegateForColumn(OPL::LogbookViewInfo::getDateColumn(m_logbookView), dateDelegate);

    // pilot_id to names
    const auto pilotDelegate = new StyledPilotDelegate(m_model);
    m_view->setItemDelegateForColumn(OPL::LogbookViewInfo::getPicColumn(m_logbookView), pilotDelegate);

    // tail_id to aircraft type and registration
    const auto typeDelegate = new StyledTypeDelegate(m_model);
    m_view->setItemDelegateForColumn(OPL::LogbookViewInfo::getTypeColumn(m_logbookView), typeDelegate);

}
