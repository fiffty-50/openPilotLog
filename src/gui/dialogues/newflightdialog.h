/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include <QComboBox>
#include <QTabWidget>
#include <QKeyEvent>
#include "src/functions/atime.h"

#include "src/classes/aflightentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/database/adatabase.h"
#include "src/classes/acompletiondata.h"


/*!
 * \brief The ValidationSetupData struct encapsulates the items required to initialise
 * the line edits with QValidators and QCompleters
 */
struct ValidationSetupData
{
    ValidationSetupData(QStringList* completion_data, const QRegularExpression* validation_RegEx)
        : completionData(completion_data), validationRegEx(validation_RegEx){};

    ValidationSetupData(const QRegularExpression* validation_RegEx)
        : completionData(nullptr), validationRegEx(validation_RegEx){};

    const QStringList* completionData;
    const QRegularExpression* validationRegEx;
};

namespace Ui {

class NewFlight;
}

/*!
 * \brief The NewFlightDialog enables the user to add a new flight or edit an existing one.
 * \details
 * - Most line edits have validators and completers.
 * - Validators are based on regular expressions, serving as raw input validation
 * - The Completers are based off the database and provide auto-completion
 * - mandatory line edits only emit editing finished if their content has passed
 *   raw input validation or focus is lost.
 * - Editing finished triggers validating inputs by mapping them to Database values
 *   where required and results in either pass or fail.
 * - A QBitArray is mainained containing the state of validity of the mandatory line edits
 * - The deducted entries are automatically filled if the necessary mandatory entries
 * are valid.
 * - Comitting an entry to the database is only allowed if all mandatory inputs are valid.
 */
class NewFlightDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief NewFlightDialog create a new flight and add it to the logbook.
     */
    explicit NewFlightDialog(ACompletionData &completion_data, QWidget *parent = nullptr);
    /*!
     * \brief NewFlightDialog Edit an existing logbook entry.
     */
    explicit NewFlightDialog(ACompletionData &completion_data, int row_id, QWidget *parent = nullptr);
    ~NewFlightDialog();

private slots:

    void onToUpperTriggered_textChanged(const QString&);
    void onPilotNameLineEdit_editingFinished();
    void onLocationEditingFinished(QLineEdit*, QLabel*);
    void onTimeLineEdit_editingFinished();
    void onCompleter_highlighted(const QString&);
    void onCompleter_activated(const QString &);
    void onCalendarWidget_clicked(const QDate &date);
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
    void on_deptTZComboBox_currentIndexChanged(int index);
    void on_destTZComboBox_currentIndexChanged(int index);

    void on_calendarPushButton_clicked();

private:
    Ui::NewFlight *ui;

    /*!
     * \brief a AFlightEntry object that is used to store either position data
     * from an old entry, is used to fill the form for editing an entry, or is
     * filled with new data for adding a new entry to the logbook.
     */
    AFlightEntry flightEntry;

    QVector<QLineEdit*> mandatoryLineEdits;
    QVector<QLineEdit*> primaryTimeLineEdits;
    QVector<QLineEdit*> pilotsLineEdits;

    /*!
     * \brief mandatoryLineEditsValid holds the minimum required information to create a
     * valid database entries.
     */
    QBitArray mandatoryLineEditsValid;
    enum mandatoryLineEdit {
        doft = 0,
        dept = 1,
        dest = 2,
        tofb = 3,
        tonb = 4,
        pic  = 5,
        acft = 6
    };
    void validateMandatoryLineEdit(mandatoryLineEdit line_edit){mandatoryLineEditsValid.setBit(line_edit, true);}
    void invalidateMandatoryLineEdit(mandatoryLineEdit line_edit){mandatoryLineEditsValid.setBit(line_edit, false);}
    bool timeLineEditsValid(){return mandatoryLineEditsValid[mandatoryLineEdit::tofb]
                                  && mandatoryLineEditsValid[mandatoryLineEdit::tonb];}
    bool acftLineEditValid(){return mandatoryLineEditsValid[mandatoryLineEdit::acft];}
    bool locLineEditsValid(){return mandatoryLineEditsValid[mandatoryLineEdit::dept]
                                 && mandatoryLineEditsValid[mandatoryLineEdit::dest];}
    bool allMandatoryLineEditsValid(){return mandatoryLineEditsValid.count(true) == 7;}

    //debug
    void validationStatus();
    /*!
     * Contains completion data for QCompleters and mapping user input
     */
    ACompletionData completionData;


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

    /*!
     * \brief Collects user input from the line edits and processes it to be ready
     * for database submission.
     */
    RowData_T collectInput();

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
