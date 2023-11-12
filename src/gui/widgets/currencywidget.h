#ifndef CURRENCYWIDGET_H
#define CURRENCYWIDGET_H

#include "src/opl.h"
#include <QWidget>
#include <QCalendarWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QLabel>
class CurrencyWidget : public QWidget
{
    Q_OBJECT
    QLayout* layout;
    QTableView *view;
    QSqlTableModel *model;
    QCalendarWidget *calendar;
    QModelIndex lastSelection;
    OPL::DateTimeFormat m_format;

    int ROWID_COLUMN = 0;
    int CURRENCY_NAME_COLUMN = 1;
    int EXPIRY_DATE_COLUMN = 2;

    void setupModelAndView();
    void setupUI();
    void fillTakeOffAndLandingCurrencies();
    void fillFlightTimeLimitations();
    void warnAboutExpiries();

    void displayNameEditRequested(const QModelIndex &index);
    void expiryDateEditRequested(const QModelIndex &index);

    QFrame *getHorizontalLine();
    QLabel *takeOffLandingHeaderLabel   		= new QLabel(tr("<b>Take-off and Landing Currency<\b>"), this);
    QLabel *takeOffCountLabel					= new QLabel(tr("Take offs (last 90 days)"), this);
    QLabel *takeOffCountDisplayLabel 			= new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *landingCountLabel 					= new QLabel(tr("Landings (last 90 days)"), this);
    QLabel *landingCountDisplayLabel			= new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *takeOffLandingExpiryLabel   		= new QLabel(tr("3 Take-offs and Landings expiry date"), this);
    QLabel *takeOffLandingExpiryDisplayLabel    = new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *flightTimeHeaderLabel		   		= new QLabel(tr("<b>Flight Time Limitations</b>"), this);
    QLabel *flightTime28DaysLabel 	    		= new QLabel(tr("Flight time (last 28 days)"), this);
    QLabel *flightTime28DaysDisplayLabel 	    = new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *flightTime365DaysLabel 	    		= new QLabel(tr("FLight time (last 365 days)"), this);
    QLabel *flightTime365DaysDisplayLabel 	    = new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *flightTimeCalendarYearLabel 		= new QLabel(tr("Flight time (this calendar year"),this);
    QLabel *flightTimeCalendarYearDisplayLabel  = new QLabel(QStringLiteral("<b>%1</b>"), this);
    QLabel *expiriesHeaderLabel			   		= new QLabel(tr("<b>Expiries<\b>"), this);

    enum class Colour {Red, Orange, None};
    inline void setLabelColour(QLabel* label, Colour colour)
    {
        switch (colour) {
        case Colour::None:
            label->setStyleSheet(QString());
            break;
        case Colour::Red:
            label->setStyleSheet(QStringLiteral("color: red"));
            break;
        case Colour::Orange:
            label->setStyleSheet(QStringLiteral("color: orange"));
            break;
        default:
            label->setStyleSheet(QString());
            break;
        }
    }


private slots:
    void editRequested(const QModelIndex &index);
    void newExpiryDateSelected();

public slots:
    void refresh();

public:
    explicit CurrencyWidget(QWidget *parent = nullptr);

signals:

};

#endif // CURRENCYWIDGET_H
