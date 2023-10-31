#include "currencywidget.h"
#include "QtSql/qsqltablemodel.h"
#include "QtWidgets/qheaderview.h"
#include "qgridlayout.h"
#include "src/classes/easaftl.h"
#include "src/classes/styleddatedelegate.h"
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

    // warn the user about impending currencies if warning threshold > 0
    int warningThreshold = Settings::getCurrencyWarningThreshold();
    if(warningThreshold) {
        warnAboutExpiries(warningThreshold);
    }
}

void CurrencyWidget::setupModelAndView()
{
    model = new QSqlTableModel(this, DB->database());
    model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::Currencies));
    model->select();
    model->setHeaderData(EXPIRY_DATE_COLUMN, Qt::Horizontal, tr("Expiry Date"));
    model->setHeaderData(CURRENCY_NAME_COLUMN, Qt::Horizontal, tr("Name"));

    view = new QTableView(this);
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectItems);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->resizeColumnsToContents();
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(ROWID_COLUMN);

    const auto dateDelegate = new StyledDateDelegate(Settings::getDateFormat(), this);
    view->setItemDelegateForColumn(EXPIRY_DATE_COLUMN, dateDelegate);
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
    gridLayout->addWidget(view, row, colL, singleRowSpan, allColSpan);

    // set the layout active
    layout = gridLayout;

    // allocate a widget for date selection
    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setWindowFlags(Qt::Dialog); // pop-up calendar


    // connect signals
    QObject::connect(view,	&QTableView::activated,
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
    if(index.column() == EXPIRY_DATE_COLUMN) {
        expiryDateEditRequested(index);
    }

    if(index.column() == CURRENCY_NAME_COLUMN) {
        displayNameEditRequested(index);
    }
}



void CurrencyWidget::refresh()
{
    model->select();
    fillTakeOffAndLandingCurrencies();
    fillFlightTimeLimitations();
}

void CurrencyWidget::displayNameEditRequested(const QModelIndex &index)
{
    QString oldData = index.data().toString();
    bool textEdited;
    const QString text = QInputDialog::getText(
        this,
        tr("Edit Currency Name"),
        tr("Please enter a name for this currency"),
        QLineEdit::Normal,
        oldData,
        &textEdited);

    if(textEdited) {
        model->setData(index, text);
        model->submitAll();
    }
    view->resizeColumnsToContents();

}

void CurrencyWidget::expiryDateEditRequested(const QModelIndex &index)
{
    const QString selection = index.data().toString();
    const QDate selectedDate = QDate::fromString(selection, dateFormat);
    if(selectedDate.isValid()) {
        const QSignalBlocker blocker(calendar);
        calendar->setSelectedDate(selectedDate);
        calendar->show();
    } else {
        calendar->show();
    }
    // calendars date selected signal is connected to newExpiryDateSelected()
}

void CurrencyWidget::newExpiryDateSelected()
{
    calendar->hide();
    model->setData(view->selectionModel()->currentIndex(), calendar->selectedDate().toJulianDay());
    model->submitAll();
    model->select();
}

void CurrencyWidget::warnAboutExpiries(int warningThreshold)
{
    const QDate today = QDate::currentDate();

    for(int i = 0; i < model->rowCount(); i++) {
        const QModelIndex dateIndex = model->index(i, EXPIRY_DATE_COLUMN);
        QDate date = QDate::fromString(dateIndex.data().toString(), Qt::ISODate);

        if(date.addDays(warningThreshold) > today) {
            const QString dateString = date.toString(Qt::ISODate);
            const QString nameString = model->index(i, CURRENCY_NAME_COLUMN).data().toString();
            QString msg = tr("%1 expires on<br><br>%2").arg(nameString, dateString);
            WARN(msg);
        }
    }
}

QFrame *CurrencyWidget::getHorizontalLine()
{
    QFrame* newFrame = new QFrame(this);
    newFrame->setFrameShape(QFrame::HLine);
    return newFrame;
}
