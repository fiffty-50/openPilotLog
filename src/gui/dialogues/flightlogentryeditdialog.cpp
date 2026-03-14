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
#include "src/database/cache/airportgeographicalinfo.h"
#include "src/database/cache/airportinfo.h"
#include "src/database/database.h"
#include "src/database/entries/flightdata.h"
#include "src/gui/comboboxes/dbselectioncombobox.h"
#include "src/gui/dialogues/airportentryeditdialog.h"
#include "src/gui/dialogues/entryeditdialog.h"
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
#include <optional>
#include <qobject.h>

namespace OPL {

FlightLogEntryEditDialog::FlightLogEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent), m_dateFormatString(Settings::getDateFormatString()),
      m_timeFormatString(Settings::getTimeFormatString()), m_night_angle(Settings::getNightAngle())
{
    init();
}

void FlightLogEntryEditDialog::init()
{
    ui = new Ui::FlightEntryEditUi();
    ui->setupUi(this);

    setTabOrder({ui->dateLabel, ui->dateEdit, ui->deptComboBox, ui->destComboBox,
                 ui->timeOffEdit, ui->timeOnEdit, ui->pilotFunctionComboBox,
                 ui->flightRulesComboBox, ui->registrationComboBox, ui->picComboBox,
                 ui->sicComboBox, ui->flightNumberLineEdit, ui->pilotFlyingCheckBox,
                 ui->takeOffCountSpinBox, ui->landingCountSpinBox, ui->buttonBox});

    setupValidationAndCompletion();
    setupSlots();
    readSettings();
    ui->dateEdit->setFocus();
}

// Entry Edit Dialog Interface

void FlightLogEntryEditDialog::loadEntry(int event_row_id)
{
    // Try to load the flight data
    if (auto flightOpt = OPL::FlightData::getFlightData(event_row_id); flightOpt) {
        m_eventId = event_row_id;
        m_flightId = flightOpt->flightEntry()->getRowId();

        const auto &flight_data  = *flightOpt;
        const auto &log_entry    = *flight_data.logEntry();
        const auto &flight_entry = *flight_data.flightEntry();

        DEB << "Log Entry:" << log_entry;
        DEB << "Flight Entry:" << flight_entry;
        DEB << "Movements:" << *flight_data.movementEntries();

        // Populate UI fields
        auto setBox = [](DbSelectionComboBox *box, int id) {
            int idx = box->findData(id);
            if (idx > -1) {
                box->setCurrentIndex(idx);
            }
        };
        setBox(ui->deptComboBox, flight_entry.getDepartureId());
        setBox(ui->destComboBox, flight_entry.getDestinationId());
        setBox(ui->registrationComboBox, flight_entry.getTailId());
        setBox(ui->picComboBox, flight_entry.getPicId());
        setBox(ui->sicComboBox, flight_entry.getSecondPilotId());

        ui->dateEdit->setDate(log_entry.getDate());
        ui->timeOffEdit->setTime(
            QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOffBlocksMs()));
        ui->timeOnEdit->setTime(QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOnBlocksMs()));
        ui->flightNumberLineEdit->setText(flight_entry.getFlightNumber());
        ui->remarksTextEdit->setPlainText(log_entry.getRemarks());

        // Movements
        ui->takeOffCountSpinBox->setValue(flight_data.getTakeOffCount());
        ui->landingCountSpinBox->setValue(flight_data.getLandingCount());

        // Segment Data
        {
            const QSignalBlocker b(ui->pilotFlyingCheckBox);
            ui->pilotFlyingCheckBox->setChecked(flight_data.isPilotFlying());
        }
        ui->flightRulesComboBox->setCurrentIndex(flight_data.isIfr());
        ui->pilotFunctionComboBox->setCurrentText(flight_data.pilotFunction());
    }
    else {
        // Flight not found
        WARN("Unable to load Flight with Log Event Id: " + QString::number(event_row_id));
    }
}

bool FlightLogEntryEditDialog::deleteEntry(int row_id) { return false; }

void FlightLogEntryEditDialog::reset()
{
    m_rowId = OPL::NEW_ROW_ID;
    ui->timeOffEdit->setTime(QTime::fromMSecsSinceStartOfDay(0));
    ui->timeOnEdit->setTime(QTime::fromMSecsSinceStartOfDay(0));
    ui->deptComboBox->setCurrentText({});
    ui->destComboBox->setCurrentText({});
    ui->registrationComboBox->setCurrentText({});
    ui->picComboBox->setCurrentText({});
    ui->sicComboBox->setCurrentText({});
    ui->flightNumberLineEdit->setText({});
    ui->pilotFlyingCheckBox->setCheckState(Qt::Unchecked);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->pilotFunctionComboBox->setCurrentIndex(0);
    ui->flightRulesComboBox->setCurrentIndex(0);
    ui->takeOffCountSpinBox->setValue(0);
    ui->landingCountSpinBox->setValue(0);
    ui->remarksTextEdit->setPlainText({});
}

// Dialog Setup

void FlightLogEntryEditDialog::setupValidationAndCompletion()
{
    // Setup Widegts
    ui->dateEdit->setDisplayFormat(m_dateFormatString);
    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setTimeZone(QTimeZone::UTC);
    ui->dateEdit->setMinimumDate(OPL::Date::minimumDate());
    ui->dateEdit->setMaximumDate(OPL::Date::maximumDate());
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateDisplayLabel->setMinimumWidth(200);
    ui->dateDisplayLabel->setMaximumWidth(200);
    QFont f = ui->dateDisplayLabel->font();
    f.setItalic(true);
    ui->dateDisplayLabel->setFont(f);
    ui->deptDisplayLabel->setFont(f);
    ui->destDisplayLabel->setFont(f);

    ui->timeOffEdit->setDisplayFormat(m_timeFormatString);
    ui->timeOffEdit->setTimeZone(QTimeZone::UTC);
    ui->timeOnEdit->setDisplayFormat(m_timeFormatString);
    ui->timeOnEdit->setTimeZone(QTimeZone::UTC);

    ui->takeOffCountSpinBox->setMinimum(0);
    ui->landingCountSpinBox->setMinimum(0);

    OPL::GLOBALS->loadPilotFunctions(ui->pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(ui->flightRulesComboBox);

    // Setup Basic Input Validation for the airport code entries
    auto dept_val = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, ui->deptComboBox);
    ui->deptComboBox->setValidator(dept_val);
    auto dest_val = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, ui->destComboBox);
    ui->destComboBox->setValidator(dest_val);
}

void FlightLogEntryEditDialog::setupSlots()
{
    // Button Box
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
            &FlightLogEntryEditDialog::on_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // Registration
    connect(ui->registrationComboBox, &DbSelectionComboBox::newValueEntered, this,
            &::OPL::FlightLogEntryEditDialog::on_unknown_value_entered);
    // Pilot Name
    connect(ui->picComboBox, &DbSelectionComboBox::newValueEntered, this,
            &::OPL::FlightLogEntryEditDialog::on_unknown_value_entered);
    connect(ui->sicComboBox, &DbSelectionComboBox::newValueEntered, this,
            &::OPL::FlightLogEntryEditDialog::on_unknown_value_entered);

    // Location Line Edits
    connect(ui->deptComboBox, &DbSelectionComboBox::newValueEntered, this,
            &::OPL::FlightLogEntryEditDialog::on_unknown_value_entered);
    connect(ui->destComboBox, &DbSelectionComboBox::newValueEntered, this,
            &::OPL::FlightLogEntryEditDialog::on_unknown_value_entered);
    // Display the airport name when the combobox is edited or the popup completer is used
    connect(ui->deptComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
        ui->deptDisplayLabel->setText(
            airportData->nameFromRowId(ui->deptComboBox->currentData().toInt()));
    });
    connect(ui->destComboBox->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
        ui->destDisplayLabel->setText(
            airportData->nameFromRowId(ui->destComboBox->currentData().toInt()));
    });
    connect(ui->deptComboBox, &QComboBox::highlighted, this, [this](int idx) {
        ui->deptDisplayLabel->setText(
            airportData->nameFromRowId(ui->deptComboBox->currentData().toInt()));
    });
    connect(ui->destComboBox, &QComboBox::highlighted, this,
            [this](int idx) { ui->destDisplayLabel->setText(airportData->nameFromRowId(idx)); });

    // Calculate Block Time when time edit is changed
    connect(ui->timeOffEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(ui->timeOffEdit->time(), ui->timeOnEdit->time()));
        ui->totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });
    connect(ui->timeOnEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(ui->timeOffEdit->time(), ui->timeOnEdit->time()));
        ui->totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });

    // Add Take Off and Landing when Pilot Flying
    connect(ui->pilotFlyingCheckBox, &QCheckBox::checkStateChanged, this,
            &FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged);
}

void FlightLogEntryEditDialog::readSettings()
{
    const auto pilot_function = Settings::getPilotFunction();
    int index = ui->pilotFunctionComboBox->findData(QVariant::fromValue(pilot_function));
    if (index != -1) ui->pilotFunctionComboBox->setCurrentIndex(index);

    ui->flightRulesComboBox->setCurrentIndex(Settings::getLogIfr());
    ui->flightNumberLineEdit->setText(Settings::getFlightNumberPrefix());
}

void FlightLogEntryEditDialog::on_unknown_value_entered(DbSelectionComboBox *box)
{
    if (offerToAddNewDatabaseElement(box)) {
        auto new_entry = addNewEntry(box);
        if (new_entry) {
            setComboBoxValue(box, new_entry.value());
        }
    }
}

void FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state)
{
    switch (state) {
    case Qt::Checked:
        ui->takeOffCountSpinBox->setValue(1);
        ui->landingCountSpinBox->setValue(1);
        break;
    case Qt::Unchecked:
        ui->takeOffCountSpinBox->setValue(0);
        ui->landingCountSpinBox->setValue(0);
        break;
    default:
        break;
    }
}

void FlightLogEntryEditDialog::on_accepted()
{
    DEB << "Dialog accepted";

    auto data = collectFlightDataFromUi();
    if (data.validate()) {
        if (DB->commit(data)) {
            QDialog::accept();
            return;
        }
        else {
            WARN(tr("Unable to submit flight. The following error has ocurred:<br><br>%1")
                     .arg(DB->lastErrorText()));
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

// Data Collection and Submission

FlightDataBuilder FlightLogEntryEditDialog::collectFlightDataFromUi()
{
    FlightDataBuilder builder;
    if (m_eventId > 0) {
        LOG << "Building from existing entry";
        builder.setEventId(m_eventId);
        builder.setFlightId(m_flightId);
    } else {
        LOG << "Bulding new entry";
    }

    // collect data
    int date_jd     = ui->dateEdit->date().toJulianDay();
    int dept_id     = ui->deptComboBox->currentData().toInt();
    int dest_id     = ui->destComboBox->currentData().toInt();
    int time_off_ms = ui->timeOffEdit->time().msecsSinceStartOfDay();
    int time_on_ms  = ui->timeOnEdit->time().msecsSinceStartOfDay();
    int pic_id      = ui->picComboBox->currentData().toInt();
    int tail_id     = ui->registrationComboBox->currentData().toInt();

    // add mandatory data
    builder.addDate(date_jd);
    builder.addDepartureLocation(dept_id);
    builder.addDestinationLocation(dest_id);
    builder.addTimeOffBlocks(time_off_ms);
    builder.addTimeOnBlocks(time_on_ms);
    builder.addPic(pic_id);
    builder.addTail(tail_id);

    // add optional data
    const QString remarks = ui->remarksTextEdit->toPlainText();
    if (!remarks.isEmpty()) builder.addRemarks(remarks);
    const QString flight_number = ui->flightNumberLineEdit->text();
    if (!flight_number.isEmpty()) builder.addFlightNumber(flight_number);
    if (!ui->sicComboBox->currentText().isEmpty())
        builder.addSecondPilot(ui->sicComboBox->currentData().toInt());

    const QVariant v = ui->pilotFunctionComboBox->currentData();
    const auto function = v.value<OPL::PilotFunction>();
    builder.addPilotFunction(function);

    // movements
    if (ui->takeOffCountSpinBox->value() > 0) {
        bool is_night   = NightTime::isNight(dept_id, date_jd, time_off_ms, m_night_angle);
        bool is_landing = false;
        builder.addMovement(dept_id, is_landing, is_night);
    }
    if (ui->landingCountSpinBox->value() > 0) {
        bool is_night   = NightTime::isNight(dest_id, date_jd, time_on_ms, m_night_angle);
        bool is_landing = true;
        builder.addMovement(dest_id, is_landing, is_night);
    }

    // Calculate automatic segments
    int duration_ms  = Time::blockTimeMs(time_off_ms, time_on_ms);
    const auto route = GreatCircleTrack::greatCircleTrack(
        airportGeoData->coordinates(dept_id), airportGeoData->coordinates(dest_id), duration_ms);
    const auto day_night_segments =
        NightTime::nightTimeForRoute(route, ui->dateEdit->date(), time_off_ms);

    // Build optionals
    bool is_ifr            = ui->flightRulesComboBox->currentData().toBool();
    bool is_pilot_flying   = ui->pilotFlyingCheckBox->isChecked();
    QString pilot_function = ui->pilotFunctionComboBox->currentText();
    FlightSegmentEntry::Optionals opts;
    opts.is_ifr          = is_ifr;
    opts.is_pilot_flying = is_pilot_flying;
    opts.pilot_function  = pilot_function;

    const auto segment_data = FlightSegmentBuilder::fromNightTime(day_night_segments, opts);

    builder.addSegmentData(segment_data);
    // approach
    // builder.addApproach(airport_id, approach_type);
    //
    return builder;
}

bool FlightLogEntryEditDialog::offerToAddNewDatabaseElement(const DbSelectionComboBox *box)
{
    auto table = box->table();

    QMessageBox::StandardButton reply;
    switch (table) {
    case OPL::DbTable::Pilots:
        reply = QMessageBox::question(
            this, tr("No Pilot found"),
            tr("No pilot with name <b>%1</b> found.<br><br> "
               "If this is the first time you log a flight with this pilot, "
               "you have to add the pilot to the database first."
               "<br><br>Would you like to add a new pilot to the database?")
                .arg(box->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::AircraftTails:
        reply = QMessageBox::question(
            this, tr("No Aircraft found"),
            tr("No aircraft with registration <b>%1</b> found.<br><br>"
               "If this is the first time you log a flight with this aircraft, "
               "you have to add the registration to the database first."
               "<br><br>Would you like to add a new aircraft to the database?")
                .arg(box->currentText()),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::AirportCodes:
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
        return false;
        break;
    }

    return reply == QMessageBox::Yes;
}

bool FlightLogEntryEditDialog::setComboBoxValue(DbSelectionComboBox *box, int row_id)
{
    int idx = box->findData(row_id);
    if (idx > -1) {
        box->setCurrentIndex(idx);
        box->verifyContent();
        DEB << "Setting new value: " << box->itemText(idx) << " (index: " << idx
            << ", row_id: " << row_id << ")";
        return true;
    }

    DEB << "No data found for row id: " << row_id;
    for (int i = 0; i < box->count(); ++i) {
        DEB << box->itemText(i) << '/' << box->itemData(i);
    }
    return false;
}
std::optional<int> FlightLogEntryEditDialog::addNewEntry(const DbSelectionComboBox *box)
{
    auto dialog = getEntryEditDialog(box);
    if (!dialog) return std::nullopt;
    if (dialog->exec() == QDialog::Accepted) {
        // success
        return dialog->getRowId();
    }
    return std::nullopt;
}

EntryEditDialog *FlightLogEntryEditDialog::getEntryEditDialog(const DbSelectionComboBox *box)
{
    auto target          = box->table();
    EntryEditDialog *dlg = nullptr;

    switch (target) {
    case OPL::DbTable::AircraftTails:
        dlg = new TailEntryEditDialog(box->currentText(), this);
        break;
    case OPL::DbTable::Pilots:
        dlg = new PilotEntryEditDialog(box->currentText(), this);
        break;
    case OPL::DbTable::AirportCodes:
        dlg = new AirportEntryEditDialog(this);
        break;
    default:
        LOG << "No edit dialogue available for target: " + QVariant::fromValue(target).toString();
        return nullptr;
    }

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}

} // namespace OPL
