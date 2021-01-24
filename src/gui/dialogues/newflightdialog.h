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
#include "src/functions/atime.h"

#include "src/classes/aflightentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/database/adatabase.h"

namespace Ui {
class NewFlight;
}

class NewFlightDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief NewFlightDialog create a new flight and add it to the logbook.
     */
    explicit NewFlightDialog(QWidget *parent = nullptr);
    /*!
     * \brief NewFlightDialog Edit an existing logbook entry.
     */
    explicit NewFlightDialog(int row_id, QWidget *parent = nullptr);
    ~NewFlightDialog();

private slots:

    void onToUpperTriggered_textChanged(const QString&);
    void onPilotNameLineEdit_editingFinished();
    void onLocationEditingFinished(QLineEdit*, QLabel*);
    void onTimeLineEdit_editingFinished();
    void onCompleter_highlighted(const QString&);
    void onCompleter_activated(const QString &);
    void onCalendarWidget_clicked(const QDate &date);
    void onCalendarWidget_selected(const QDate &date);
    void onDoftLineEdit_entered();
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

    /*!
     * \brief a AFlightEntry object that is used to store either position data
     * from an old entry, is used to fill the form for editing an entry, or is
     * filled with new data for adding a new entry to the logbook.
     */
    AFlightEntry flightEntry;

    // [G]: Initial refactoring based on previous use.
    /*!
     * \brief Wrapper around Vector of mandatory line edits and their corresponding
     * "ok" QBitArray.
     */
    struct MandatoryLineEdits {
        QVector<QLineEdit*> lineEdits;
        QBitArray lineEditsValid;

        MandatoryLineEdits() = default;
        MandatoryLineEdits(std::initializer_list<QLineEdit*> init_list);
        void operator= (std::initializer_list<QLineEdit*> init_list);

        bool contains(QLineEdit* line_edit);
        void validate(QLineEdit* line_edit);
        void unvalidate(QLineEdit* line_edit);
        int countValid();
        int size();
        bool validAt(int idx);
        bool allValid();
        QLineEdit* operator[] (int idx);

    } mandatoryLineEdits;

    QVector<QLineEdit*> primaryTimeLineEdits;
    QVector<QLineEdit*> pilotsLineEdits;

    /*!
     * To be used by the QCompleters
     */
    QStringList pilotList;
    QStringList tailsList;
    QStringList airportList;

    /*!
     * \brief Used to map user input to database keys
     */
    QMap<PilotName_t, PilotRowId_t> pilotsIdMap;
    QMap<TailRgstr_t, TailId_t> tailsIdMap;
    QMap<ArprtICAO_t, ArprtId_t> airportIcaoIdMap;
    QMap<ArprtIATA_t, ArprtId_t> airportIataIdMap;
    QMap<ArprtName_t, ArprtId_t> airportNameIdMap;

    Opl::Time::FlightTimeFormat flightTimeFormat;

    /*!
     * \brief If the user elects to manually edit function times, automatic updating
     * is disabled.
     */
    bool updateEnabled;

    void setup();
    void readSettings();
    void writeSettings();
    void setupButtonGroups();
    void setPopUpCalendarEnabled(bool state);
    void setupRawInputValidation();
    void setupSignalsAndSlots();
    void formFiller();
    void fillDeductibleData();

    void onMandatoryLineEditsFilled();
    void onGoodInputReceived(QLineEdit*);
    void onBadInputReceived(QLineEdit *);
    bool eventFilter(QObject *object, QEvent *event);
    bool isLessOrEqualThanBlockTime(const QString time_string);

    void addNewTail(QLineEdit*);
    void addNewPilot(QLineEdit *);

    RowData_t collectInput();

    /*!
     * \brief converts a time string as used in the UI to an integer of minutes for
     * use in the database based on the format in use in the Dialog
     */
    inline int stringToMinutes(const QString &time_string, Opl::Time::FlightTimeFormat format)
    {
        return ATime::toMinutes(ATime::fromString(time_string, format));
    }

    /*!
     * \brief minutesToString converts an integer of minutes as received from the database
     * to a String to be displayed in the UI, based on the format in use in the Dialog.
     */
    inline QString minutesToString(const int minutes, Opl::Time::FlightTimeFormat format)
    {
        return ATime::toString(ATime::fromMinutes(minutes), format);
    }
};


#endif // NEWFLIGHT_H
