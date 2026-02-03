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
 *You should have received acopy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "flightlogentryeditdialog.h"
#include "src/calc/greatcircletrack.h"
#include "src/calc/nighttime.h"
#include "src/classes/date.h"
#include "src/classes/settings.h"
#include "src/classes/time.h"
#include "src/database/airportgeographicalinfo.h"
#include "src/database/airportinfo.h"
#include "src/database/database.h"
#include "src/database/flightdata.h"
#include "src/database/pilotinfo.h"
#include "src/database/tailregistrationsinfo.h"
#include "src/gui/comboboxes/dbselectioncombobox.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/gui/dialogues/tailentryeditdialog.h"
#include "src/gui/verification/flightdatabuilder.h"
#include "src/gui/verification/flightsegmentbuilder.h"
#include "src/opl.h"
#include <QCalendarWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTextFormat>
#include <QTimeEdit>
#include <QValidator>

namespace OPL {

FlightLogEntryEditDialog::FlightLogEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent), m_dateFormatString(Settings::getDateFormatString()),
      m_timeFormatString(Settings::getTimeFormatString()), m_night_angle(Settings::getNightAngle())
{
    init();
}

void FlightLogEntryEditDialog::loadEntry(int event_row_id)
{
    // Try to load the flight data
    if (auto flightOpt = OPL::FlightData::getFlightData(event_row_id); flightOpt) {
        m_eventId = event_row_id;

        const auto &flight_data  = *flightOpt;
        const auto &log_entry    = *flight_data.logEntry();
        const auto &flight_entry = *flight_data.flightEntry();

        DEB << "Log Entry:" << log_entry;
        DEB << "Flight Entry:" << flight_entry;
        DEB << "Movements:" << *flight_data.movementEntries();

        // Create Signal Blockers for line edits that run verification logic
        auto setText = [](QLineEdit *line_edit, const QString &text) {
            // QSignalBlocker b(line_edit);
            line_edit->setText(text);
        };
        auto setCurrentText = [](DbSelectionComboBox *box, const QString &text) {
            // QSignalBlocker b(box);
            box->setCurrentText(text);
        };

        // Populate UI fields
        dateEdit->setDate(log_entry.getDate());
        timeOffEdit->setTime(QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOffBlocksMs()));
        timeOnEdit->setTime(QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOnBlocksMs()));
        setCurrentText(deptComboBox, airportData->icao(flight_entry.getDepartureId()));
        setCurrentText(destComboBox, airportData->icao(flight_entry.getDestinationId()));
        setCurrentText(registrationComboBox, tailsData->registration(flight_entry.getTailId()));
        setCurrentText(picComboBox, pilotsData->name(flight_entry.getPicId()));
        setCurrentText(sicComboBox, pilotsData->name(flight_entry.getSecondPilotId()));
        flightNumberLineEdit->setText(flight_entry.getFlightNumber());
        remarksTextEdit->setPlainText(log_entry.getRemarks());

        // Movements
        takeOffCountSpinBox->setValue(flight_data.getTakeOffCount());
        landingCountSpinBox->setValue(flight_data.getLandingCount());

        // Segment Data
        {
            const QSignalBlocker b(pilotFlyingCheckBox);
            pilotFlyingCheckBox->setChecked(flight_data.isPilotFlying());
        }
        flightRulesComboBox->setCurrentIndex(flight_data.isIfr());
        pilotFunctionComboBox->setCurrentText(flight_data.pilotFunction());
    }
    else {
        // Flight not found
        WARN("Unable to load Flight with Log Event Id: " + QString::number(event_row_id));
    }
}

bool FlightLogEntryEditDialog::deleteEntry(int row_id) { return false; }

void FlightLogEntryEditDialog::init()
{
    // Main Layout
    // 5 columns with the middle column used as a spacer or in some cases for display Labels
    gridLayout         = new QGridLayout(this);
    int row            = 0;
    constexpr int col0 = 0;
    constexpr int col1 = 1;
    constexpr int col2 = 2; // middle separator
    constexpr int col3 = 3;
    constexpr int col4 = 4;

    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Left side (cols 0–1), optionally fills col 2
    auto addLeft = [&](QWidget *left, QWidget *right, QWidget *middle = nullptr) {
        gridLayout->addWidget(left, row, col0, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col1, singleSpan, singleSpan);

        if (middle)
            gridLayout->addWidget(middle, row, col2, singleSpan, singleSpan);
        else
            gridLayout->addWidget(new QLabel(this), row, col2, singleSpan, singleSpan);
    };

    // Right side (cols 3–4), advances row
    auto addRight = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, col3, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col4, singleSpan, singleSpan);
        row++;
    };

    // Row
    // Left
    datePushButton   = new QPushButton(this);
    dateEdit         = new QDateEdit(this);
    dateDisplayLabel = new QLabel(this);
    addLeft(datePushButton, dateEdit, dateDisplayLabel);

    // Right
    registrationLabel = new QLabel(this);
    // registrationLineEdit = new QLineEdit(this);
    registrationComboBox = new DbSelectionComboBox(DbSelectionComboBox::TailRegistrations, this);
    addRight(registrationLabel, registrationComboBox);

    // Row
    // Left
    deptLabel        = new QLabel(this);
    deptComboBox     = new DbSelectionComboBox(DbSelectionComboBox::AirportCodes, this);
    deptDisplayLabel = new QLabel(this);
    addLeft(deptLabel, deptComboBox, deptDisplayLabel);

    // Right
    picLabel    = new QLabel(this);
    picComboBox = new DbSelectionComboBox(DbSelectionComboBox::PilotNames, this);
    addRight(picLabel, picComboBox);

    // Row
    // Left
    destLabel        = new QLabel(this);
    destComboBox     = new DbSelectionComboBox(DbSelectionComboBox::AirportCodes, this);
    destDisplayLabel = new QLabel(this);
    addLeft(destLabel, destComboBox, destDisplayLabel);

    // Right
    sicLabel    = new QLabel(this);
    sicComboBox = new DbSelectionComboBox(DbSelectionComboBox::PilotNames, this);
    addRight(sicLabel, sicComboBox);

    // Row
    // Left
    timeOffLabel = new QLabel(this);
    timeOffEdit  = new QTimeEdit(this);
    addLeft(timeOffLabel, timeOffEdit);

    // Right
    flightNumberLabel    = new QLabel(this);
    flightNumberLineEdit = new QLineEdit(this);
    addRight(flightNumberLabel, flightNumberLineEdit);

    // Row
    // Left
    timeOnLabel = new QLabel(this);
    timeOnEdit  = new QTimeEdit(this);
    addLeft(timeOnLabel, timeOnEdit);

    // Right
    pilotFlyingCheckBox = new QCheckBox(this);
    gridLayout->addWidget(pilotFlyingCheckBox, row, col3, singleSpan, spanRemaining);
    row++;

    // Row
    // Left
    pilotFunctionLabel    = new QLabel(this);
    pilotFunctionComboBox = new QComboBox(this);
    addLeft(pilotFunctionLabel, pilotFunctionComboBox);

    // Right
    takeOffCountLabel   = new QLabel(this);
    takeOffCountSpinBox = new QSpinBox(this);
    addRight(takeOffCountLabel, takeOffCountSpinBox);

    // Row
    // Left
    flightRulesLabel    = new QLabel(this);
    flightRulesComboBox = new QComboBox(this);
    addLeft(flightRulesLabel, flightRulesComboBox);

    // Right
    landingCountLabel   = new QLabel(this);
    landingCountSpinBox = new QSpinBox(this);
    addRight(landingCountLabel, landingCountSpinBox);

    // Row
    // Left
    remarksLabel    = new QLabel(this);
    remarksTextEdit = new QPlainTextEdit(this);
    remarksTextEdit->setMaximumHeight(flightNumberLineEdit->sizeHint().height() * 2);
    addLeft(remarksLabel, remarksTextEdit);

    // Right
    totalTimeLabel        = new QLabel(this);
    totalTimeDisplayLabel = new QLabel(this);
    addRight(totalTimeLabel, totalTimeDisplayLabel);

    // Row
    buttonBox = new QDialogButtonBox(this);
    // buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, row, col4, singleSpan, singleSpan);

    m_locationLineEdits = {deptComboBox->lineEdit(), destComboBox->lineEdit()};

    setTabOrder({datePushButton, dateEdit, deptComboBox, destComboBox, timeOffEdit, timeOnEdit,
                 pilotFunctionComboBox, flightRulesComboBox, registrationComboBox, picComboBox,
                 sicComboBox, flightNumberLineEdit, pilotFlyingCheckBox, takeOffCountSpinBox,
                 landingCountSpinBox, buttonBox});

    retranslateUi();
    setupValidationAndCompletion();
    setupSlots();
    readSettings();
    dateEdit->setFocus();
}

void FlightLogEntryEditDialog::retranslateUi()
{
    datePushButton->setText(tr("Date"));
    dateDisplayLabel->setText(dateEdit->date().toString(QStringLiteral("ddd d MMM, yyyy")));
    deptLabel->setText(tr("Departure"));
    destLabel->setText(tr("Destination"));
    timeOffLabel->setText(tr("Off Blocks"));
    timeOnLabel->setText(tr("On Blocks"));
    totalTimeLabel->setText(tr("Total"));
    pilotFunctionLabel->setText(tr("Function"));
    flightRulesLabel->setText(tr("Flight Rules"));
    registrationLabel->setText(tr("Registration"));
    picLabel->setText(tr("Pilot in Command"));
    sicLabel->setText(tr("Second Pilot"));
    flightNumberLabel->setText(tr("Flight Number"));
    pilotFlyingCheckBox->setText(tr("Pilot Flying"));
    takeOffCountLabel->setText(tr("Take Off"));
    landingCountLabel->setText(tr("Landing"));
    remarksLabel->setText(tr("Remarks"));
    totalTimeDisplayLabel->setText(QStringLiteral("00:00"));
}

void FlightLogEntryEditDialog::setupValidationAndCompletion()
{
    // Setup Widegts
    dateEdit->setDisplayFormat(m_dateFormatString);
    dateEdit->setCalendarPopup(true);
    dateEdit->setTimeZone(QTimeZone::UTC);
    dateEdit->setMinimumDate(OPL::Date::minimumDate());
    dateEdit->setMaximumDate(OPL::Date::maximumDate());
    dateEdit->setDate(QDate::currentDate());
    dateDisplayLabel->setMinimumWidth(200);
    dateDisplayLabel->setMaximumWidth(200);
    QFont f = dateDisplayLabel->font();
    f.setItalic(true);
    dateDisplayLabel->setFont(f);
    deptDisplayLabel->setFont(f);
    destDisplayLabel->setFont(f);

    timeOffEdit->setDisplayFormat(m_timeFormatString);
    timeOffEdit->setTimeZone(QTimeZone::UTC);
    timeOnEdit->setDisplayFormat(m_timeFormatString);
    timeOnEdit->setTimeZone(QTimeZone::UTC);

    takeOffCountSpinBox->setMinimum(0);
    landingCountSpinBox->setMinimum(0);

    OPL::GLOBALS->loadPilotFunctions(pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(flightRulesComboBox);

    // Setup Basic Input Validation for the airport code entries
    for (const auto &line_edit : std::as_const(m_locationLineEdits)) {
        const auto val = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, line_edit);
        line_edit->setValidator(val);
    }
}

void FlightLogEntryEditDialog::setupSlots()
{
    // Button Box
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FlightLogEntryEditDialog::on_accepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // Registration
    connect(registrationComboBox, &DbSelectionComboBox::newValueEntered, this,
            &FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered);
    // Pilot Name
    connect(picComboBox, &DbSelectionComboBox::newValueEntered, this,
            &FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered);
    connect(sicComboBox, &DbSelectionComboBox::newValueEntered, this,
            &FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered);

    // Location Line Edits
    connect(deptComboBox, &DbSelectionComboBox::newValueEntered, this,
            &FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered);
    connect(destComboBox, &DbSelectionComboBox::newValueEntered, this,
            &FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered);
    // Display the airport name when the combobox is edited or the popup completer is used
    connect(deptComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
        deptDisplayLabel->setText(airportData->nameFromRowId(deptComboBox->currentData().toInt()));
    });
    connect(destComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
        destDisplayLabel->setText(airportData->nameFromRowId(destComboBox->currentData().toInt()));
    });
    connect(deptComboBox, &QComboBox::highlighted, this, [this](int idx) {
        deptDisplayLabel->setText(airportData->nameFromRowId(deptComboBox->currentData().toInt()));
    });
    connect(destComboBox, &QComboBox::highlighted, this,
            [this](int idx) { destDisplayLabel->setText(airportData->nameFromRowId(idx)); });

    // Calculate Block Time when time edit is changed
    connect(timeOffEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(timeOffEdit->time(), timeOnEdit->time()));
        totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });
    connect(timeOnEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(timeOffEdit->time(), timeOnEdit->time()));
        totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });

    // Add Take Off and Landing when Pilot Flying
    connect(pilotFlyingCheckBox, &QCheckBox::checkStateChanged, this,
            &FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged);
}

void FlightLogEntryEditDialog::readSettings()
{
    const auto pilot_function = Settings::getPilotFunction();
    int index = pilotFunctionComboBox->findData(QVariant::fromValue(pilot_function));
    if (index != -1) pilotFunctionComboBox->setCurrentIndex(index);

    flightRulesComboBox->setCurrentIndex(Settings::getLogIfr());
    flightNumberLineEdit->setText(Settings::getFlightNumberPrefix());
}

// Data Collection and Submission

bool FlightLogEntryEditDialog::runSanityChecks()
{
    // make sure the pilot function and pilot names make sense
    int pic_id = picComboBox->currentData().toInt();
    int sic_id = sicComboBox->currentData().toInt();

    if (pic_id == sic_id) {
        INFO(tr("PIC and SIC names are the same."));
        return false;
    }

    QVariant v = pilotFunctionComboBox->currentData();
    if (!v.canConvert<OPL::PilotFunction>()) {
        INFO(tr("Invalid pilot function."));
        return false;
    }
    auto function = v.value<OPL::PilotFunction>();

    // if the logbook owner is entered as PIC, he must be the first pilot and vice versa
    bool ownerIsPic = (pic_id == OPL::LOGBOOK_OWNER_ID);
    bool functionIsPicCompatible =
        (function == OPL::PilotFunction::PIC || function == OPL::PilotFunction::FI);

    if (ownerIsPic != functionIsPicCompatible) {
        INFO(tr("PIC and Pilot Function are inconsistent."));
        return false;
    }
    return true;
}

bool FlightLogEntryEditDialog::addNewEntry(DbSelectionComboBox *box)
{
    if (m_addNewOffered) return false;
    m_addNewOffered = true;
    DEB << "Add new Database entry?";
    auto target = box->getCompletionTarget();

    QMessageBox::StandardButton reply;
    switch (target) {
    case DbSelectionComboBox::CompletionTarget::PilotNames:
        reply = QMessageBox::question(
            this, tr("No Pilot found"),
            tr("No pilot with name <b>%1</b> found.<br><br> "
               "If this is the first time you log a flight with this pilot, "
               "you have to add the pilot to the database first."
               "<br><br>Would you like to add a new pilot to the database?")
                .arg(box->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case DbSelectionComboBox::CompletionTarget::TailRegistrations:
        reply = QMessageBox::question(
            this, tr("No Aircraft found"),
            tr("No aircraft with registration <b>%1</b> found.<br><br>"
               "If this is the first time you log a flight with this aircraft, "
               "you have to add the registration to the database first."
               "<br><br>Would you like to add a new aircraft to the database?")
                .arg(box->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case DbSelectionComboBox::CompletionTarget::AirportCodes:
        reply = QMessageBox::question(
            this, tr("No Airport found"),
            tr("No Airport with the identifier <b>%1</b> found.<br><br>"
               "<b>Please verify the input. Airports can be entered with their ICAO or IATA "
               "code.<br><br></b>"
               "If this is the first time you log a flight to this airport, "
               "and it is not yet in the database, "
               "you have to add the airport to the database first."
               "<br><br>Would you like to add a new airport to the database?")
                .arg(box->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    default:
        reply = QMessageBox::No;
        break;
    }

    if (reply != QMessageBox::Yes) return false;

    return addNewDatabaseElement(box);
}

bool FlightLogEntryEditDialog::addNewDatabaseElement(DbSelectionComboBox *box)
{
    if (m_addNewDialogExecuted) return false;
    m_addNewDialogExecuted = true;
    auto target            = box->getCompletionTarget();
    // Create a Factory for entry edit dialogues associated with the boxes
    using FuncTable                             = QHash<DbSelectionComboBox::CompletionTarget,
                                                        std::function<std::unique_ptr<EntryEditDialog>(DbSelectionComboBox *)>>;
    static const FuncTable entry_edit_dialogues = {
        {DbSelectionComboBox::CompletionTarget::TailRegistrations,
         [this](DbSelectionComboBox *box) {
             return std::make_unique<TailEntryEditDialog>(box->currentText(), this);
         }},
        {DbSelectionComboBox::CompletionTarget::PilotNames,
         [this](DbSelectionComboBox *box) {
             return std::make_unique<PilotEntryEditDialog>(box->currentText(), this);
         }},
        {DbSelectionComboBox::CompletionTarget::AirportCodes,
         [this](DbSelectionComboBox *box) {
             return std::make_unique<PilotEntryEditDialog>(box->currentText(), this);
         }},
    };

    if (!entry_edit_dialogues.contains(target)) {
        WARN("No edit dialogue available for target: " + QVariant::fromValue(target).toString());
        return false;
    }
    std::unique_ptr<EntryEditDialog> dlg(entry_edit_dialogues[target](box));

    bool success = false;
    success      = dlg->exec() == QDialog::Accepted;
    int row_id   = dlg->getRowId();
    if (success) {
        int idx = box->findData(row_id);
        if (idx > 0) {
            QSignalBlocker b(box);
            DEB << "Setting box to index: " << row_id;
            box->setCurrentIndex(idx);
        }
    }

    return success;
}

// Slots

void FlightLogEntryEditDialog::on_selectionComboBox_unkownValueEntered(DbSelectionComboBox *caller)
{
    DEB << "Unknown Value entered...";
    auto debounce = [this]() {
        QTimer::singleShot(1000, this, [this]() { m_addNewOffered = false; });
        QTimer::singleShot(1000, this, [this]() { m_addNewDialogExecuted = false; });
    };

    if (m_addNewOffered || m_addNewDialogExecuted) return;

    if (addNewEntry(caller)) addNewDatabaseElement(caller);
    debounce();
}

void FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state)
{
    switch (state) {
    case Qt::Checked:
        takeOffCountSpinBox->setValue(1);
        landingCountSpinBox->setValue(1);
        break;
    case Qt::Unchecked:
        takeOffCountSpinBox->setValue(0);
        landingCountSpinBox->setValue(0);
        break;
    default:
        break;
    }
}

void FlightLogEntryEditDialog::on_accepted()
{
    DEB << "Dialog accepted";

    if(!runSanityChecks()) return;

    auto data = collectFlightDataFromUi();
    if (data.validate()) {
        if (DB->commit(data)) {
            QDialog::accept();
            return;
        }
        else {
            WARN(tr("Unable to submit flight. The following error has ocurred:<br><br>%1")
                     .arg(DB->lastError.text()));
            return;
        }
    }
    else {
        auto warn_string =
            tr("Unable to submit flight. The following error(s) have ocurred:<br><br>");
        for (const auto &s : data.errors()) {
            warn_string.append(s);
            warn_string.append("<br>");
        }
        WARN(warn_string);
    }
}

FlightDataBuilder FlightLogEntryEditDialog::collectFlightDataFromUi()
{
    FlightDataBuilder builder;

    // collect data
    int date_jd     = dateEdit->date().toJulianDay();
    int dept_id     = deptComboBox->currentData().toInt();
    int dest_id     = destComboBox->currentData().toInt();
    int time_off_ms = timeOffEdit->time().msecsSinceStartOfDay();
    int time_on_ms  = timeOnEdit->time().msecsSinceStartOfDay();
    int pic_id      = picComboBox->currentData().toInt();
    int tail_id     = registrationComboBox->currentData().toInt();

    // add mandatory data
    builder.addDate(date_jd);
    builder.addDepartureLocation(dept_id);
    builder.addDestinationLocation(dest_id);
    builder.addTimeOffBlocks(time_off_ms);
    builder.addTimeOnBlocks(time_on_ms);
    builder.addPic(pic_id);
    builder.addTail(tail_id);

    // add optional data
    const QString remarks = remarksTextEdit->toPlainText();
    if (!remarks.isEmpty()) builder.addRemarks(remarks);
    if (!sicComboBox->currentText().isEmpty()) builder.addSecondPilot(sicComboBox->currentData().toInt());

    // movements
    if (takeOffCountSpinBox->value() > 0) {
        bool is_night   = NightTime::isNight(dept_id, date_jd, time_off_ms, m_night_angle);
        bool is_landing = false;
        builder.addMovement(dept_id, is_landing, is_night);
    }
    if (landingCountSpinBox->value() > 0) {
        bool is_night   = NightTime::isNight(dest_id, date_jd, time_on_ms, m_night_angle);
        bool is_landing = true;
        builder.addMovement(dest_id, is_landing, is_night);
    }

    // Calculate automatic segments
    int duration_ms = Time::blockTimeMs(time_off_ms, time_on_ms);
    const auto route      = GreatCircleTrack::greatCircleTrack(
        airportGeoData->coordinates(dept_id), airportGeoData->coordinates(dest_id), duration_ms);
    const auto day_night_segments = NightTime::nightTimeForRoute(route, dateEdit->date(), time_off_ms);
    const auto segment_data = FlightSegmentBuilder::fromNightTime(day_night_segments);

    builder.addSegmentData(segment_data);
    // approach
    // builder.addApproach(airport_id, approach_type);
    //
    return builder;
}

} // namespace OPL
