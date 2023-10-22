#include "currencywidget.h"
#include "QtSql/qsqltablemodel.h"
#include "QtWidgets/qgridlayout.h"
#include "QtWidgets/qheaderview.h"
#include "src/classes/easaftl.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/functions/statistics.h"
#include "src/classes/settings.h"
#include <QCalendarWidget>
#include <QInputDialog>
#include <QLabel>


CurrencyWidget::CurrencyWidget(QWidget *parent)
    : QWidget{parent}
{
    dateFormat = QStringLiteral("yyyy-MM-dd"); // TODO implement date formats
    setupModelAndView();
    setupUI();

    fillTakeOffAndLandingCurrencies();
    fillFlightTimeLimitations();
}

void CurrencyWidget::setupModelAndView()
{
    model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Currencies));
    model->select();
    model->setHeaderData(2, Qt::Horizontal, tr("Expiry Date"));
    model->setHeaderData(3, Qt::Horizontal, tr("Name"));

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
    int singleRowSpan = 1; // span only a single row
    int row = 0;
    auto gridLayout = new QGridLayout(this);

    // Take-off and Landing Currency
    gridLayout->addWidget(takeOffLandingHeaderLabel, row, colM, Qt::AlignCenter);
    row++;
    gridLayout->addWidget(getHorizontalLine(), row, colL, singleRowSpan, allColSpan);
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
    gridLayout->addWidget(getHorizontalLine(), row, colL, singleRowSpan, allColSpan);
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
    gridLayout->addWidget(getHorizontalLine(), row, colL, singleRowSpan, allColSpan);
    row++;
    gridLayout->addWidget(tableView, row, colL, singleRowSpan, allColSpan);

    // set the layout active
    layout = gridLayout;

    // allocate a widget for date selection
    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setWindowFlags(Qt::Dialog); // pop-up calendar


    // connect signals
    QObject::connect(tableView,	&QTableView::activated,
                     this, &CurrencyWidget::editRequested);
    QObject::connect(calendar, &QCalendarWidget::selectionChanged,
                     this, &CurrencyWidget::newExpiryDateSelected);
}

void CurrencyWidget::fillTakeOffAndLandingCurrencies()
{
    const auto takeoff_landings = OPL::Statistics::countTakeOffLanding();
    if(takeoff_landings.isEmpty() || takeoff_landings.size() != 2)
        return;

    QList<QLabel*> displayLabels = {
        takeOffCountDisplayLabel,
        landingCountDisplayLabel
    };

    for(int i = 0; i < 2; i++) {
        int count = takeoff_landings[i].toInt();
        if(count < 3)
            setLabelColour(displayLabels[i], Colour::Red);
        displayLabels[i]->setText(displayLabels[i]->text().arg(count));
    }
    QDate expiration_date = OPL::Statistics::currencyTakeOffLandingExpiry();
    if (expiration_date <= QDate::currentDate())
        setLabelColour(takeOffLandingExpiryDisplayLabel, Colour::Red);
    takeOffLandingExpiryDisplayLabel->setText(expiration_date.toString(Qt::TextDate));
}

void CurrencyWidget::fillFlightTimeLimitations()
{
    const QList<QPair<QLabel *, OPL::Statistics::TimeFrame>> limits =
        {
        { flightTime28DaysDisplayLabel, 		 OPL::Statistics::TimeFrame::Rolling28Days },
        { flightTime365DaysDisplayLabel, 	 	 OPL::Statistics::TimeFrame::Rolling12Months },
        { flightTimeCalendarYearDisplayLabel,    OPL::Statistics::TimeFrame::CalendarYear },
        };

    double ftlWarningThreshold = Settings::getFtlWarningThreshold();
    for (const auto &pair : limits) {
        int accruedMinutes = OPL::Statistics::totalTime(pair.second);
        int limitMinutes = EasaFTL::getLimit(pair.second);
        pair.first->setText(OPL::Time(accruedMinutes).toString());


        if (accruedMinutes >= limitMinutes)
            setLabelColour(pair.first, Colour::Red);
        else if (accruedMinutes >= limitMinutes * ftlWarningThreshold)
            setLabelColour(pair.first, Colour::Orange);
    }
}

void CurrencyWidget::editRequested(const QModelIndex &index)
{
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

void CurrencyWidget::refresh()
{
    model->select();
    fillTakeOffAndLandingCurrencies();
    fillFlightTimeLimitations();
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
