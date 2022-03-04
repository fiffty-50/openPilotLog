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
#ifndef NewFlightDialog_H
#define NewFlightDialog_H

#include <QDialog>
#include <QDebug>
#include <QLineEdit>
#include <QList>
#include <QBitArray>

#include "src/functions/atime.h"
#include "src/classes/acompletiondata.h"
#include "src/classes/aflightentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/database/adatabase.h"
#include "src/opl.h"

/*!
 * \brief The ValidationItem enum contains the items that are mandatory for logging a flight:
 * Date of Flight, Departure, Destination, Time Off Blocks, Time On Blocks, Pilot in Command, Aircraft Registration
 */
enum ValidationItem {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6};

/*!
 * \brief The ValidationState class encapsulates a QBitArray that has a bit set (or unset) depending on wether the
 * input for the associated index has been verified. The indexes correspond to the mandatory items enumerated in the
 * ValidationItem enum.
 */
class ValidationState {
public:
    ValidationState() = default;

    void validate(ValidationItem item)             { validationArray[item] = true;};
    void validate(int index)                       { validationArray[index] = true;};
    void invalidate(ValidationItem item)           { validationArray[item] = false;}
    void invalidate(int index)                     { validationArray[index] = false;}
    inline bool allValid() const                   { return validationArray.count(true) == 7;};
    inline bool timesValid() const                 { return validationArray[ValidationItem::tofb] && validationArray[ValidationItem::tonb];}
    inline bool locationsValid() const             { return validationArray[ValidationItem::dept] && validationArray[ValidationItem::dest];}
    inline bool nightDataValid() const             { return timesValid() && locationsValid() && validationArray[ValidationItem::doft];}
    inline bool acftValid() const                  { return validationArray[ValidationItem::acft];}
    inline bool validAt(int index) const           { return validationArray[index];}
    inline bool validAt(ValidationItem item) const { return validationArray[item];}

    // Debug
    void printValidationStatus() const {
        QString deb_string("\033[mValidation State:\tdoft\tdept\tdest\ttofb\ttonb\tpic\tacft\n");
        deb_string += "\t\t\t\t";
        for (int i = 0; i < 7; i++) { //\033[32m
            if (validationArray[i])
                deb_string += "\t\033[32m" + QString::number(validationArray[i]);
            else
                deb_string += "\t\033[31m" + QString::number(validationArray[i]);
        }
        deb_string += QLatin1String("\u001b[38;5;75m"); // return to default DEB
        qDebug().noquote() << deb_string;
    }
private:
    QBitArray validationArray = QBitArray(7);
};

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
 * QMaps of known good values from the database and their respective ROW_IDs. If user-entered data has been matched to a known good database
 * value, the data is considered acceptable. This means that in order to, for example, log a flight with a certain Pilot, that this pilot
 * already has to exist in the pilots table. If this is not the case, the user is prompted to add a new pilot (or aircraft) to the database
 * before proceeding. In order to make this matching process seamless for the user, the completionData also contains a set of QStringLists
 * for each of the database tables which are used to create QCompleters that provide pop-up completion on the respective QLineEdits.
 *
 * Once the user is satisfied with his entries, a final set of input verification is triggered and the entry is submitted to the database,
 * see on_buttonBox_accepted() and ADatabase::commit()
 */
class NewFlightDialog : public QDialog
{
    Q_OBJECT

public:

    /*!
     * \brief NewFlightDialog - Creates a NewFlightDialog that can be used to add a new flight entry to the logbook
     * \param completion_data - contains QStringLists for the QCompleter to autocomplete Airport Codes, Pilot Names and aircraft registrationsn
     */
    explicit NewFlightDialog(ACompletionData& completion_data, QWidget *parent = nullptr);
    /*!
     * \brief NewFlightDialog - Creates a NewFlightDialog that can be used to edit an existing entry in the logbook
     * \param completion_data - contains QStringLists for the QCompleter to autocomplete Airport Codes, Pilot Names and aircraft registrationsn
     * \param row_id - The database ROW ID of the entry to be edited
     */
    explicit NewFlightDialog(ACompletionData& completion_data, RowId_T row_id, QWidget* parent = nullptr);
    ~NewFlightDialog();

private:
    Ui::NewFlightDialog *ui;
    ACompletionData completionData;
    ValidationState validationState;

    /*!
     * \brief a AFlightEntry object that is used to store either position data
     * from an old entry, is used to fill the form for editing an entry, or is
     * filled with new data for adding a new entry to the logbook.
     */
    AFlightEntry flightEntry;

    /*!
     * \brief timeLineEdits - Line Edits for time Off Blocks and Time On Blocks
     */
    static const inline QList<QLineEdit*>* timeLineEdits;
    /*!
     * \brief locationLineEdits - Line Edits for Departure and Destination Airports
     */
    static const inline QList<QLineEdit*>* locationLineEdits;
    /*!
     * \brief pilotNameLineEdits - Line Edits for Pilot in Command, Second in Command (Co-Pilot) and Third Pilot
     */
    static const inline QList<QLineEdit*>* pilotNameLineEdits;
    /*!
     * \brief mandatoryLineEdits - Contains the Line Edits that are needed for logging a complete flight from A to B.
     * The list is ordered like the ValidationItem enum so that indexed access is possible using the enum.
     */
    static const inline QList<QLineEdit*>* mandatoryLineEdits;
    static const inline QLatin1String self = QLatin1String("self");

    void init();
    void setupRawInputValidation();
    void setupSignalsAndSlots();
    void readSettings();
    void fillWithEntryData();

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

    void updateNightCheckBoxes();
    void setNightCheckboxes();
    void updateBlockTimeLabel();

    bool addNewTail(QLineEdit& parent_line_edit);
    bool addNewPilot(QLineEdit& parent_line_edit);

    RowData_T prepareFlightEntryData();


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
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
};


#endif // NewFlightDialog_H
