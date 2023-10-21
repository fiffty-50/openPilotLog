#include "currencywidget.h"
#include "QtSql/qsqltablemodel.h"
#include "QtWidgets/qgridlayout.h"
#include "QtWidgets/qheaderview.h"
#include "src/database/database.h"
#include <QCalendarWidget>
#include <QInputDialog>
#include <QLabel>


CurrencyWidget::CurrencyWidget(QWidget *parent)
    : QWidget{parent}
{
    dateFormat = QStringLiteral("yyyy-MM-dd"); // TODO implement date formats
    setupModelAndView();
    setupUI();
}

void CurrencyWidget::setupModelAndView()
{
    model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Currencies));
    model->select();
    model->setHeaderData(2, Qt::Horizontal, "Expiry Date");
    model->setHeaderData(3, Qt::Horizontal, "Name");

    tableView = new QTableView(this);
    tableView->setModel(model);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    tableView->verticalHeader()->hide();
    tableView->setAlternatingRowColors(true);
    tableView->hideColumn(0);
    tableView->hideColumn(1); // TODO remove once sql table is adjusted
}

void CurrencyWidget::setupUI()
{
    // create a 3-column grid layout
    int colL = 0; // left column
    int colM = 1; // middle column
    int colR = 2; // right column
    int allColSpan = 3; // span all columns
    int noRowSpan = 1; // span only a single row
    int row = 0;
    auto gridLayout = new QGridLayout(this);

    // Take-off and Landing Currency
    gridLayout->addWidget(takeOffLandingHeaderLabel, row, colM, Qt::AlignCenter);
    row++;
    gridLayout->addWidget(getHorizontalLine(), row, colL, noRowSpan, allColSpan);
    row++;

    gridLayout->addWidget(takeOffCountLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(takeOffCountDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    gridLayout->addWidget(landingCountLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(landingCountDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    gridLayout->addWidget(takeOffLandingExpiryLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(takeOffLandingExpiryDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    // Flight Time Limitations
    gridLayout->addWidget(flightTimeHeaderLabel, row, colM, Qt::AlignCenter);
    row++;
    gridLayout->addWidget(getHorizontalLine(), row, colL, noRowSpan, allColSpan);
    row++;

    gridLayout->addWidget(flightTime28DaysLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(flightTime28DaysDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    gridLayout->addWidget(flightTime365DaysLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(flightTime365DaysDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    gridLayout->addWidget(flightTimeCalendarYearLabel, row, colL, Qt::AlignLeft);
    gridLayout->addWidget(flightTimeCalendarYearDisplayLabel, row, colR, Qt::AlignRight);
    row++;

    // Expiries (currencies table)
    gridLayout->addWidget(expiriesHeaderLabel, row, colM, Qt::AlignCenter);
    row++;
    gridLayout->addWidget(getHorizontalLine(), row, colL, noRowSpan, allColSpan);
    row++;
    gridLayout->addWidget(tableView, row, colL, noRowSpan, allColSpan);

    // set the layout active
    layout = gridLayout;

    // allocate a widget for date selection
    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setWindowFlags(Qt::Dialog);


    // connect signals
    QObject::connect(tableView,	&QTableView::activated,
                     this, &CurrencyWidget::editRequested);
    QObject::connect(calendar, &QCalendarWidget::selectionChanged,
                     this, &CurrencyWidget::newExpiryDateSelected);
}



void CurrencyWidget::editRequested(const QModelIndex &index)
{
    LOG << "Edit requested at: " << index.data();
    lastSelection = index;
    const QString selection = index.data().toString();
    const QDate selectedDate = QDate::fromString(selection, dateFormat);
    if(selectedDate.isValid()) {
        // the date column has been selected for editing
        const QSignalBlocker blocker(calendar);
        calendar->setSelectedDate(selectedDate);
        calendar->show();
    } else {
        // the displayName column has been selected for editing
        LOG << "Other edit requested";
        displayNameEditRequested(index);
    }
}

void CurrencyWidget::newExpiryDateSelected()
{
    calendar->hide();
    const QString selectedDate = calendar->selectedDate().toString(dateFormat);
    model->setData(lastSelection, selectedDate);
    model->submitAll();
}

void CurrencyWidget::displayNameEditRequested(QModelIndex index)
{
    const QString text = QInputDialog::getText(
        this,
        tr("Edit Currency Name"),
        tr("Please enter a name for this currency"),
        QLineEdit::Normal,
        index.data().toString());

    model->setData(index, text);
    model->submitAll();
}

QFrame *CurrencyWidget::getHorizontalLine()
{
    QFrame* newFrame = new QFrame(this);
    newFrame->setFrameShape(QFrame::HLine);
    return newFrame;
}
