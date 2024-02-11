/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#ifndef NewFlightDialog_H
#define NewFlightDialog_H

#include <QDialog>
#include <QDebug>
#include <QLineEdit>
#include <QList>
#include <QBitArray>

#include "QtWidgets/qcalendarwidget.h"
#include "src/database/database.h"
#include "src/database/flightentry.h"
#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/verification/userinput.h"
#include "src/opl.h"
#include "src/gui/verification/validationstate.h"
#include "src/classes/date.h"

namespace Ui {
class NewFlightDialog;
}
/*!
 * \brief The NewFlightDialog is used to add or edit entries from the flights table in the database
 * \details NewFlightDialog offers two constructors, one is used to create a new flight entry from scratch, where the other one is used
 * to edit an existing entry. This entry is identified by its ROW ID, which is used to retreive the entry data and pre-populate the
 * user interface with the data from the database.
 *
 * The flights table is the core of the application's database and care needs to be taken when interfacing with it.
 *
 * To ensure only good data is written to the database, the ValidationState class is used. It contains a QBitArray with each bit representing
 * a mandatory data point. The array is initialized to all false and progressively changed to true as entry data is validated. An entry can
 * only be submitted if all the verification bits are set.
 *
 * Inputs from the user are verified with a two-step process. The first level of verification is accomplished by QRegularExpressionValidator, which limits the
 * user to only inputting generally acceptable data (like numbers for date or time, or characters for airport identifiers). If the input
 * passes this sanity check, the line edits emit the editingFinished() signal, which triggers a more granular and sophisticated set of input
 * verification, broadly based on cross-checking the entered data against known good values. The ACompletionData class is used to provide
 * QHashs of known good values from the database and their respective ROW_IDs. If user-entered data has been matched to a known good database
 * value, the data is considered acceptable. This means that in order to, for example, log a flight with a certain Pilot, that this pilot
 * already has to exist in the pilots table. If this is not the case, the user is prompted to add a new pilot (or aircraft) to the database
 * before proceeding. In order to make this matching process seamless for the user, the completionData also contains a set of QStringLists
 * for each of the database tables which are used to create QCompleters that provide pop-up completion on the respective QLineEdits.
 *
 * Once the user is satisfied with his entries, a final set of input verification is triggered and the entry is submitted to the database,
 * see on_buttonBox_accepted() and Database::commit()
 */
class NewFlightDialog : public EntryEditDialog
{
    Q_OBJECT

public:

    /*!
     * \brief NewFlightDialog - Creates a NewFlightDialog that can be used to add a new flight entry to the logbook
     */
    explicit NewFlightDialog(QWidget *parent = nullptr);
    /*!
     * \brief NewFlightDialog - Creates a NewFlightDialog that can be used to edit an existing entry in the logbook
     * \param row_id - The database ROW ID of the entry to be edited
     */
    explicit NewFlightDialog(int row_id, QWidget* parent = nullptr);
    ~NewFlightDialog();



private:
    Ui::NewFlightDialog *ui;
    ValidationState validationState;
    QCalendarWidget *calendar;
    OPL::DateTimeFormat m_format;

    /*!
     * \brief a AFlightEntry object that is used to store either position data
     * from an old entry, is used to fill the form for editing an entry, or is
     * filled with new data for adding a new entry to the logbook.
     */
    OPL::FlightEntry flightEntry;

    /*!
     * \brief timeLineEdits - Line Edits for time Off Blocks and Time On Blocks
     */
    static const inline QList<QLineEdit*> *timeLineEdits;
    /*!
     * \brief locationLineEdits - Line Edits for Departure and Destination Airports
     */
    static const inline QList<QLineEdit*> *locationLineEdits;
    /*!
     * \brief pilotNameLineEdits - Line Edits for Pilot in Command, Second in Command (Co-Pilot) and Third Pilot
     */
    static const inline QList<QLineEdit*> *pilotNameLineEdits;
    /*!
     * \brief mandatoryLineEdits - Contains the Line Edits that are needed for logging a complete flight from A to B.
     * The list is ordered like the ValidationItem enum so that indexed access is possible using the enum.
     */
    static const inline QList<QLineEdit*> *mandatoryLineEdits;
    // static const inline QLatin1String self = QLatin1String("self");
    static const inline QHash<int, QString> pilotFuncionsMap = {
                                           {0, OPL::FlightEntry::TPIC},
                                           {1, OPL::FlightEntry::TPICUS},
                                           {2, OPL::FlightEntry::TSIC},
                                           {3, OPL::FlightEntry::TDUAL},
                                           {4, OPL::FlightEntry::TFI},
                                           };
    static const inline QHash<ValidationState::ValidationItem, QString> validationItemsDisplayNames = {
        {ValidationState::ValidationItem::doft, QObject::tr("Date of Flight")},
        {ValidationState::ValidationItem::dept, QObject::tr("Departure Airport")},
        {ValidationState::ValidationItem::dest, QObject::tr("Destination Airport")},
        {ValidationState::ValidationItem::tofb, QObject::tr("Time Off Blocks")},
        {ValidationState::ValidationItem::tonb, QObject::tr("Time on Blocks")},
        {ValidationState::ValidationItem::pic, QObject::tr("PIC Name")},
        {ValidationState::ValidationItem::acft, QObject::tr("Aircraft Registration")},
    };

    /*!
     * \brief init - set up the UI
     */
    void init();
    /*!
     * \brief setPilotFunction - Reads the application setting and pre-fills the logbook owners
     * desired function
     */
    void setPilotFunction();
    void setupRawInputValidation();
    void setupSignalsAndSlots();
    void readSettings();
    void fillWithEntryData();
    bool verifyUserInput(QLineEdit *line_edit, const UserInput &input);

    /*!
     * \brief onGoodInputReceived - Sets a verification bit for the line edit that has been edited.
     * \details When a Line Edit of the mandatoryLineEdits list is edited, on editingFinished(), the received input is
     * evaluated and if considered a good input, the validation bit in validationState is set.
     */
    void onGoodInputReceived(QLineEdit *line_edit);
    /*!
     * \brief onBadInputReceived Unsets a verification bit for the line edit that has been edited.
     * \details When a Line Edit of the mandatoryLineEdits list is edited, on editingFinished(), the received input is
     * evaluated and if considered a bad input, the validation bit in validationState is unset.
     */
    void onBadInputReceived(QLineEdit *line_edit);

    /*!
     * \brief addNewDatabaseElement Adds a new element to the database
     * \param parent - The line edit that triggered the event
     * \param table - The table to which the new element is added
     * \return true on success
     * \details The flights database has a couple of NOT NULL constraints which
     * must be met before a new flight can be submitted. If the user enters a
     * constrained field which does not exist in a related table (pilots, tails
     * or airports), the user is prompted to add a new entry to one of those
     * tables before proceeding to log a flight with the missing element.
     */
    bool addNewDatabaseElement(QLineEdit* parent, OPL::DbTable table);

    /*!
     * \brief userWantsToAddNewEntry - Asks the user if he wants to add a new entry to the database
     * \param table - The table to which the entry will be committed.
     * \return true if the reply is QMessageBox::Yes
     */
    bool userWantsToAddNewEntry(OPL::DbTable table);


    void informUserAboutMissingItems();
    bool pilotFunctionsInvalid();
    bool duplicateNamesPresent();
    bool flightTimeIsZero();
    OPL::RowData_T prepareFlightEntryData();

    const static inline auto CAT_3 = QLatin1String(OPL::GLOBALS->getApproachTypes()[3].toLatin1());

private slots:
    void toUpper(const QString& text);
    void onTimeLineEdit_editingFinished();
    void onPilotNameLineEdit_editingFinshed();
    void onLocationLineEdit_editingFinished();
    void on_acftLineEdit_editingFinished();
    void on_doftLineEdit_editingFinished();
    void on_buttonBox_accepted();
    void on_pilotFlyingCheckBox_stateChanged(int arg1);
    void on_approachComboBox_currentTextChanged(const QString &arg1);
    void on_functionComboBox_currentIndexChanged(int index);

    void on_calendarPushButton_clicked();

    void calendarDateSelected();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

    // EntryEditDialog interface
public:
    virtual void loadEntry(int rowID) override;
    virtual bool deleteEntry(int rowID) override;
};


#endif // NewFlightDialog_H
