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
#include "newflightdialog.h"
#include "ui_newflight.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/functions/acalc.h"
#include "src/testing/atimer.h"
#include "src/database/adatabase.h"
#include "src/opl.h"
#include "src/functions/adate.h"
#include "src/functions/alog.h"

const auto LOC_LINE_EDIT_NAME   = QLatin1String("Loc");
const auto ACFT_LINE_EDIT_NAME  = QLatin1String("acft");
const auto TIME_LINE_EDIT_NAME  = QLatin1String("Time");
const auto PILOT_LINE_EDIT_NAME = QLatin1String("Name");
const auto SELF                 = QLatin1String("self");

NewFlightDialog::NewFlightDialog(ACompletionData &completion_data,
                                 QWidget *parent)
    : QDialog(parent),
      ui(new Ui::NewFlight),
      completionData(completion_data)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    setup();
    if (ASettings::read(ASettings::FlightLogging::FunctionComboBox).toString() == QLatin1String("SIC")) {
        ui->picNameLineEdit->setText(QString());
        ui->secondPilotNameLineEdit->setText(SELF);
    }
    if(ASettings::read(ASettings::FlightLogging::FunctionComboBox).toString() == QLatin1String("PIC")){
        ui->picNameLineEdit->setText(SELF);
        emit ui->picNameLineEdit->editingFinished();
    }
    CRIT("Time empty bug - check verification logic");
}

NewFlightDialog::NewFlightDialog(ACompletionData &completion_data,
                                 int row_id,
                                 QWidget *parent)
    : QDialog(parent),
      ui(new Ui::NewFlight),
      completionData(completion_data)
{
    ui->setupUi(this);
    flightEntry = aDB->getFlightEntry(row_id);
    setup();
    formFiller();
}

NewFlightDialog::~NewFlightDialog()
{
    LOG << "Closing NF Dialog";
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                        setup and maintenance of dialog                            ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void NewFlightDialog::setup()
{
    for (const auto & approach : Opl::ApproachTypes){
        ui->ApproachComboBox->addItem(approach);
    }
    updateEnabled = true;
    setupButtonGroups();
    setupRawInputValidation();
    setupSignalsAndSlots();
    readSettings();
    setupUi();

    ui->deptLocLineEdit->setFocus();
}

void NewFlightDialog::setupButtonGroups()
{
    QButtonGroup *FlightRulesGroup = new QButtonGroup(this);
    FlightRulesGroup->addButton(ui->IfrCheckBox);
    FlightRulesGroup->addButton(ui->VfrCheckBox);

    QButtonGroup *PilotTaskGroup = new QButtonGroup(this);
    PilotTaskGroup->addButton(ui->PilotFlyingCheckBox);
    PilotTaskGroup->addButton(ui->PilotMonitoringCheckBox);
}

void NewFlightDialog::setupRawInputValidation()
{
    // Prepare Regular Expressions for QValidators
    const auto name_rx       = QLatin1String("((\\p{L}+)?('|\\-|,)?(\\p{L}+)?)");
    const auto add_name_rx   = QLatin1String("(\\s?(\\p{L}+('|\\-|,)?\\p{L}+?))?");
    const auto time_valid_rx = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");
    const auto loc_valid_rx  = QRegularExpression("[a-zA-Z0-9]{1,4}");
    const auto acft_valid_rx = QRegularExpression("\\w+\\-?(\\w+)?");
    const auto name_valid_rx = QRegularExpression(
                name_rx + add_name_rx + add_name_rx + add_name_rx + ",?\\s?" // up to 4 first names
              + name_rx + add_name_rx + add_name_rx + add_name_rx );         // up to 4 last names

    // create the structs holding the inititalisation data (completion lists and RXs) and store them in a map
    const auto location_data = ValidationSetupData(&completionData.airportList, &loc_valid_rx);
    const auto pilot_data    = ValidationSetupData(&completionData.pilotList, &name_valid_rx);
    const auto aircraft_data = ValidationSetupData(&completionData.tailsList, &acft_valid_rx);
    const auto time_data     = ValidationSetupData(&time_valid_rx);

    const QHash<const QLatin1String*, const ValidationSetupData*> init_data_map = {
        {&LOC_LINE_EDIT_NAME,  &location_data},
        {&PILOT_LINE_EDIT_NAME, &pilot_data},
        {&ACFT_LINE_EDIT_NAME, &aircraft_data},
        {&TIME_LINE_EDIT_NAME, &time_data}
    };

    //get and set up line edits
    const QList<QLineEdit*> line_edits = ui->flightDataTab->findChildren<QLineEdit*>();
    const QList<const QLatin1String*> keys_list = init_data_map.keys();

    for (const auto &line_edit : line_edits) {
        for (const auto &key : keys_list ) {
            if (line_edit->objectName().contains(*key)) {
                // Fetch Data from the map and set up the line edit
                const ValidationSetupData* init_data = init_data_map.value(key);
                DEB << "Setting up: " << line_edit->objectName();

                // Set Validator
                auto validator = new QRegularExpressionValidator(*init_data->validationRegEx, line_edit);
                line_edit->setValidator(validator);
                // Set Completer
                if (init_data->completionData) {
                    auto completer = new QCompleter(*init_data->completionData, line_edit);
                    completer->setCaseSensitivity(Qt::CaseInsensitive);
                    completer->setCompletionMode(QCompleter::PopupCompletion);
                    completer->setFilterMode(Qt::MatchContains);
                    line_edit->setCompleter(completer);
                }
            }
        }
    }

    // populate Mandatory Line Edits list and prepare QBitArray
    mandatoryLineEditsValid.resize(7);
    mandatoryLineEdits = {
        ui->doftLineEdit,
        ui->deptLocLineEdit,
        ui->destLocLineEdit,
        ui->tofbTimeLineEdit,
        ui->tonbTimeLineEdit,
        ui->picNameLineEdit,
        ui->acftLineEdit,
    };
    primaryTimeLineEdits = {
        ui->tofbTimeLineEdit,
        ui->tonbTimeLineEdit
    };
    pilotsLineEdits = {
        ui->picNameLineEdit,
        ui->secondPilotNameLineEdit,
        ui->thirdPilotNameLineEdit
    };

}

void NewFlightDialog::setupSignalsAndSlots()
{
    const auto line_edits = this->findChildren<QLineEdit*>();

    for (const auto &line_edit : line_edits){
        line_edit->installEventFilter(this);
        if(line_edit->objectName().contains(LOC_LINE_EDIT_NAME)){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onToUpperTriggered_textChanged);
        }
        if(line_edit->objectName().contains(ACFT_LINE_EDIT_NAME)){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onToUpperTriggered_textChanged);
        }
        if(line_edit->objectName().contains(PILOT_LINE_EDIT_NAME)){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onPilotNameLineEdit_editingFinished);
        }
        if(line_edit->objectName().contains(TIME_LINE_EDIT_NAME)){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onTimeLineEdit_editingFinished);
        }
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    for (const auto &line_edit : qAsConst(mandatoryLineEdits)) {
        if(line_edit->objectName().contains(QStringLiteral("doft")))
            break;
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::highlighted),
                         this, &NewFlightDialog::onCompleter_highlighted);
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::activated),
                         this, &NewFlightDialog::onCompleter_activated);
    }
#endif
    QObject::connect(ui->calendarWidget, &QCalendarWidget::clicked,
                     this, &NewFlightDialog::onCalendarWidget_clicked);
}

void NewFlightDialog::setupUi()
{
    ui->flightDataTabWidget->setCurrentIndex(0);
    ui->flightDataTabWidget->removeTab(2); // hide calendar widget
    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();
    ui->doftLineEdit->setToolTip(tr("Date Format: %1").arg(ADate::getFormatString(Opl::Date::ADateFormat::ISODate)));
}

void NewFlightDialog::readSettings()
{
    DEB << "Reading Settings...";
    QSettings settings;
    ui->FunctionComboBox->setCurrentText(ASettings::read(ASettings::FlightLogging::Function).toString());
    ui->ApproachComboBox->setCurrentIndex(ASettings::read(ASettings::FlightLogging::Approach).toInt());

    ASettings::read(ASettings::FlightLogging::PilotFlying).toBool() ? ui->PilotFlyingCheckBox->setChecked(true)
                                                          : ui->PilotMonitoringCheckBox->setChecked(true);

    ui->TakeoffSpinBox->setValue(ASettings::read(ASettings::FlightLogging::NumberTakeoffs).toInt());
    ui->TakeoffSpinBox->value() > 0 ? ui->TakeoffCheckBox->setChecked(true)
                                    : ui->TakeoffCheckBox->setChecked(false);
    ui->LandingSpinBox->setValue(ASettings::read(ASettings::FlightLogging::NumberLandings).toInt());
    ui->LandingSpinBox->value() > 0 ? ui->LandingCheckBox->setChecked(true)
                                    : ui->LandingCheckBox->setChecked(false);
    if (ASettings::read(ASettings::FlightLogging::LogIFR).toBool()) {
        ui->IfrCheckBox->setChecked(true);
    } else {
        ui->VfrCheckBox->setChecked(true);
    }

    ui->FlightNumberLineEdit->setText(ASettings::read(ASettings::FlightLogging::FlightNumberPrefix).toString());

    // Debug
    ASettings::write(ASettings::FlightLogging::FlightTimeFormat, Opl::Time::Default);
    TODO << "Support for Decimal Logging is not implemented yet.";
    flightTimeFormat = static_cast<Opl::Time::FlightTimeFormat>(
                ASettings::read(ASettings::FlightLogging::FlightTimeFormat).toInt());


}

void NewFlightDialog::writeSettings()
{
    DEB << "Writing Settings...";

    ASettings::write(ASettings::FlightLogging::Function, ui->FunctionComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::Approach, ui->ApproachComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::PilotFlying, ui->PilotFlyingCheckBox->isChecked());
    ASettings::write(ASettings::FlightLogging::NumberTakeoffs, ui->TakeoffSpinBox->value());
    ASettings::write(ASettings::FlightLogging::NumberLandings, ui->LandingSpinBox->value());
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->IfrCheckBox->isChecked());
}

bool NewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    auto line_edit = qobject_cast<QLineEdit*>(object);
    if (line_edit != nullptr) {
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::FocusIn) {
            //invalidateMandatoryLineEdit(mandatoryLineEdit(mandatoryLineEdits.indexOf(line_edit)));
            //DEB << "Editing " << line_edit->objectName();
            // set verification bit to false when entering a mandatory line edit
            return false;
        }
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::KeyPress) {
            // show completion menu when pressing down arrow
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                //DEB << "Key down event.";
                line_edit->completer()->complete();
            }
            return false;
        }
        if (line_edit->objectName().contains(QStringLiteral("Name")) && event->type() == QEvent::KeyPress) {
            // show completion menu when pressing down arrow
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                //DEB << "Key down event.";
                line_edit->completer()->complete();
            }
            return false;
        }
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///                                 Methods - Input Processing                                  ///
///////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief Fills the deductable items in the Dialog based on mandatory data ui selections.
 */
void NewFlightDialog::fillDeductibleData()
{    
    //zero out labels and line edits to delete previous calculations
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit, ui->tSPMETimeLineEdit, ui->tMPTimeLineEdit,    ui->tIFRTimeLineEdit,
                              ui->tNIGHTTimeLineEdit,ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                              ui->tDUALTimeLineEdit, ui->tFITimeLineEdit,};
    QList<QLabel*>      LB = {ui->tSPSELabel, ui->tSPMELabel,  ui->tMPLabel,  ui->tIFRLabel,  ui->tNIGHTLabel,
                              ui->tPICLabel,  ui->tPICUSLabel, ui->tSICLabel, ui->tDUALLabel, ui->tFILabel};
    for(const auto& widget : LE) {widget->setText(QString());}
    for(const auto& widget : LB) {widget->setText(Opl::Db::NULL_TIME_hhmm);}
    //Calculate block time
    const auto tofb = ATime::fromString(ui->tofbTimeLineEdit->text());
    const auto tonb = ATime::fromString(ui->tonbTimeLineEdit->text());
    const auto tblk = ATime::blocktime(tofb, tonb);
    const auto block_time_string = ATime::toString(tblk, flightTimeFormat);

    ui->tblkTimeLineEdit->setText(block_time_string);
    // get acft data and fill deductible entries
    auto acft = aDB->getTailEntry(completionData.tailsIdMap.key(ui->acftLineEdit->text()));

    // TOTAL
    ui->tblkLabel->setText(QLatin1String("<b>") + block_time_string + QLatin1String("</b>"));
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->tIFRTimeLineEdit->setText(block_time_string);
        ui->tIFRLabel->setText(block_time_string);
    }
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        ui->tPICTimeLineEdit->setText(block_time_string);
        ui->tPICLabel->setText(block_time_string);
        break;
    case 1://PICus
        ui->tPICUSTimeLineEdit->setText(block_time_string);
        ui->tPICUSLabel->setText(block_time_string);
        break;
    case 2://Co-Pilot
        ui->tSICTimeLineEdit->setText(block_time_string);
        ui->tSICLabel->setText(block_time_string);
        break;
    case 3://Dual
        ui->tDUALTimeLineEdit->setText(block_time_string);
        ui->tDUALLabel->setText(block_time_string);
        break;
    case 4://Instructor
        ui->tFITimeLineEdit->setText(block_time_string);
        ui->tFILabel->setText(block_time_string);
        ui->tPICTimeLineEdit->setText(block_time_string);
        ui->tPICLabel->setText(block_time_string);
    }

    // if location data is available, fill night time
    if (locLineEditsValid()) {

        const auto block_minutes = ATime::toMinutes(tblk);
        QString dept_date = ui->doftLineEdit->text() + 'T' + ATime::toString(tofb);
        auto dept_date_time = QDateTime::fromString(dept_date, QStringLiteral("yyyy-MM-ddThh:mm"));
        const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();
        auto night_time = ATime::qTimefromMinutes(ACalc::calculateNightTime(
                                                 ui->deptLocLineEdit->text(),
                                                 ui->destLocLineEdit->text(),
                                                 dept_date_time,
                                                 block_minutes,
                                                 night_angle));

        ui->tNIGHTTimeLineEdit->setText(ATime::toString(night_time, flightTimeFormat));
        ui->tNIGHTLabel->setText(ATime::toString(night_time, flightTimeFormat));
    }

    // if acft data is available, fill additional times
    if (acftLineEditValid()) {

        if (acft.getData().isEmpty())
            WARN("Error: No valid aircraft object available, unable to deterime auto times.");

        // SP SE
        if(acft.getData().value(Opl::Db::TAILS_MULTIPILOT).toInt() == 0
                && acft.getData().value(Opl::Db::TAILS_MULTIENGINE).toInt() == 0){
            ui->tSPSETimeLineEdit->setText(block_time_string);
            ui->tSPSELabel->setText(block_time_string);
        }
        // SP ME
        if(acft.getData().value(Opl::Db::TAILS_MULTIPILOT).toInt() == 0
                && acft.getData().value(Opl::Db::TAILS_MULTIENGINE).toInt() == 1){
            ui->tSPMETimeLineEdit->setText(block_time_string);
            ui->tSPMELabel->setText(block_time_string);
        }
        // MP
        if(acft.getData().value(Opl::Db::TAILS_MULTIPILOT).toInt() == 1){
            ui->tMPTimeLineEdit->setText(block_time_string);
            ui->tMPLabel->setText(block_time_string);
        }
    }
}

/*!
 * \brief Collect input and create a Data map for the entry object.
 *
 * This function should only be called if input validation has been passed, since
 * no input validation is done in this step and input data is assumed to be valid.
 * \return
 */
RowData_T NewFlightDialog::collectInput()
{
    RowData_T newData;
    //DEB << "Collecting Input...";
    //Block Time
    const auto tofb = ATime::fromString(ui->tofbTimeLineEdit->text());
    const auto tonb = ATime::fromString(ui->tonbTimeLineEdit->text());
    const auto tblk = ATime::blocktime(tofb, tonb);
    const auto block_minutes = ATime::toMinutes(tblk);
    // Mandatory data
    newData.insert(Opl::Db::FLIGHTS_DOFT, ui->doftLineEdit->text());
    newData.insert(Opl::Db::FLIGHTS_DEPT, ui->deptLocLineEdit->text());
    newData.insert(Opl::Db::FLIGHTS_TOFB, ATime::toMinutes(tofb));
    newData.insert(Opl::Db::FLIGHTS_DEST, ui->destLocLineEdit->text());
    newData.insert(Opl::Db::FLIGHTS_TONB, ATime::toMinutes(tonb));
    newData.insert(Opl::Db::FLIGHTS_TBLK, block_minutes);
    // Aircraft
    newData.insert(Opl::Db::FLIGHTS_ACFT, completionData.tailsIdMap.key(ui->acftLineEdit->text()));
    // Pilots
    newData.insert(Opl::Db::FLIGHTS_PIC, completionData.pilotsIdMap.key(ui->picNameLineEdit->text()));
    newData.insert(Opl::Db::FLIGHTS_SECONDPILOT, completionData.pilotsIdMap.key(ui->secondPilotNameLineEdit->text()));
    newData.insert(Opl::Db::FLIGHTS_THIRDPILOT, completionData.pilotsIdMap.key(ui->thirdPilotNameLineEdit->text()));

    // Extra Times
    ui->tSPSETimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TSPSE, QString())
              : newData.insert(Opl::Db::FLIGHTS_TSPSE, stringToMinutes(
                                   ui->tSPSETimeLineEdit->text(), flightTimeFormat));

    ui->tSPMETimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TSPME, QString())
              : newData.insert(Opl::Db::FLIGHTS_TSPME, stringToMinutes(
                                   ui->tSPMETimeLineEdit->text(), flightTimeFormat));
    ui->tMPTimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TMP, QString())
              : newData.insert(Opl::Db::FLIGHTS_TMP, stringToMinutes(
                                   ui->tMPTimeLineEdit->text(), flightTimeFormat));

    if (ui->IfrCheckBox->isChecked()) {
        newData.insert(Opl::Db::FLIGHTS_TIFR, block_minutes);
    } else {
        newData.insert(Opl::Db::FLIGHTS_TIFR, QString());
    }
    // Night
    const QString dept_date = ui->doftLineEdit->text() + 'T'
            + ATime::toString(tofb);
    const auto dept_date_time = QDateTime::fromString(dept_date, QStringLiteral("yyyy-MM-ddThh:mm"));
    const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();
    const auto night_time = ATime::qTimefromMinutes(ACalc::calculateNightTime(
                                             ui->deptLocLineEdit->text(),
                                             ui->destLocLineEdit->text(),
                                             dept_date_time,
                                             block_minutes,
                                             night_angle));
    const auto night_minutes = ATime::toMinutes(night_time);
    newData.insert(Opl::Db::FLIGHTS_TNIGHT, night_minutes);

    // Function times - This is a little explicit but these are mutually exclusive so its better to be safe than sorry here.
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        newData.insert(Opl::Db::FLIGHTS_TPIC, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, QString());
        newData.insert(Opl::Db::FLIGHTS_TSIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TDUAL, QString());
        newData.insert(Opl::Db::FLIGHTS_TFI, QString());
        break;
    case 1://PICUS
        newData.insert(Opl::Db::FLIGHTS_TPIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TPICUS, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TSIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TDUAL, QString());
        newData.insert(Opl::Db::FLIGHTS_TFI, QString());
        break;
    case 2://Co-Pilot
        newData.insert(Opl::Db::FLIGHTS_TPIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TPICUS, QString());
        newData.insert(Opl::Db::FLIGHTS_TSIC, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, QString());
        newData.insert(Opl::Db::FLIGHTS_TFI, QString());
        break;
    case 3://Dual
        newData.insert(Opl::Db::FLIGHTS_TPIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TPICUS, QString());
        newData.insert(Opl::Db::FLIGHTS_TSIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TDUAL, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TFI, QString());
        break;
    case 4://Instructor
        newData.insert(Opl::Db::FLIGHTS_TPIC, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, QString());
        newData.insert(Opl::Db::FLIGHTS_TSIC, QString());
        newData.insert(Opl::Db::FLIGHTS_TDUAL, QString());
        newData.insert(Opl::Db::FLIGHTS_TFI, block_minutes);
    }
    // Pilot Flying
    newData.insert(Opl::Db::FLIGHTS_PILOTFLYING, ui->PilotFlyingCheckBox->isChecked());
    // TO and LDG - again a bit explicit, but we  need to check for both night to day as well as day to night transitions.
    if (ui->TakeoffCheckBox->isChecked()) {
        if (night_minutes == 0) { // all day
            newData.insert(Opl::Db::FLIGHTS_TODAY, ui->TakeoffSpinBox->value());
            newData.insert(Opl::Db::FLIGHTS_TONIGHT, 0);
        } else if (night_minutes == block_minutes) { // all night
            newData.insert(Opl::Db::FLIGHTS_TODAY, 0);
            newData.insert(Opl::Db::FLIGHTS_TONIGHT, ui->TakeoffSpinBox->value());
        } else {
            if(ACalc::isNight(ui->deptLocLineEdit->text(), dept_date_time,  night_angle)) {
                newData.insert(Opl::Db::FLIGHTS_TODAY, 0);
                newData.insert(Opl::Db::FLIGHTS_TONIGHT, ui->TakeoffSpinBox->value());
            } else {
                newData.insert(Opl::Db::FLIGHTS_TODAY, ui->TakeoffSpinBox->value());
                newData.insert(Opl::Db::FLIGHTS_TONIGHT, 0);
            }
        }
    } else {
        newData.insert(Opl::Db::FLIGHTS_TODAY, 0);
        newData.insert(Opl::Db::FLIGHTS_TONIGHT, 0);
    }

    if (ui->LandingCheckBox->isChecked()) {
        if (night_minutes == 0) { // all day
            newData.insert(Opl::Db::FLIGHTS_LDGDAY, ui->LandingSpinBox->value());
            newData.insert(Opl::Db::FLIGHTS_LDGNIGHT, 0);
        } else if (night_minutes == block_minutes) { // all night
            newData.insert(Opl::Db::FLIGHTS_LDGDAY, 0);
            newData.insert(Opl::Db::FLIGHTS_LDGNIGHT, ui->LandingSpinBox->value());
        } else { //check
            const QString dest_date = ui->doftLineEdit->text() + 'T'
                    + ATime::toString(tonb);
            const auto dest_date_time = QDateTime::fromString(dest_date, QStringLiteral("yyyy-MM-ddThh:mm"));
            if (ACalc::isNight(ui->destLocLineEdit->text(), dest_date_time,  night_angle)) {
                newData.insert(Opl::Db::FLIGHTS_LDGDAY, 0);
                newData.insert(Opl::Db::FLIGHTS_LDGNIGHT, ui->LandingSpinBox->value());
            } else {
                newData.insert(Opl::Db::FLIGHTS_LDGDAY, ui->LandingSpinBox->value());
                newData.insert(Opl::Db::FLIGHTS_LDGNIGHT, 0);
            }
        }
    } else {
        newData.insert(Opl::Db::FLIGHTS_LDGDAY, 0);
        newData.insert(Opl::Db::FLIGHTS_LDGNIGHT, 0);
    }

    newData.insert(Opl::Db::FLIGHTS_AUTOLAND, ui->AutolandSpinBox->value());
    newData.insert(Opl::Db::FLIGHTS_APPROACHTYPE, ui->ApproachComboBox->currentText());
    newData.insert(Opl::Db::FLIGHTS_FLIGHTNUMBER, ui->FlightNumberLineEdit->text());
    newData.insert(Opl::Db::FLIGHTS_REMARKS, ui->RemarksLineEdit->text());
    DEB << "New Flight Data: " << newData;

    return newData;
}

void NewFlightDialog::formFiller()
{
    DEB << "Filling Line Edits...";
    // get Line Edits
    const auto line_edits = this->findChildren<QLineEdit *>();
    QStringList line_edits_names;
    for (const auto& le : line_edits) {
        line_edits_names << le->objectName();
    }

    ui->acftLineEdit->setText(flightEntry.getRegistration());
    line_edits_names.removeOne(QStringLiteral("acftLineEdit"));

    const auto data_keys = flightEntry.getData().keys();
    for (const auto& data_key : data_keys) {
        auto rx = QRegularExpression(data_key + QStringLiteral("LineEdit"));//acftLineEdit
        for(const auto& leName : qAsConst(line_edits_names)){
            if(rx.match(leName).hasMatch())  {
                //DEB << "Loc Match found: " << key << " - " << leName);
                auto line_edit = this->findChild<QLineEdit *>(leName);
                if(line_edit != nullptr){
                    line_edit->setText(flightEntry.getData().value(data_key).toString());
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + QStringLiteral("Loc\\w+?"));
        for(const auto& leName : qAsConst(line_edits_names)){
            if(rx.match(leName).hasMatch())  {
                //DEB << "Loc Match found: " << key << " - " << leName);
                auto line_edit = this->findChild<QLineEdit *>(leName);
                if(line_edit != nullptr){
                    line_edit->setText(flightEntry.getData().value(data_key).toString());
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + QStringLiteral("Time\\w+?"));
        for(const auto& leName : qAsConst(line_edits_names)){
            if(rx.match(leName).hasMatch())  {
                //DEB << "Time Match found: " << key << " - " << leName);
                auto line_edits = this->findChild<QLineEdit *>(leName);
                if(line_edits != nullptr){
                    DEB << "Setting " << line_edits->objectName() << " to " << ATime::toString(flightEntry.getData().value(data_key).toInt(), flightTimeFormat);
                    line_edits->setText(ATime::toString(flightEntry.getData().value(data_key).toInt(),
                                                        flightTimeFormat));
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + QStringLiteral("Name\\w+?"));
        for(const auto& leName : qAsConst(line_edits_names)){
            if(rx.match(leName).hasMatch())  {
                auto line_edits = this->findChild<QLineEdit *>(leName);
                if(line_edits != nullptr){
                    DEB << completionData.pilotsIdMap.value(1);
                    line_edits->setText(completionData.pilotsIdMap.value(flightEntry.getData().value(data_key).toInt()));
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
    }
    //FunctionComboBox
    QList<QLineEdit*> function_combo_boxes = {ui->tPICTimeLineEdit, ui->tPICUSTimeLineEdit,
                                              ui->tSICTimeLineEdit, ui->tDUALTimeLineEdit,
                                              ui->tFITimeLineEdit};
    for(const auto& line_edit : function_combo_boxes){
        if(line_edit->text() != QStringLiteral("00:00")){
            QString name = line_edit->objectName();
            name.chop(12);
            name.remove(0,1);
            ui->FunctionComboBox->setCurrentText(name);
        }
    }
    // Approach Combo Box
    const QString& app = flightEntry.getData().value(Opl::Db::FLIGHTS_APPROACHTYPE).toString();
    if(app != QString()){
        ui->ApproachComboBox->setCurrentText(app);
    }
    // Task and Rules
    qint8 PF = flightEntry.getData().value(Opl::Db::FLIGHTS_PILOTFLYING).toInt();
    if (PF > 0) {
        ui->PilotFlyingCheckBox->setChecked(true);
    } else {
        ui->PilotMonitoringCheckBox->setChecked(true);
    }
    qint8 FR = flightEntry.getData().value(Opl::Db::FLIGHTS_TIFR).toInt();
    if (FR > 0) {
        ui->IfrCheckBox->setChecked(true);
    } else {
        ui->tIFRTimeLineEdit->setText(QString());
        ui->VfrCheckBox->setChecked(true);
    }
    // Take Off and Landing
    qint8 TO = flightEntry.getData().value(Opl::Db::FLIGHTS_TODAY).toInt()
            + flightEntry.getData().value(Opl::Db::FLIGHTS_TONIGHT).toInt();
    qint8 LDG = flightEntry.getData().value(Opl::Db::FLIGHTS_LDGDAY).toInt()
            + flightEntry.getData().value(Opl::Db::FLIGHTS_LDGNIGHT).toInt();
    if(TO > 0) {
        ui->TakeoffCheckBox->setChecked(true);
        ui->TakeoffSpinBox->setValue(TO);
    } else {
        ui->TakeoffCheckBox->setChecked(false);
        ui->TakeoffSpinBox->setValue(0);
    }
    if(LDG > 0) {
        ui->LandingCheckBox->setChecked(true);
        ui->LandingSpinBox->setValue(LDG);
    } else {
        ui->LandingCheckBox->setChecked(false);
        ui->LandingSpinBox->setValue(0);
    }
    qint8 AL = flightEntry.getData().value(Opl::Db::FLIGHTS_AUTOLAND).toInt();
    if(AL > 0) {
        ui->AutolandCheckBox->setChecked(true);
        ui->AutolandSpinBox->setValue(AL);
    }

    for(const auto& le : qAsConst(mandatoryLineEdits)){
        emit le->editingFinished();
    }
}

bool NewFlightDialog::isLessOrEqualThanBlockTime(const QString time_string)
{
    if (!allMandatoryLineEditsValid()){
        INFO(tr("Unable to determine total block time.<br>"
                               "Please fill out all Mandatory Fields<br>"
                               "before manually editing these times."));
        return false;
    }

    auto extra_time = ATime::fromString(time_string, flightTimeFormat);
    auto block_time = ATime::blocktime(ATime::fromString(
                                           ui->tofbTimeLineEdit->text(), flightTimeFormat),
                                       ATime::fromString(
                                           ui->tonbTimeLineEdit->text(), flightTimeFormat));
    if (extra_time <= block_time) {
        return true;
    } else {
        WARN(tr("The flight time you have entered is longer than the total blocktime:"
                               "<br><center><b>%1</b></center>"
                               ).arg(ATime::toString(block_time, flightTimeFormat)));
        return false;
    }
}

/*!
 * \brief NewFlight::addNewTailMessageBox If the user input is not in the aircraftList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlightDialog::addNewTail(QLineEdit *parent_line_edit)
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
        NewTailDialog na(ui->acftLineEdit->text(), this);
        int ret = na.exec();
        // update map and list, set line edit
        if (ret == QDialog::Accepted) {
            DEB << "New Tail Entry added. Id:" << aDB->getLastEntry(ADatabaseTable::tails);

            // update completion Data and Completer
            completionData.updateTails();
            auto new_model = new QStringListModel(completionData.tailsList, parent_line_edit->completer());
            parent_line_edit->completer()->setModel(new_model); //setModel deletes old model if it has the completer as parent

            // update Line Edit
            parent_line_edit->setText(completionData.tailsIdMap.value(aDB->getLastEntry(ADatabaseTable::tails)));
            emit parent_line_edit->editingFinished();
        } else {
            parent_line_edit->setText(QString());
            parent_line_edit->setFocus();
        }
    } else {
        parent_line_edit->setText(QString());
        parent_line_edit->setFocus();
    }
}

/*!
 * \brief NewFlight::addNewPilot If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlightDialog::addNewPilot(QLineEdit *parent_line_edit)
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
            DEB << "New Pilot Entry added. Id:" << aDB->getLastEntry(ADatabaseTable::pilots);
            // update completion Data and Completer
            completionData.updatePilots();
            auto new_model = new QStringListModel(completionData.pilotList, parent_line_edit->completer());
            parent_line_edit->completer()->setModel(new_model); //setModel deletes old model if it has the completer as parent

            // update Line Edit
            parent_line_edit->setText(completionData.pilotsIdMap.value(aDB->getLastEntry(ADatabaseTable::pilots)));
            emit parent_line_edit->editingFinished();
        } else {
            parent_line_edit->setText(QString());
            parent_line_edit->setFocus();
        }
    } else {
        parent_line_edit->setText(QString());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                               Flight Data Tab Slots                                         ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void NewFlightDialog::on_cancelButton_clicked()
{
    DEB << "Cancel Button clicked.";
    reject();
}

void NewFlightDialog::on_submitButton_clicked()
{
    // emit editing finished for all mandatory line edits to trigger input verification
    for (const auto &line_edit : qAsConst(mandatoryLineEdits)) {
        emit line_edit->editingFinished();
    }

    // If input verification is passed, continue, otherwise prompt user to correct
    if (!allMandatoryLineEditsValid()) {
        const auto display_names = QMap<int, QString> {
            {0, QObject::tr("Date of Flight")},      {1, QObject::tr("Departure Airport")},
            {2, QObject::tr("Destination Airport")}, {3, QObject::tr("Time Off Blocks")},
            {4, QObject::tr("Time on Blocks")},      {5, QObject::tr("PIC Name")},
            {6, QObject::tr("Aircraft Registration")}
        };
        QString missing_items;
        for (int i=0; i < mandatoryLineEdits.size(); i++) {
            if (!mandatoryLineEditsValid[i]){
                missing_items.append(display_names.value(i) + "<br>");
                mandatoryLineEdits[i]->setStyleSheet(QStringLiteral("border: 1px solid red"));
            }
        }

        INFO(tr("Not all mandatory entries are valid.<br>"
                "The following item(s) are empty or missing:"
                "<br><br><center><b>%1</b></center><br>"
                "Please go back and fill in the required data."
                ).arg(missing_items));
        return;
    }

    // If input verification passed, collect input and submit to database
    auto newData = collectInput();
    DEB << "Setting Data for flightEntry...";
    flightEntry.setData(newData);
    DEB << "Committing...";
    if (!aDB->commit(flightEntry)) {
        WARN(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(aDB->lastError.text()));
        return;
    } else {
        QDialog::accept();
    }
}



/*
 * Shared Slots
 */

void NewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Good input received - " << line_edit->text();

    line_edit->setStyleSheet(QString());

    if (mandatoryLineEdits.contains(line_edit))
        validateMandatoryLineEdit(mandatoryLineEdit(mandatoryLineEdits.indexOf(line_edit)));
    validationStatus();

    if (timeLineEditsValid())
        onMandatoryLineEditsFilled();
}

void NewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Bad input received - " << line_edit->text();
    line_edit->setStyleSheet(QStringLiteral("border: 1px solid red"));

    if (mandatoryLineEdits.contains(line_edit))
        invalidateMandatoryLineEdit(mandatoryLineEdit(mandatoryLineEdits.indexOf(line_edit)));
    validationStatus();

}

// capitalize input for dept, dest and registration input
void NewFlightDialog::onToUpperTriggered_textChanged(const QString &text)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    //DEB << "Text changed - " << line_edit->objectName() << line_edit->text();
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}

// update is disabled if the user chose to manually edit extra times
void NewFlightDialog::onMandatoryLineEditsFilled()
{
    if (updateEnabled)
        fillDeductibleData();
}

// make sure that when using keyboard to scroll through completer sugggestions, line edit is up to date
void NewFlightDialog::onCompleter_highlighted(const QString &completion)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB << "Completer highlighted - " << line_edit->objectName() << completion;
    line_edit->setText(completion);
}

void NewFlightDialog::onCompleter_activated(const QString &)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB << "Line edit " << line_edit->objectName() << "completer activated.";
    emit line_edit->editingFinished();
}

/*
 *  Date of Flight
 */

void NewFlightDialog::on_doftLineEdit_editingFinished()
{    
    auto line_edit = ui->doftLineEdit;
    auto text = ui->doftLineEdit->text();
    auto label = ui->doftDisplayLabel;
    //DEB << line_edit->objectName() << "Editing finished - " << text;

    TODO << "Non-default Date formats not implemented yet.";
    Opl::Date::ADateFormat date_format = Opl::Date::ADateFormat::ISODate;
    auto date = ADate::parseInput(text, date_format);
    if (date.isValid()) {
        label->setText(date.toString(Qt::TextDate));
        line_edit->setText(ADate::toString(date, date_format));
        onGoodInputReceived(line_edit);
        return;
    }

    label->setText(tr("Invalid Date."));
    onBadInputReceived(line_edit);
}

void NewFlightDialog::onCalendarWidget_clicked(const QDate &date)
{
    const auto& le = ui->doftLineEdit;
    le->blockSignals(false);
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    le->setText(date.toString(Qt::ISODate));
    emit le->editingFinished();
}

void NewFlightDialog::on_calendarPushButton_clicked()
{
    const auto& cw = ui->calendarWidget;
    const auto& le = ui->doftLineEdit;
    const auto& anchor = ui->placeLabel1;

    if(cw->isVisible()){
        le->blockSignals(false);
        cw->hide();
        anchor->resize(ui->placeLabel2->size());
        le->setFocus();
    } else {
        le->blockSignals(true);
        // Determine size based on layout coordinates
        int c1 = anchor->pos().rx();
        int c2 = le->pos().rx();
        int z  = le->size().rwidth();
        int x = (c2 - c1)+ z;
        c1 = anchor->pos().ry();
        c2 = ui->acftLineEdit->pos().ry();
        z  = ui->acftLineEdit->size().height();
        int y = (c2 - c1) + z;
        //Re-size calendar and parent label accordingly
        anchor->resize(x, y);
        cw->setParent(ui->placeLabel1);
        cw->setGeometry(QRect(0, 0, x, y));
        cw->show();
        cw->setFocus();
    }
}

void NewFlightDialog::validationStatus()
{

    QString deb_string("Validation State:\tdoft\tdept\tdest\ttofb\ttonb\tpic\tacft\n");
    deb_string += "\t\t\t\t\t" + QString::number(mandatoryLineEditsValid[0]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[1]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[2]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[3]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[4]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[5]);
    deb_string += "\t" + QString::number(mandatoryLineEditsValid[6]);
    qDebug().noquote() << deb_string;
}

/*
 * Location Line Edits
 */

void NewFlightDialog::on_deptLocLineEdit_editingFinished()
{
    onLocationEditingFinished(ui->deptLocLineEdit, ui->deptNameLabel);
}

void NewFlightDialog::on_destLocLineEdit_editingFinished()
{
    onLocationEditingFinished(ui->destLocLineEdit, ui->destNameLabel);
}

void NewFlightDialog::onLocationEditingFinished(QLineEdit *line_edit, QLabel *name_label)
{
    const auto &text = line_edit->text();
    //DEB << line_edit->objectName() << " Editing finished. " << text);
    int airport_id = 0;

    // try to map iata or icao code to airport id;
    if (text.length() == 3) {
        airport_id = completionData.airportIataIdMap.key(text);
    } else {
        airport_id = completionData.airportIcaoIdMap.key(text);
    }
    // check result
    if (airport_id == 0) {
        // to do: prompt user how to handle unknown airport
        name_label->setText(tr("Unknown airport identifier"));
        onBadInputReceived(line_edit);
        return;
    }
    line_edit->setText(completionData.airportIcaoIdMap.value(airport_id));
    name_label->setText(completionData.airportNameIdMap.value(airport_id));
    onGoodInputReceived(line_edit);
}

/*
 * Time Line Edits
 */

void NewFlightDialog::onTimeLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB << line_edit->objectName() << "Editing Finished -" << line_edit->text();

    line_edit->setText(ATime::formatTimeInput(line_edit->text()));
    const auto time = ATime::fromString(line_edit->text());
    if(time.isValid()){
        if(primaryTimeLineEdits.contains(line_edit)) {
            onGoodInputReceived(line_edit);
        } else { // is extra time line edit
            if (!isLessOrEqualThanBlockTime(line_edit->text())) {
                line_edit->setText(QString());
                line_edit->setFocus();
                return;
            }
        }
    } else {
        if (!line_edit->text().isEmpty())
            onBadInputReceived(line_edit);
    }
}

/*
 * Aircraft Line Edit
 */

void NewFlightDialog::on_acftLineEdit_editingFinished()
{
    TODO << "Looking up and matching tails is currently broken...";
    auto line_edit = ui->acftLineEdit;
    int acft_id = completionData.tailsIdMap.key(line_edit->text());
    DEB << "acft_id: " << acft_id;

    if (acft_id != 0) { // Success
        //DEB << "Mapped: " << line_edit->text() << completionData.tailsIdMap.value(line_edit->text());
        auto acft = aDB->getTailEntry(acft_id);
        ui->acftTypeLabel->setText(acft.type());
        ui->picCompanyLabel->setText(acft.getData().value(Opl::Db::TAILS_COMPANY).toString());
        onGoodInputReceived(line_edit);
        return;
    }

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        line_edit->setText(line_edit->completer()->currentCompletion().split(QLatin1Char(' ')).first());
        emit line_edit->editingFinished();
        return;
    }

    // If no success mark as bad input
    onBadInputReceived(line_edit);
    ui->acftTypeLabel->setText(tr("Unknown Registration."));
    if (line_edit->text() != QString())
        addNewTail(line_edit);
}

/*
 * Pilot Line Edits
 */

void NewFlightDialog::onPilotNameLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    //DEB << line_edit->objectName() << "Editing Finished -" << line_edit->text());

    if(line_edit->text().contains(SELF, Qt::CaseInsensitive)) {
        DEB << "self recognized.";
        line_edit->setText(completionData.pilotsIdMap.value(1));
        onGoodInputReceived(line_edit);
        return;
    }

    if(completionData.pilotsIdMap.key(line_edit->text()) != 0) {
        DEB << "Mapped: " << line_edit->text() << completionData.pilotsIdMap.key(line_edit->text());
        onGoodInputReceived(line_edit);
        return;
    }

    if (line_edit->text().isEmpty()) {
        return;
    }

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB << "Trying to fix input...";
        line_edit->setText(line_edit->completer()->currentCompletion());
        emit line_edit->editingFinished();
        return;
    }

    onBadInputReceived(line_edit);
    addNewPilot(line_edit);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                               Auto Logging Tab Slots                                        ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void NewFlightDialog::on_setAsDefaultButton_clicked()
{
    writeSettings();
}

void NewFlightDialog::on_restoreDefaultButton_clicked()
{
    readSettings();
}

void NewFlightDialog::on_PilotFlyingCheckBox_stateChanged(int)
{
    DEB << "PF checkbox state changed.";
    if(ui->PilotFlyingCheckBox->isChecked()){
        ui->TakeoffSpinBox->setValue(1);
        ui->TakeoffCheckBox->setCheckState(Qt::Checked);
        ui->LandingSpinBox->setValue(1);
        ui->LandingCheckBox->setCheckState(Qt::Checked);

    }else if(!ui->PilotFlyingCheckBox->isChecked()){
        ui->TakeoffSpinBox->setValue(0);
        ui->TakeoffCheckBox->setCheckState(Qt::Unchecked);
        ui->LandingSpinBox->setValue(0);
        ui->LandingCheckBox->setCheckState(Qt::Unchecked);
    }
}

void NewFlightDialog::on_IfrCheckBox_stateChanged(int)
{
    if (timeLineEditsValid() && updateEnabled)
        onMandatoryLineEditsFilled();
}

void NewFlightDialog::on_manualEditingCheckBox_stateChanged(int arg1)
{
    if (!(timeLineEditsValid()) && ui->manualEditingCheckBox->isChecked()) {
        INFO(tr("Before editing times manually, please fill out the required fields "
                "in the flight data tab, so that total time can be calculated."));
        ui->manualEditingCheckBox->setChecked(false);
        return;
    }
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit, ui->tSPMETimeLineEdit, ui->tMPTimeLineEdit,    ui->tIFRTimeLineEdit,
                              ui->tNIGHTTimeLineEdit,ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                              ui->tDUALTimeLineEdit, ui->tFITimeLineEdit};
    switch (arg1) {
    case 0:
        for(const auto& le : LE){
            le->setFocusPolicy(Qt::NoFocus);
            le->setStyleSheet(QString());
        }
        updateEnabled = true;
        if (allMandatoryLineEditsValid() && updateEnabled)
            onMandatoryLineEditsFilled();
        break;
    case 2:
        for(const auto& le : LE){
            le->setFocusPolicy(Qt::StrongFocus);
        }
        updateEnabled = false;
        break;
    default:
        break;
    }
}

void NewFlightDialog::on_ApproachComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == QLatin1String("ILS CAT III")){  //for a CAT III approach an Autoland is mandatory, so we can preselect it.
        ui->AutolandCheckBox->setCheckState(Qt::Checked);
        ui->AutolandSpinBox->setValue(1);
    }else{
        ui->AutolandCheckBox->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox->setValue(0);
    }

    if (arg1 != QLatin1String("VISUAL"))
        ui->IfrCheckBox->setChecked(true);
}

void NewFlightDialog::on_FunctionComboBox_currentIndexChanged(int)
{
    if (updateEnabled)
        fillDeductibleData();
}

// [F]: Not a priority right now.
void NewFlightDialog::on_deptTZComboBox_currentIndexChanged(int index)
{
    if (index > 0) {
        INFO(tr("Currently only logging in UTC time is supported."));
        ui->deptTZComboBox->setCurrentIndex(0);
    }
}

void NewFlightDialog::on_destTZComboBox_currentIndexChanged(int index)
{
    if (index > 0) {
        INFO(tr("Currently only logging in UTC time is supported."));
        ui->destTZComboBox->setCurrentIndex(0);}
}
