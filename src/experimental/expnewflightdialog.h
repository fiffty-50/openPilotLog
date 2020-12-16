#ifndef EXPNEWFLIGHTDIALOG_H
#define EXPNEWFLIGHTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include <QDebug>
#include <QCompleter>
#include <QLatin1Char>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QButtonGroup>
#include <QBitArray>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QTabWidget>

#include "src/experimental/adatabase.h"
#include "src/experimental/aflightentry.h"
#include "src/experimental/apilotentry.h"
#include "src/experimental/atailentry.h"

#include "src/functions/acalc.h"

#include "src/testing/atimer.h"

namespace Ui {
class ExpNewFlightDialog;
}

class ExpNewFlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExpNewFlightDialog(QWidget *parent = nullptr);
    explicit ExpNewFlightDialog(int row_id, QWidget *parent = nullptr);
    ~ExpNewFlightDialog();

signals:
    void goodInputReceived(QLineEdit*);
    void badInputReceived(QLineEdit*);
    void locationEditingFinished(QLineEdit*, QLabel*);
    void timeEditingFinished(QLineEdit*);
    void mandatoryLineEditsFilled();

private slots:

    void onGoodInputReceived(QLineEdit*);
    void onBadInputReceived(QLineEdit *);
    void onTextChangedToUpper(const QString&);
    void onPilotLineEdit_editingFinished();
    void onLocLineEdit_editingFinished(QLineEdit*, QLabel*);
    void onTimeLineEdit_editingFinished(QLineEdit*);
    void onMandatoryLineEditsFilled();
    void onCompleterHighlighted(const QString&);
    void onDateClicked(const QDate &date);
    void onDateSelected(const QDate &date);
    void onDoftLineEditEntered();

    void on_deptLocLineEdit_2_editingFinished();
    void on_destLocLineEdit_2_editingFinished();
    void on_acftLineEdit_2_editingFinished();


/////// DEBUG
    void onInputRejected();
/////// DEBUG

    void on_tofbTimeLineEdit_2_editingFinished();

    void on_tonbTimeLineEdit_2_editingFinished();



    void on_calendarCheckBox_stateChanged(int arg1);

    void on_doftLineEdit_2_editingFinished();

    void on_cancelButton_clicked();

    void on_submitButton_clicked();

private:
    Ui::ExpNewFlightDialog *ui;

    experimental::AFlightEntry flightEntry;

    QList<QLineEdit*> mandatoryLineEdits;

    QBitArray mandatoryLineEditsGood;

    QStringList pilotList;
    QStringList tailsList;
    QStringList airportList;

    QMap<QString, int> pilotsIdMap;
    QMap<QString, int> tailsIdMap;
    QMap<QString, int> airportIcaoIdMap;
    QMap<QString, int> airportIataIdMap;
    QMap<QString, int> airportNameIdMap;

    bool eventFilter(QObject *object, QEvent *event);

    bool updateEnabled;

    void setup();
    void readSettings();
    void writeSettings();
    void setupButtonGroups();
    void setPopUpCalendarEnabled(bool state);
    void setupRawInputValidation();
    void setupLineEditSignalsAndSlots();

    void fillDeductibleData();

};

#endif // EXPNEWFLIGHTDIALOG_H
