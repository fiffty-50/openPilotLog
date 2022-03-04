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
#ifndef NEWTAIL_H
#define NEWTAIL_H

#include <QDialog>
#include <QCompleter>
#include <QMessageBox>
#include <QRegularExpression>
#include <QComboBox>

#include "src/classes/asettings.h"
#include "src/functions/acalc.h"
#include "src/database/adatabase.h"
#include "src/classes/atailentry.h"
#include "src/classes/aaircraftentry.h"

namespace Ui {
class NewTail;
}
/*!
 * \brief The NewTailDialog enables adding new Tail entries to the database or editing existing ones.
 *
 * \details The NewTailDialog offers two constructors, one is used to create a new Tail Entry
 * from scratch, while the other one is used to edit an existing entry. The existing entry
 * is identified by its ROW ID in the database and is then retreived, its data being used
 * to pre-fill the UI to enable editing the existing data.
 *
 * The Dialog is headed by a search-field containing template data retreived from the aircraft table.
 * This enables a fast and convenient way of rapidly filling out all the details for a given aircraft type.
 *
 * Think of aircraft and tails as classes and instances. The tail refers to the actual aircraft, which is
 * identified by its registration, normally a 5-letter alphanumeric, e.g. D-AIDB or LN-NEN. The aircraft table
 * in the database holds templates for the aircraft type, for example a Boeing 737 or Airbus 320. A typical
 * operator has several aircraft of the same type, for example OE-ABC, OE-DEF and OE-GHI, all of which are of the
 * aircraft type Airbus 320.
 *
 * The Dialog contains 4 QComboBoxes that have to be filled out before an entry can be submitted, since this data
 * is being used to deduct certain types of hours that have to be logged depending on the number or type of engines.
 *
 * A set of QValidators provides raw input validation for the line edits.
 *
 *
 *
 */
class NewTailDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief NewTailDialog - create a new ATailEntry and submit it to the database
     * \param new_registration - when called from the NewFlightDialog, pre-fills the registration already entered.
     */
    explicit NewTailDialog(const QString& new_registration, QWidget *parent = nullptr);
    /*!
     * \brief NewTailDialog - edit an existing Tail Entry
     * \param row_id - the ROW_ID of the entry to be edited in the database
     */
    explicit NewTailDialog(int row_id, QWidget *parent = nullptr);

    ~NewTailDialog();
private:

    Ui::NewTail *ui;

    ATailEntry entry;

    QStringList aircraftList;

    QMap<RowId_T, QString> idMap;

    void setupCompleter();
    void setupValidators();
    void fillForm(AEntry entry, bool is_template);
    bool verify();
    void submitForm();

private slots:
    void on_operationComboBox_currentIndexChanged(int index);
    void on_ppTypeComboBox_currentIndexChanged(int index);
    void on_ppNumberComboBox_currentIndexChanged(int index);
    void on_weightComboBox_currentIndexChanged(int index);
    void on_registrationLineEdit_textChanged(const QString &arg1);
    void on_buttonBox_accepted();
    void onSearchCompleterActivated();

};

#endif // NEWTAIL_H
