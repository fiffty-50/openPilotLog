/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "newflightdialog.h"
#include "src/classes/time.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/airportinput.h"
#include "src/gui/verification/completerprovider.h"
#include "src/gui/verification/pilotinput.h"
#include "src/gui/verification/tailinput.h"
#include "src/gui/verification/timeinput.h"
#include "ui_newflightdialog.h"
#include "src/opl.h"
#include "src/functions/datetime.h"
#include "src/classes/settings.h"
#include "src/functions/calc.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include <QDateTime>
#include <QCompleter>
#include <QKeyEvent>


NewFlightDialog::NewFlightDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::NewFlightDialog)
{
    init();
    // Set up UI (New Flight)
    LOG << Settings::read(Settings::FlightLogging::Function);
    if(Settings::read(Settings::FlightLogging::Function).toInt() == static_cast<int>(OPL::PilotFunction::PIC)){
        ui->picNameLineEdit->setText(self);
        ui->functionComboBox->setCurrentIndex(0);
        emit ui->picNameLineEdit->editingFinished();
    }
    if (Settings::read(Settings::FlightLogging::Function).toInt() == static_cast<int>(OPL::PilotFunction::SIC)) {
        ui->sicNameLineEdit->setText(self);
        ui->functionComboBox->setCurrentIndex(2);
        emit ui->sicNameLineEdit->editingFinished();
    }
    ui->doftLineEdit->setText(OPL::DateTime::currentDate());
    emit ui->doftLineEdit->editingFinished();
}

NewFlightDialog::NewFlightDialog(int row_id, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::NewFlightDialog)
{
    init();
    flightEntry = DB->getFlightEntry(row_id);
    fillWithEntryData();
}

NewFlightDialog::~NewFlightDialog()
{
    delete ui;
}

void NewFlightDialog::init()
{
    // Setup UI
    ui->setupUi(this);
    // Initialise line edit lists
    auto time_line_edits = new QList<QLineEdit*>{ui->tofbTimeLineEdit, ui->tonbTimeLineEdit};
    timeLineEdits = time_line_edits;
    auto location_line_edits = new QList<QLineEdit*>{ui->deptLocationLineEdit, ui->destLocationLineEdit};
    locationLineEdits = location_line_edits;
    auto pilot_name_line_edits = new QList<QLineEdit*>{ui->picNameLineEdit, ui->sicNameLineEdit, ui->thirdPilotNameLineEdit};
    pilotNameLineEdits = pilot_name_line_edits;
    auto mandatory_line_edits = new QList<QLineEdit*>{ui->doftLineEdit, ui->deptLocationLineEdit, ui->destLocationLineEdit,
            ui->tofbTimeLineEdit, ui->tonbTimeLineEdit,
            ui->picNameLineEdit, ui->acftLineEdit};
    // {doft = 0, dept = 1, dest = 2, tofb = 3, tonb = 4, pic = 5, acft = 6}
    mandatoryLineEdits = mandatory_line_edits;

    for (const auto& line_edit : *mandatoryLineEdits)
        line_edit->installEventFilter(this);
    // Approach Combo Box and Function Combo Box
    OPL::GLOBALS->loadApproachTypes(ui->approachComboBox);
    OPL::GLOBALS->loadPilotFunctios(ui->functionComboBox);

    setupRawInputValidation();
    setupSignalsAndSlots();
    readSettings();
}

/*!
 * \brief NewFlightDialog::setupRawInputValidation outfits the line edits with QRegularExpresionValidators and QCompleters
 */
void NewFlightDialog::setupRawInputValidation()
{
    // Time Line Edits
    for (const auto& line_edit : *timeLineEdits) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"), line_edit);
        line_edit->setValidator(validator);
    }
    // Location Line Edits
    for (const auto& line_edit : *locationLineEdits) {
        auto validator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]{1,4}"), line_edit);
        line_edit->setValidator(validator);
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
    }
    // Name Line Edits
    for (const auto& line_edit : *pilotNameLineEdits) {
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Pilots));
    }
    // Acft Line Edit
    ui->acftLineEdit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Tails));
}

void NewFlightDialog::setupSignalsAndSlots()
{
    for (const auto& line_edit : *timeLineEdits)
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewFlightDialog::onTimeLineEdit_editingFinished);
    // Change text to upper case for location and acft line edits
    QObject::connect(ui->acftLineEdit, &QLineEdit::textChanged,
                     this, &NewFlightDialog::toUpper);
    for (const auto& line_edit : *locationLineEdits) {
        QObject::connect(line_edit, &QLineEdit::textChanged,
                         this, &NewFlightDialog::toUpper);
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewFlightDialog::onLocationLineEdit_editingFinished);
    }
    for (const auto& line_edit : *pilotNameLineEdits)
        QObject::connect(line_edit, &QLineEdit::editingFinished,
                         this, &NewFlightDialog::onPilotNameLineEdit_editingFinshed);
}

/*!
 * \brief NewFlightDialog::eventFilter invalidates mandatory line edits on focus in.
 * \details Some of the QLineEdits have validators set that provide raw input validation. These validators have the side effect
 * that if an input does not meet the raw input validation criteria, onEditingFinished() is not emitted when the line edit loses
 * focus. This could lead to a line edit that previously had good input to be changed to bad input without the validation bit
 * in validationState being unset, because the second step of input validation is only triggered when editingFinished() is emitted.
 */
bool NewFlightDialog::eventFilter(QObject *object, QEvent *event)
{
    auto line_edit = qobject_cast<QLineEdit*>(object);
    if (line_edit != nullptr) {
        if (mandatoryLineEdits->contains(line_edit) && event->type() == QEvent::FocusIn) {
            // set verification bit to false when entering a mandatory line edit
            validationState.invalidate(mandatoryLineEdits->indexOf(line_edit));
            return false;
        }
    }
    return false;
}

/*!
 * \brief NewFlightDialog::readSettings Reads user-defined settings from an INI file.
 */
void NewFlightDialog::readSettings()
{
    ui->functionComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::Function).toInt());
    ui->approachComboBox->setCurrentIndex(Settings::read(Settings::FlightLogging::Approach).toInt());
    ui->pilotFlyingCheckBox->setChecked(Settings::read(Settings::FlightLogging::PilotFlying).toBool());
    ui->ifrCheckBox->setChecked(Settings::read(Settings::FlightLogging::LogIFR).toBool());
    ui->flightNumberLineEdit->setText(Settings::read(Settings::FlightLogging::FlightNumberPrefix).toString());
}

/*!
 * \brief NewFlightDialog::fillWithEntryData Takes an existing flight entry and fills the UI with its data
 * to enable editing an existing flight.
 */
void NewFlightDialog::fillWithEntryData()
{
    DEB << "Restoring Flight: ";
    DEB << flightEntry;

    const auto &flight_data = flightEntry.getData();

    // Date of Flight
    ui->doftLineEdit->setText(flight_data.value(OPL::Db::FLIGHTS_DOFT).toString());
    // Location
    ui->deptLocationLineEdit->setText(flight_data.value(OPL::Db::FLIGHTS_DEPT).toString());
    ui->destLocationLineEdit->setText(flight_data.value(OPL::Db::FLIGHTS_DEST).toString());
    // Times
    ui->tofbTimeLineEdit->setText(OPL::Time(flight_data.value(OPL::Db::FLIGHTS_TOFB).toInt()).toString());
    ui->tonbTimeLineEdit->setText(OPL::Time(flight_data.value(OPL::Db::FLIGHTS_TONB).toInt()).toString());
    ui->acftLineEdit->setText(DBCache->getTailsMap().value(flight_data.value(OPL::Db::FLIGHTS_ACFT).toInt()));
    ui->picNameLineEdit->setText(DBCache->getPilotNamesMap().value(flight_data.value(OPL::Db::FLIGHTS_PIC).toInt()));
    ui->sicNameLineEdit->setText(DBCache->getPilotNamesMap().value(flight_data.value(OPL::Db::FLIGHTS_SECONDPILOT).toInt()));
    ui->thirdPilotNameLineEdit->setText(DBCache->getPilotNamesMap().value(flight_data.value(OPL::Db::FLIGHTS_THIRDPILOT).toInt()));

    //Function
    const QHash<int, QString> functions = {
        {0, OPL::Db::FLIGHTS_TPIC},
        {1, OPL::Db::FLIGHTS_TPICUS},
        {2, OPL::Db::FLIGHTS_TSIC},
        {3, OPL::Db::FLIGHTS_TDUAL},
        {4, OPL::Db::FLIGHTS_TFI},
    };
    for (int i = 0; i < 5; i++) { // QHash::iterator not guarenteed to be in ordetr
        if(flight_data.value(functions.value(i)).toInt() != 0)
            ui->functionComboBox->setCurrentIndex(i);
    }
    // Approach ComboBox
    const QString& app = flight_data.value(OPL::Db::FLIGHTS_APPROACHTYPE).toString();
    if(app != QString()){
        ui->approachComboBox->setCurrentText(app);
    }
    // Task
    bool PF = flight_data.value(OPL::Db::FLIGHTS_PILOTFLYING).toBool();
    ui->pilotFlyingCheckBox->setChecked(PF);
    // Flight Rules
    bool time_ifr = flight_data.value(OPL::Db::FLIGHTS_TIFR).toBool();
    ui->ifrCheckBox->setChecked(time_ifr);
    // Take-Off and Landing
    int TO =  flight_data.value(OPL::Db::FLIGHTS_TODAY).toInt()
            + flight_data.value(OPL::Db::FLIGHTS_TONIGHT).toInt();
    int LDG = flight_data.value(OPL::Db::FLIGHTS_LDGDAY).toInt()
            + flight_data.value(OPL::Db::FLIGHTS_LDGNIGHT).toInt();
    ui->takeOffSpinBox->setValue(TO);
    ui->landingSpinBox->setValue(LDG);
    // Remarks
    ui->remarksLineEdit->setText(flight_data.value(OPL::Db::FLIGHTS_REMARKS).toString());
    // Flight Number
    ui->flightNumberLineEdit->setText(flight_data.value(OPL::Db::FLIGHTS_FLIGHTNUMBER).toString());

    for(const auto &line_edit : *mandatoryLineEdits)
        emit line_edit->editingFinished();
}

bool NewFlightDialog::verifyUserInput(QLineEdit *line_edit, const UserInput &input)
{
    DEB << "Verifying user input: " << line_edit->text();

    if(!input.isValid()) {
        QString fixed = input.fixup();
        if(fixed == QString()) {
            onBadInputReceived(line_edit);
            return false;
        } else {
            line_edit->setText(fixed);
            onGoodInputReceived(line_edit);
            return true;
        }
    }
    onGoodInputReceived(line_edit);
    return true;
}

void NewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Good input received - " << line_edit->text();
    line_edit->setStyleSheet(QString());

    if (mandatoryLineEdits->contains(line_edit))
        validationState.validate(mandatoryLineEdits->indexOf(line_edit));

    if (validationState.timesValid()) {
        updateBlockTimeLabel();
        if (validationState.nightDataValid())
            updateNightCheckBoxes();
    }
}

void NewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Bad input received - " << line_edit->text();
    line_edit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    line_edit->setText(QString());

    if (mandatoryLineEdits->contains(line_edit))
        validationState.invalidate(mandatoryLineEdits->indexOf(line_edit));

    validationState.printValidationStatus();
}

void NewFlightDialog::updateBlockTimeLabel()
{
    const OPL::Time tofb = OPL::Time::fromString(ui->tofbTimeLineEdit->text());
    const OPL::Time tonb = OPL::Time::fromString(ui->tonbTimeLineEdit->text());
    const OPL::Time tblk = OPL::Time::blockTime(tofb, tonb);

    ui->tblkDisplayLabel->setText(tblk.toString());
}

/*!
 * \brief NewFlightDialog::addNewTail If the user input is not in the aircraftList, the user
 * is prompted if he wants to add a new entry to the database
 */
bool NewFlightDialog::addNewTail(QLineEdit& parent_line_edit)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("No Aircraft found"),
                                  tr("No aircraft with this registration found.<br>"
                                     "If this is the first time you log a flight with this aircraft, "
                                     "you have to add the registration to the database first."
                                     "<br><br>Would you like to add a new aircraft to the database?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // create and open new aircraft dialog
        NewTailDialog nt(ui->acftLineEdit->text(), this);
        int ret = nt.exec();
        // update map and list, set line edit
        if (ret == QDialog::Accepted) {
            DEB << "New Tail Entry added:";
            DEB << DB->getTailEntry(DB->getLastEntry(OPL::DbTable::Tails));

            // update Line Edit with newly added tail
            parent_line_edit.setText(DBCache->getTailsMap().value(DB->getLastEntry(OPL::DbTable::Tails)));
            emit parent_line_edit.editingFinished();
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/*!
 * \brief NewFlightDialog::addNewPilot If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
bool NewFlightDialog::addNewPilot(QLineEdit& parent_line_edit)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("No Pilot found"),
                                  tr("No pilot found.<br>Please enter the Name as"
                                     "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
                                     "If this is the first time you log a flight with this pilot, "
                                     "you have to add the pilot to the database first."
                                     "<br><br>Would you like to add a new pilot to the database?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // create and open new pilot dialog
        NewPilotDialog np(this);
        int ret = np.exec();
        // update map and list, set line edit
        if (ret == QDialog::Accepted) {
            DEB << "New Pilot Entry added:";
            DEB << DB->getPilotEntry(DB->getLastEntry(OPL::DbTable::Pilots));

            // update Line Edit with newly added pilot
            parent_line_edit.setText(DBCache->getPilotNamesMap().value(DB->getLastEntry(OPL::DbTable::Pilots)));
            emit parent_line_edit.editingFinished();
            return true;
        } else {
            return false;
        }
    } else
        return false;
}

/*!
 * \brief NewFlightDialog::prepareFlightEntryData reads the user input from the UI and converts it into
 * the database format.
 * \return OPL::RowData_T a map containing a row ready for database submission
 */
OPL::RowData_T NewFlightDialog::prepareFlightEntryData()
{
    if(!validationState.allValid())
        return {};

    // prepare the entry data
    OPL::RowData_T new_data;

    // Calculate Block and Night Time
    const OPL::Time tofb = OPL::Time::fromString(ui->tofbTimeLineEdit->text());
    const OPL::Time tonb = OPL::Time::fromString(ui->tonbTimeLineEdit->text());
    const int block_minutes = OPL::Time::blockMinutes(tofb, tonb);

    QDateTime departure_date_time = OPL::DateTime::fromString(ui->doftLineEdit->text() + ui->tofbTimeLineEdit->text());
    const auto night_time_data = OPL::Calc::NightTimeValues(ui->deptLocationLineEdit->text(), ui->destLocationLineEdit->text(),
                           departure_date_time, block_minutes, Settings::read(Settings::FlightLogging::NightAngle).toInt());
    // Mandatory data
    new_data.insert(OPL::Db::FLIGHTS_DOFT, ui->doftLineEdit->text());
    new_data.insert(OPL::Db::FLIGHTS_DEPT, ui->deptLocationLineEdit->text());
    new_data.insert(OPL::Db::FLIGHTS_TOFB, tofb.toMinutes());
    new_data.insert(OPL::Db::FLIGHTS_DEST, ui->destLocationLineEdit->text());
    new_data.insert(OPL::Db::FLIGHTS_TONB, tonb.toMinutes());
    new_data.insert(OPL::Db::FLIGHTS_TBLK, block_minutes);
    // Night
    new_data.insert(OPL::Db::FLIGHTS_TNIGHT, night_time_data.nightMinutes);
    // Aircraft
    int acft_id = DBCache->getTailsMap().key(ui->acftLineEdit->text());
    new_data.insert(OPL::Db::FLIGHTS_ACFT, acft_id);
    const OPL::TailEntry acft_data = DB->getTailEntry(acft_id);
    bool multi_pilot = acft_data.getData().value(OPL::Db::TAILS_MULTIPILOT).toBool();
    bool multi_engine = acft_data.getData().value(OPL::Db::TAILS_MULTIENGINE).toBool();

    if (multi_pilot) {
        new_data.insert(OPL::Db::FLIGHTS_TMP, block_minutes);
        new_data.insert(OPL::Db::FLIGHTS_TSPSE, QString());
        new_data.insert(OPL::Db::FLIGHTS_TSPME, QString());
    } else if (!multi_pilot && !multi_engine) {
        new_data.insert(OPL::Db::FLIGHTS_TMP, QString());
        new_data.insert(OPL::Db::FLIGHTS_TSPSE, block_minutes);
        new_data.insert(OPL::Db::FLIGHTS_TSPME, QString());
    } else if (!multi_pilot && multi_engine) {
        new_data.insert(OPL::Db::FLIGHTS_TMP, QString());
        new_data.insert(OPL::Db::FLIGHTS_TSPSE, QString());
        new_data.insert(OPL::Db::FLIGHTS_TSPME, block_minutes);
    }
    // Pilots
    new_data.insert(OPL::Db::FLIGHTS_PIC, DBCache->getPilotNamesMap().key(ui->picNameLineEdit->text()));
    new_data.insert(OPL::Db::FLIGHTS_SECONDPILOT, DBCache->getPilotNamesMap().key(ui->sicNameLineEdit->text()));
    new_data.insert(OPL::Db::FLIGHTS_THIRDPILOT, DBCache->getPilotNamesMap().key(ui->thirdPilotNameLineEdit->text()));
    // IFR time
    if (ui->ifrCheckBox->isChecked()) {
        new_data.insert(OPL::Db::FLIGHTS_TIFR, block_minutes);
    } else {
        new_data.insert(OPL::Db::FLIGHTS_TIFR, QString());
    }
    // Function Times
    QStringList function_times = {
        OPL::Db::FLIGHTS_TPIC,
        OPL::Db::FLIGHTS_TPICUS,
        OPL::Db::FLIGHTS_TSIC,
        OPL::Db::FLIGHTS_TDUAL,
        OPL::Db::FLIGHTS_TFI,
    };

    // Determine function times, zero out all values except one (use QString() iso 0 for correct handling of NULL in DB
    // Log Instructor Time as PIC time as well
    const int& function_index = ui->functionComboBox->currentIndex();
    switch (function_index) {
    case 4:
        LOG << "Function FI";
        for (int i = 0; i < 5; i++){
            if(i == 0 || i == 4)
                new_data.insert(function_times[i], block_minutes);
            else
                new_data.insert(function_times[i], QString());
        }
        break;
    default:
        for (int i = 0; i < 5; i++){
            if(i == function_index)
                new_data.insert(function_times[i], block_minutes);
            else
                new_data.insert(function_times[i], QString());
        }
        break;
    }
    // Pilot flying / Pilot monitoring
    new_data.insert(OPL::Db::FLIGHTS_PILOTFLYING, ui->pilotFlyingCheckBox->isChecked());
    // Take-Off and Landing
    if (ui->toNightCheckBox->isChecked()) {
        new_data.insert(OPL::Db::FLIGHTS_TONIGHT, ui->takeOffSpinBox->value());
        new_data.insert(OPL::Db::FLIGHTS_TODAY, 0);
    } else {
        new_data.insert(OPL::Db::FLIGHTS_TONIGHT, 0);
        new_data.insert(OPL::Db::FLIGHTS_TODAY, ui->takeOffSpinBox->value());
    }
    if (ui->ldgNightCheckBox->isChecked()) {
        new_data.insert(OPL::Db::FLIGHTS_LDGNIGHT, ui->landingSpinBox->value());
        new_data.insert(OPL::Db::FLIGHTS_LDGDAY, 0);
    } else {
        new_data.insert(OPL::Db::FLIGHTS_LDGNIGHT, 0);
        new_data.insert(OPL::Db::FLIGHTS_LDGDAY, ui->landingSpinBox->value());
    }
    if (ui->approachComboBox->currentText() == CAT_3) // ILS CAT III
        new_data.insert(OPL::Db::FLIGHTS_AUTOLAND, ui->landingSpinBox->value());

    // Additional Data
    new_data.insert(OPL::Db::FLIGHTS_APPROACHTYPE, ui->approachComboBox->currentText());
    new_data.insert(OPL::Db::FLIGHTS_FLIGHTNUMBER, ui->flightNumberLineEdit->text());
    new_data.insert(OPL::Db::FLIGHTS_REMARKS, ui->remarksLineEdit->text());
    return new_data;
}

/*!
 * \brief NewFlightDialog::updateNightCheckBoxes updates the check boxes for take-off and landing
 * at night. Returns the number of minutes of night time.
 * \return
 */
void NewFlightDialog::updateNightCheckBoxes()
{
    // calculate Block Time
    const OPL::Time tofb = OPL::Time::fromString(ui->tofbTimeLineEdit->text());
    const OPL::Time tonb = OPL::Time::fromString(ui->tonbTimeLineEdit->text());
    const int block_minutes = OPL::Time::blockMinutes(tofb, tonb);

    // Calculate Night Time
    const QString dept_date = (ui->doftLineEdit->text() + ui->tofbTimeLineEdit->text());
    const auto dept_date_time = OPL::DateTime::fromString(dept_date);
    const int night_angle = Settings::read(Settings::FlightLogging::NightAngle).toInt();
    const auto night_values = OPL::Calc::NightTimeValues(
                ui->deptLocationLineEdit->text(),
                ui->destLocationLineEdit->text(),
                dept_date_time,
                block_minutes,
                night_angle);
    // set check boxes
    if (night_values.takeOffNight)
        ui->toNightCheckBox->setCheckState(Qt::Checked);
    else
        ui->toNightCheckBox->setCheckState(Qt::Unchecked);

    if (night_values.landingNight)
        ui->ldgNightCheckBox->setCheckState(Qt::Checked);
    else
        ui->ldgNightCheckBox->setCheckState(Qt::Unchecked);
}

/*!
 * \brief NewFlightDialog::toUpper - changes inserted text to upper case for IATA/ICAO airport codes and registrations.
 */
void NewFlightDialog::toUpper(const QString &text)
{
    const auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}

void NewFlightDialog::onTimeLineEdit_editingFinished()
{
    auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    verifyUserInput(line_edit, TimeInput(line_edit->text()));
}

void NewFlightDialog::onPilotNameLineEdit_editingFinshed()
{
    auto line_edit = this->findChild<QLineEdit*>(sender()->objectName());
    if(!verifyUserInput(line_edit, PilotInput(line_edit->text()))) {
        if(!addNewPilot(*line_edit))
            onBadInputReceived(line_edit);
    }
}

void NewFlightDialog::onLocationLineEdit_editingFinished()
{
    const QString& line_edit_name = sender()->objectName();
    const auto line_edit = this->findChild<QLineEdit*>(line_edit_name);
    QLabel* name_label;
    if (line_edit_name.contains(QLatin1String("dept")))
        name_label = ui->deptNameLabel;
    else
        name_label = ui->destNameLabel;


    if(verifyUserInput(line_edit, AirportInput(line_edit->text())) ) {
        // Match ICAO code with Airport Name and display on label
        name_label->setText(DBCache->getAirportsMapNames().value(
                                DBCache->getAirportsMapICAO().key(
                                    line_edit->text())));
    } else
        name_label->setText("Unknown Airport");
}

void NewFlightDialog::on_acftLineEdit_editingFinished()
{
    const auto line_edit = ui->acftLineEdit;

    if(!verifyUserInput(line_edit, TailInput(line_edit->text())))
        if(!addNewTail(*line_edit))
            onBadInputReceived(line_edit);

    const auto space = QLatin1Char(' ');
    if(line_edit->text().contains(space))
        line_edit->setText(line_edit->text().split(space)[0]); // strip out autocomplete suggestion
}

void NewFlightDialog::on_doftLineEdit_editingFinished()
{
    const auto line_edit = ui->doftLineEdit;
    auto text = ui->doftLineEdit->text();
    auto label = ui->doftDisplayLabel;

    TODO << "Non-default Date formats not implemented yet.";
    OPL::DateFormat date_format = OPL::DateFormat::ISODate;
    auto date = OPL::DateTime::parseInput(text, date_format);
    if (date.isValid()) {
        label->setText(date.toString(Qt::TextDate));
        line_edit->setText(OPL::DateTime::dateToString(date, date_format));
        onGoodInputReceived(line_edit);
        return;
    }

    label->setText(tr("Invalid Date."));
    onBadInputReceived(line_edit);
}

void NewFlightDialog::on_pilotFlyingCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::CheckState::Checked) {
        ui->takeOffSpinBox->setValue(1);
        ui->landingSpinBox->setValue(1);
    } else {
        ui->takeOffSpinBox->setValue(0);
        ui->landingSpinBox->setValue(0);
    }
}

void NewFlightDialog::on_approachComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == CAT_3)
        ui->remarksLineEdit->setText(QStringLiteral("Autoland"));
    else if (arg1 == QLatin1String("OTHER"))
        INFO(tr("Please specify the approach type in the remarks field."));
}

/*!
 * \brief NewFlightDialog::on_functionComboBox_currentIndexChanged
 * If the Function Combo Box is selected to be either PIC or SIC, fill the corresponding
 * nameLineEdit with the logbook owner's name, then check for duplication.
 */
void NewFlightDialog::on_functionComboBox_currentIndexChanged(int index)
{
    if (index == static_cast<int>(OPL::PilotFunction::PIC)) {
        ui->picNameLineEdit->setText(self);
        emit ui->picNameLineEdit->editingFinished();
        if (DBCache->getPilotNamesMap().key(ui->picNameLineEdit->text())
         == DBCache->getPilotNamesMap().key(ui->sicNameLineEdit->text()))
                ui->sicNameLineEdit->setText(QString());
    } else if (index == static_cast<int>(OPL::PilotFunction::SIC)) {
        ui->sicNameLineEdit->setText(self);
        emit ui->sicNameLineEdit->editingFinished();
        if (DBCache->getPilotNamesMap().key(ui->picNameLineEdit->text())
         == DBCache->getPilotNamesMap().key(ui->sicNameLineEdit->text()))
            ui->picNameLineEdit->setText(QString());
    }
}

/*!
 * \brief NewFlightDialog::checkPilotFunctionsValid checks if there are incompatible selections made on Pilot Function.
 * \details Checks for 2 cases in which there might be a discrepancy between the PilotNameLineEdit and the functionComboBox:
 * - If the pilotNameLineEdit's value is self, but the functionComboBox has been manually selected to be different from either
 * PIC or FI
 * - If the functionComboBox has been set to PIC but the pilotNameLineEdit is not self
 * \param error_msg - the error string displayed to the user
 * \return
 */
bool NewFlightDialog::checkPilotFunctionsValid()
{
    int pic_id = DBCache->getPilotNamesMap().key(ui->picNameLineEdit->text());
    int function_index = ui->functionComboBox->currentIndex();

    if (pic_id == 1) {
        if (!(function_index == static_cast<int>(OPL::PilotFunction::PIC) || function_index == static_cast<int>(OPL::PilotFunction::FI))) {
            INFO(tr("The PIC is set to %1 but the Pilot Function is set to %2")
                    .arg(ui->picNameLineEdit->text(), ui->functionComboBox->currentText()));
            return false;
        }
    } else {
        if (function_index == static_cast<int>(OPL::PilotFunction::PIC) || function_index == static_cast<int>(OPL::PilotFunction::FI)) {
            INFO(tr("The Pilot Function is set to %1, but the PIC is set to %2")
                    .arg(ui->functionComboBox->currentText(), ui->picNameLineEdit->text()));
            return false;
        }
    }

    return true;
}

/*!
 * \brief NewFlightDialog::on_buttonBox_accepted - checks for validity and commits the form data to the database
 * \details When the user is ready to submit a flight entry, a final check for valid entries is made, and the user
 * is prompted to correct unsatisfactory inputs. When this is done, prepareFlightEntryData() collects the input from
 * the user interface and converts it to database format. The data is then stored in a QHash<QString, QVariant>.
 * This data is then used to create a AFlightEntry object, which is then commited to the database by Database::commit()
 */
void NewFlightDialog::on_buttonBox_accepted()
{
    // Debug
    validationState.printValidationStatus();
    for (const auto& le : *mandatoryLineEdits)
        emit le->editingFinished();
    // If input verification is passed, continue, otherwise prompt user to correct
    if (!validationState.allValid()) {
        const auto display_names = QHash<ValidationState::ValidationItem, QString> {
            {ValidationState::ValidationItem::doft, QObject::tr("Date of Flight")},      {ValidationState::ValidationItem::dept, QObject::tr("Departure Airport")},
            {ValidationState::ValidationItem::dest, QObject::tr("Destination Airport")}, {ValidationState::ValidationItem::tofb, QObject::tr("Time Off Blocks")},
            {ValidationState::ValidationItem::tonb, QObject::tr("Time on Blocks")},      {ValidationState::ValidationItem::pic, QObject::tr("PIC Name")},
            {ValidationState::ValidationItem::acft, QObject::tr("Aircraft Registration")}
        };
        QString missing_items;
        for (int i=0; i < mandatoryLineEdits->size(); i++) {
            if (!validationState.validAt(i)){
                missing_items.append(display_names.value(static_cast<ValidationState::ValidationItem>(i)) + "<br>");
                mandatoryLineEdits->at(i)->setStyleSheet(QStringLiteral("border: 1px solid red"));
            }
        }

        INFO(tr("Not all mandatory entries are valid.<br>"
                "The following item(s) are empty or invalid:"
                "<br><br><center><b>%1</b></center><br>"
                "Please go back and fill in the required data."
                ).arg(missing_items));
        return;
    }

    if(!checkPilotFunctionsValid())
        return;

    // If input verification passed, collect input and submit to database
    const auto newData = prepareFlightEntryData();
    DEB << "Old Data: ";
    DEB << flightEntry;

    flightEntry.setData(newData);
    DEB << "Committing: ";
    DEB << flightEntry;
    if (!DB->commit(flightEntry)) {
        WARN(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(DB->lastError.text()));
        return;
    } else {
        QDialog::accept();
    }
}
