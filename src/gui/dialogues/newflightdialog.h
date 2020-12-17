/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef NEWFLIGHT_H
#define NEWFLIGHT_H

#include <QDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include <QDebug>
#include <QCompleter>
#include <QStringList>
#include <QButtonGroup>
#include <QBitArray>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QTabWidget>
#include <QKeyEvent>

#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"

#include "src/experimental/adatabase.h"
#include "src/experimental/aflightentry.h"
#include "src/experimental/apilotentry.h"
#include "src/experimental/atailentry.h"
#include "src/functions/acalc.h"
#include "src/testing/atimer.h"

namespace Ui {
class NewFlight;
}

class NewFlightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFlightDialog(QWidget *parent = nullptr);
    explicit NewFlightDialog(int row_id, QWidget *parent = nullptr);
    ~NewFlightDialog();

signals:
    void goodInputReceived(QLineEdit*);
    void badInputReceived(QLineEdit*);
    void locationEditingFinished(QLineEdit*, QLabel*);
    void timeEditingFinished(QLineEdit*);
    void mandatoryLineEditsFilled();

private slots:
    void onGoodInputReceived(QLineEdit*);
    void onBadInputReceived(QLineEdit *);
    void onToUpperTriggered_textChanged(const QString&);
    void onPilotNameLineEdit_editingFinished();
    void onLocLineEdit_editingFinished(QLineEdit*, QLabel*);
    void onTimeLineEdit_editingFinished();
    void onMandatoryLineEditsFilled();
    void onCompleter_highlighted(const QString&);
    void onCompleter_activated(const QString &);
    void onCalendarWidget_clicked(const QDate &date);
    void onCalendarWidget_selected(const QDate &date);
    void onDoftLineEdit_entered();
/////// DEBUG
    void onInputRejected();
/////// DEBUG
    void on_calendarCheckBox_stateChanged(int arg1);
    void on_doftLineEdit_editingFinished();
    void on_cancelButton_clicked();
    void on_submitButton_clicked();
    void on_setAsDefaultButton_clicked();
    void on_restoreDefaultButton_clicked();
    void on_PilotFlyingCheckBox_stateChanged(int arg1);
    void on_IfrCheckBox_stateChanged(int);
    void on_manualEditingCheckBox_stateChanged(int arg1);
    void on_ApproachComboBox_currentTextChanged(const QString &arg1);
    void on_FunctionComboBox_currentIndexChanged(int index);
    void on_deptLocLineEdit_editingFinished();
    void on_destLocLineEdit_editingFinished();
    void on_acftLineEdit_editingFinished();

private:
    Ui::NewFlight *ui;

    experimental::AFlightEntry flightEntry;

    QList<QLineEdit*> mandatoryLineEdits;
    QList<QLineEdit*> primaryTimeLineEdits;
    QList<QLineEdit*> pilotsLineEdits;

    QBitArray mandatoryLineEditsGood;

    QStringList pilotList;
    QStringList tailsList;
    QStringList airportList;

    QMap<QString, int> pilotsIdMap;
    QMap<QString, int> tailsIdMap;
    QMap<QString, int> airportIcaoIdMap;
    QMap<QString, int> airportIataIdMap;
    QMap<QString, int> airportNameIdMap;

    void setup();
    void readSettings();
    void writeSettings();
    void setupButtonGroups();
    void setPopUpCalendarEnabled(bool state);
    void setupRawInputValidation();
    void setupLineEditSignalsAndSlots();
    void formFiller();
    void fillDeductibleData();
    experimental::TableData collectInput();
    bool eventFilter(QObject *object, QEvent *event);
    bool updateEnabled;
    bool isLessOrEqualThanBlockTime(const QString time_string);

    void addNewTail(QLineEdit*);
    void addNewPilot(QLineEdit *);
};


#endif // NEWFLIGHT_H
