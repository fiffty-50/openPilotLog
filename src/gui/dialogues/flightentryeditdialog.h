/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#ifndef FLIGHTENTRYEDITDIALOG_H
#define FLIGHTENTRYEDITDIALOG_H

#include "entryeditdialog.h"
#include "src/gui/verification/flightentryparser.h"
#include "src/gui/verification/userinput.h"
#include "src/opl.h"
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

class FlightEntryEditDialog : public EntryEditDialog {
  public:
    FlightEntryEditDialog(QWidget *parent = nullptr);
    FlightEntryEditDialog(int rowId, QWidget *parent = nullptr);

    void loadEntry(int rowID) override;
    void loadEntry(const OPL::Row &entry);
    bool deleteEntry(int rowID) override;

  private:
    /*!
     * \brief m_entryParser encapsulates a new FlightEntry or one loaded from the Database
     */
    OPL::FlightEntryParser m_entryParser;
    static constexpr int NEW_ENTRY = 0;
    int m_rowID                    = NEW_ENTRY;
    OPL::DateTimeFormat m_displayFormat;

    QLineEdit dateLineEdit    = QLineEdit(this);
    QLineEdit timeOutLineEdit = QLineEdit(this);
    QLineEdit timeOffLineEdit = QLineEdit(this);
    QLineEdit timeOnLineEdit  = QLineEdit(this);
    QLineEdit timeInLineEdit  = QLineEdit(this);

    QLineEdit departureLineEdit   = QLineEdit(this);
    QLineEdit destinationLineEdit = QLineEdit(this);

    QLineEdit firstPilotLineEdit  = QLineEdit(this);
    QLineEdit secondPilotLineEdit = QLineEdit(this);
    QLineEdit thirdPilotLineEdit  = QLineEdit(this);

    QLineEdit registrationLineEdit = QLineEdit(this);

    QLineEdit remarksLineEdit      = QLineEdit(this);
    QLineEdit flightNumberLineEdit = QLineEdit(this);

    QComboBox pilotFunctionComboBox = QComboBox(this);
    QComboBox flightRulesComboBox   = QComboBox(this);
    QComboBox approachTypeComboBox  = QComboBox(this);

    QSpinBox takeOffCountSpinBox = QSpinBox(this);
    QSpinBox landingCountSpinBox = QSpinBox(this);

    QCheckBox pilotFlyingCheckBox = QCheckBox(this);

    QPushButton dateButton         = QPushButton(tr("Date"), this);
    QLabel dateDisplayLabel        = QLabel(this);
    QLabel timeOutLabel            = QLabel(tr("Off Blocks"), this);
    QLabel timeOffLabel            = QLabel(tr("Take Off"), this);
    QLabel timeOnLabel             = QLabel(tr("Landing"), this);
    QLabel timeInLabel             = QLabel(tr("On Blocks"), this);
    QLabel departureLabel          = QLabel(tr("Departure"), this);
    QLabel departureDisplayLabel   = QLabel(this);
    QLabel destinationLabel        = QLabel(tr("Destination"), this);
    QLabel destinationDisplayLabel = QLabel(this);
    QLabel firstPilotLabel         = QLabel(tr("PIC"), this);
    QLabel secondPilotLabel        = QLabel(tr("SIC"), this);
    QLabel thirdPilotLabel         = QLabel(tr("Third Pilot"), this);
    QLabel registrationLabel       = QLabel(tr("Registration"), this);
    QLabel remarksLabel            = QLabel(tr("Remarks"), this);
    QLabel flightNumberLabel       = QLabel(tr("Flight Number"), this);
    QLabel pilotFlyingLabel        = QLabel(tr("Pilot Flying"), this);
    QLabel takeOffCountLabel       = QLabel(tr("Take Off"), this);
    QLabel landingCountLabel       = QLabel(tr("Landing"), this);
    QLabel totalTimeLabel          = QLabel(tr("Total Time"), this);
    QLabel totalTimeDisplayLabel   = QLabel("00:00", this);
    QLabel pilotFunctionLabel      = QLabel(tr("Function"), this);
    QLabel approachTypeLabel       = QLabel(tr("Approach"), this);
    QLabel flightRulesLabel        = QLabel(tr("Flight Rules"), this);

    QCalendarWidget *calendarWidget;

    QDialogButtonBox acceptButtonBox =
        QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    const std::array<QLineEdit *, 4> timeLineEdits     = {&timeOutLineEdit, &timeOffLineEdit,
                                                          &timeOnLineEdit, &timeInLineEdit};
    const std::array<QLineEdit *, 2> locationLineEdits = {&departureLineEdit, &destinationLineEdit};
    const QList<QLineEdit *> pilotNameLineEdits        = {&firstPilotLineEdit, &secondPilotLineEdit,
                                                          &thirdPilotLineEdit};

    void init();
    void setupUI();
    void setupAutoCompletion();
    void setupSignalsAndSlots();
    void readSettings();
    bool verifyUserInput(QLineEdit *lineEdit, const UserInput &input);
    void onBadInputReceived(QLineEdit *lineEdit);
    void onGoodInputReceived(QLineEdit *lineEdit);
    void updateAirportLabels();

    /*!
     * \brief Add the data from combo and spin boxes to the flight entry
     */
    void collectSecondaryFlightData();

    /*!
     * \brief run some checks on user data that are not covered by basic input validation
     */
    bool runSanityChecks();

  private slots:
    void onDialogAccepted();
    void onCalendarRequested();
    void onCalendarDateSelected();
    void onPilotFlyingCheckboxStateChanged(int index);

    // line edits
    void onDateEditingFinished();
    void onTimeOutEditingFinished();
    void onTimeInEditingFinished();
    void onNameEditingFinished(QLineEdit *lineEdit);
    void onRegistrationEditingFinished(QLineEdit *lineEdit);
    void onLocationEditingFinished(QLineEdit *lineEdit);

    void onRemarksEditingFinished();
    void onFlightNumberEditingFinished();

    /*!
     * \brief adds a new Database Element
     * \param caller - The Line Edit that called the function
     * \param table - The OPL::DBTable where the entry is to be created
     * \return true if a new entry was succesfully created
     * \details This function queries the user if he wants to add a new Element to the
     * Database. If so desired, a new dialog is launched to create the entry. If the
     * return code indicates success, the caller text is set to the just now created
     * new entry
     */
    bool addNewDatabaseElement(QLineEdit *caller, const OPL::DbTable table);
};

#endif // FLIGHTENTRYEDITDIALOG_H
