#include "defaultlogbookview.h"
#include "QtWidgets/qheaderview.h"
#include "src/classes/styleddatedelegate.h"
#include "src/classes/styledpilotdelegate.h"
#include "src/classes/styledregistrationdelegate.h"
#include "src/classes/styledtimedelegate.h"
#include "src/database/database.h"
#include "src/classes/settings.h"

DefaultLogbookView::DefaultLogbookView(QWidget *parent)
    : QTableView(parent)
{
    auto model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Flights));
    model->select();

    model->setHeaderData(COL_DATE, Qt::Horizontal, tr("Date of Flight"));
    model->setHeaderData(COL_DEPT, Qt::Horizontal, tr("Dept"));
    model->setHeaderData(COL_DEST, Qt::Horizontal, tr("Dest"));
    model->setHeaderData(COL_TOFB, Qt::Horizontal, tr("Time"));
    model->setHeaderData(COL_TONB, Qt::Horizontal, tr("Time"));
    model->setHeaderData(COL_PIC, Qt::Horizontal, tr("Name PIC"));
    model->setHeaderData(COL_ACFT, Qt::Horizontal, tr("Registration"));
    model->setHeaderData(COL_FLIGHT_NR, Qt::Horizontal, tr("Flight #"));
    model->setHeaderData(COL_REMARKS, Qt::Horizontal, tr("Remarks"));

    // set the item delegates for converting db values to human readable formatting
    const auto dateDelegate = new StyledDateDelegate(Settings::getDateFormat(), this);
    setItemDelegateForColumn(COL_DATE, dateDelegate);

    const auto timeDelegate = new StyledTimeDelegate(this);
    setItemDelegateForColumn(COL_TOFB, timeDelegate);
    setItemDelegateForColumn(COL_TONB, timeDelegate);
    setItemDelegateForColumn(COL_TBLK, timeDelegate);

    const auto pilotDelegate = new StyledPilotDelegate(this);
    setItemDelegateForColumn(COL_PIC, pilotDelegate);

    const auto registrationDelegate = new StyledRegistrationDelegate(this);
    setItemDelegateForColumn(COL_ACFT, registrationDelegate);


    setModel(model);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setContextMenuPolicy(Qt::CustomContextMenu);
    resizeColumnsToContents();
    horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    verticalHeader()->hide();
    setAlternatingRowColors(true);

    for(int i = 0; i < COLUMN_COUNT; i++) {
        hideColumn(i);
    }
    showColumn(COL_DATE);
    showColumn(COL_ACFT);
    showColumn(COL_PIC);
    showColumn(COL_TONB);
    showColumn(COL_DEPT);
    showColumn(COL_DEST);
    showColumn(COL_TOFB);
    showColumn(COL_FLIGHT_NR);
    showColumn(COL_REMARKS);
}
