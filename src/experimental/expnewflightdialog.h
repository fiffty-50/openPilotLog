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

#include "src/classes/astrictrxvalidator.h"

#include "src/experimental/adatabase.h"
#include "src/experimental/aflightentry.h"
#include "src/experimental/apilotentry.h"
#include "src/experimental/atailentry.h"

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

private slots:

    void onGoodInputReceived(QLineEdit*);
    void onBadInputReceived(QLineEdit*);

    void on_deptLocLineEdit_2_editingFinished();

    void onInputRejected();

    void onTextChangedToUpper(const QString&);

    void on_acftLineEdit_2_editingFinished();

    void on_destLocLineEdit_2_editingFinished();

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
    QMap<QString, int> airportIdentifierIdMap;
    QMap<QString, int> airportNameIdMap;

    void setupRawInputValidation();
    void setupMandatoryLineEdits();

    void setupValidators();
};

#endif // EXPNEWFLIGHTDIALOG_H
