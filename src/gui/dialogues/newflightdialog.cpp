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
#include "newflightdialog.h"
#include "ui_newflight.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/functions/acalc.h"
#include "src/testing/atimer.h"
#include "src/database/adatabase.h"
#include "src/oplconstants.h"

#include "src/testing/adebug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                         constants                                           ///
///////////////////////////////////////////////////////////////////////////////////////////////////

static const auto EMPTY_STRING=QStringLiteral("");
static const auto NAME_RX     = QLatin1String("((\\p{L}+)?('|\\-|,)?(\\p{L}+)?)");
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-|,)?\\p{L}+?))?");

static const auto TIME_VALID_RGX       = QRegularExpression(
            "([01]?[0-9]|2[0-3]):?[0-5][0-9]?");
static const auto LOC_VALID_RGX        = QRegularExpression(
            "[a-zA-Z0-9]{1,4}");
static const auto AIRCRAFT_VALID_RGX   = QRegularExpression(
            "\\w+\\-?(\\w+)?");
static const auto NAME_VALID_RGX       = QRegularExpression(
            NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",?\\s?" // up to 4 first names
            + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );         // up to 4 last names
static const auto DATE_VALID_RGX       = QRegularExpression(
            "^([1-9][0-9]{3}).?(1[0-2]|0[1-9]).?(3[01]|0[1-9]|[12][0-9])?$");
static const auto SELF_RX              = QRegularExpression(
            "self", QRegularExpression::CaseInsensitiveOption);

static const auto MANDATORY_LINE_EDITS_DISPLAY_NAMES = QMap<int, QString> {
    {0, QObject::tr("Date of Flight")},      {1, QObject::tr("Departure Airport")},
    {2, QObject::tr("Destination Airport")}, {3, QObject::tr("Time Off Blocks")},
    {4, QObject::tr("Time on Blocks")},      {5, QObject::tr("PIC Name")},
    {6, QObject::tr("Aircraft Registration")}
};

//
// MandatoryLineEdits definition
// Ugly but works
NewFlightDialog::MandatoryLineEdits::MandatoryLineEdits(std::initializer_list<QLineEdit*> il)
    : lineEdits(il), lineEditsOk(il.size())
{}
void NewFlightDialog::MandatoryLineEdits::operator= (std::initializer_list<QLineEdit*> il)
{
    lineEdits = il;
    lineEdits.resize(il.size());
}
bool NewFlightDialog::MandatoryLineEdits::contains(QLineEdit* le)
{
    return lineEdits.contains(le);
}
void NewFlightDialog::MandatoryLineEdits::validate(QLineEdit* le)
{
    lineEditsOk.setBit(lineEdits.indexOf(le), true);
}
void NewFlightDialog::MandatoryLineEdits::unvalidate(QLineEdit* le)
{
    lineEditsOk.setBit(lineEdits.indexOf(le), false);
}
int NewFlightDialog::MandatoryLineEdits::countOk()
{
    return lineEditsOk.count(true);
}
int NewFlightDialog::MandatoryLineEdits::size()
{
    return lineEditsOk.size();
}
bool NewFlightDialog::MandatoryLineEdits::okAt(int idx)
{
    return lineEditsOk[idx];
}
bool NewFlightDialog::MandatoryLineEdits::allOk()
{
    return lineEditsOk.count(true) == lineEdits.size();
}
QLineEdit* NewFlightDialog::MandatoryLineEdits::operator[] (int idx)
{
    return lineEdits[idx];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                      Construction                                           ///
///////////////////////////////////////////////////////////////////////////////////////////////////

/// [F] The general idea for this dialog is this:
/// - Most line edits have validators and completers.
/// - Validators are based on regular expressions, serving as raw input validation
/// - The Completers are based off the database and provide auto-completion
/// - mandatory line edits only emit editing finished if their content has passed
///   raw input validation or focus is lost.
/// - Editing finished triggers validating inputs by mapping them to Database values
///   where required and results in either pass or fail.
/// - A QBitArray is mainained containing the state of validity of the mandatory line edits
/// - The deducted entries are automatically filled if all mandatory entries
/// are valid.
/// - Comitting an entry to the database is only allowed if all mandatory inputs are valid.
///
/// if the user presses "OK", check if all mandatory inputs are valid,
/// check if optional user inputs are valid and commit.
///
/// For the completion and mapping, I have settled on a more low-level approach using
/// Completers based on QStringLists and mapping with QMaps.
///
/// I implemented the Completers and mapping based on a QSqlTableModel which would
/// have been quite nice, since it would keep all data in one place, providing both completion
/// and mapping in one model.
/// But as we have seen before with the more high-level qt classes, they are quite slow on execution
/// when used for tasks they were probably not designed to do.
/// Mapping a registration to an ID for example took around 300ms, which is very
/// noticeable in the UI and not an acceptable user experience. Using QStringLists and QMaps
/// this goes down to around 5ms.

NewFlightDialog::NewFlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    setup();
    if (ASettings::read(ASettings::NewFlight::FunctionComboBox).toString() == "SIC") {
        ui->picNameLineEdit->setText(QStringLiteral(""));
        ui->secondPilotNameLineEdit->setText("self");
    }
    if(ASettings::read(ASettings::NewFlight::FunctionComboBox).toString() == "PIC"){
        ui->picNameLineEdit->setText("self");
        emit ui->picNameLineEdit->editingFinished();
    }
}

NewFlightDialog::NewFlightDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    flightEntry = aDB->getFlightEntry(row_id);
    setup();
    formFiller();
}

NewFlightDialog::~NewFlightDialog()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                        Methods - setup and maintenance of dialog                            ///
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

    // Visually mark mandatory fields
    ui->deptLocLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));
    ui->destLocLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));
    ui->tofbTimeLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));
    ui->tonbTimeLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));
    ui->picNameLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));
    ui->acftLineEdit->setStyleSheet(QStringLiteral("border: 0.1ex solid #3daee9"));

    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();
    ui->deptLocLineEdit->setFocus();
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
    ui->calendarCheckBox->setChecked(ASettings::read(ASettings::FlightLogging::PopupCalendar).toBool());

    // Debug
    ASettings::write(ASettings::FlightLogging::FlightTimeFormat, Opl::Time::Default);
    //[F]: Support for Decimal Logging is not implemented yet.
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
    ASettings::write(ASettings::FlightLogging::PopupCalendar, ui->calendarCheckBox->isChecked());
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
    // get Maps
    pilotsIdMap      = aDB->getIdMap(ADatabaseTarget::pilots);
    tailsIdMap       = aDB->getIdMap(ADatabaseTarget::tails);
    airportIcaoIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_icao);
    airportIataIdMap = aDB->getIdMap(ADatabaseTarget::airport_identifier_iata);
    airportNameIdMap = aDB->getIdMap(ADatabaseTarget::airport_names);
    //get Completer Lists
    pilotList   = aDB->getCompletionList(ADatabaseTarget::pilots);
    tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);
    airportList = aDB->getCompletionList(ADatabaseTarget::airport_identifier_all);
    auto tempList = QStringList();
    // define tuples
    const std::tuple<QString, QStringList*, QRegularExpression>
            location_line_edit_settings {QStringLiteral("Loc"), &airportList, LOC_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            name_line_edit_settings {QStringLiteral("Name"), &pilotList, NAME_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            acft_line_edit_settings {QStringLiteral("acft"), &tailsList, AIRCRAFT_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            time_line_edit_settings {QStringLiteral("Time"), &tempList, TIME_VALID_RGX};
    const QList<std::tuple<QString, QStringList*, QRegularExpression>> line_edit_settings = {
        location_line_edit_settings,
        name_line_edit_settings,
        acft_line_edit_settings,
        time_line_edit_settings
    };
    //get line edits, set up completers and validators
    auto line_edits = ui->flightDataTab->findChildren<QLineEdit*>();

    for (const auto &item : line_edit_settings) {
        for (const auto &line_edit : line_edits) {
            if(line_edit->objectName().contains(std::get<0>(item))) {
                DEB << "Setting up: " << line_edit->objectName();
                // Set Validator
                auto validator = new QRegularExpressionValidator(std::get<2>(item), line_edit);
                line_edit->setValidator(validator);
                // Set Completer
                auto completer = new QCompleter(*std::get<1>(item), line_edit);
                completer->setCaseSensitivity(Qt::CaseInsensitive);
                completer->setCompletionMode(QCompleter::PopupCompletion);
                completer->setFilterMode(Qt::MatchContains);
                line_edit->setCompleter(completer);
            }
        }
    }

    // [G]: TODO cleanup
    // populate Mandatory Line Edits list and prepare QBitArray
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
    auto line_edits = this->findChildren<QLineEdit*>();

    for (const auto &line_edit : line_edits){
        line_edit->installEventFilter(this);
        if(line_edit->objectName().contains(QStringLiteral("Loc"))){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onToUpperTriggered_textChanged);
        }
        if(line_edit->objectName().contains(QStringLiteral("acft"))){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onToUpperTriggered_textChanged);
        }
        if(line_edit->objectName().contains(QStringLiteral("Name"))){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onPilotNameLineEdit_editingFinished);
        }
        if(line_edit->objectName().contains(QStringLiteral("Time"))){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onTimeLineEdit_editingFinished);
        }
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    for (const auto &line_edit : qAsConst(mandatoryLineEdits.lineEdits)) {
        if(line_edit->objectName().contains(QStringLiteral("doft")))
            break;
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::highlighted),
                         this, &NewFlightDialog::onCompleter_highlighted);
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::activated),
                         this, &NewFlightDialog::onCompleter_activated);
    }
#endif
}


void NewFlightDialog::setPopUpCalendarEnabled(bool state)
{
    ui->flightDataTabWidget->setCurrentIndex(0);
    ui->flightDataTabWidget->removeTab(2); // hide calendar widget

    if (state) {
        DEB << "Enabling pop-up calendar widget...";
        ui->calendarWidget->installEventFilter(this);
        ui->placeLabel1->installEventFilter(this);
        ui->doftLineEdit->installEventFilter(this);

        QObject::connect(ui->calendarWidget, &QCalendarWidget::clicked,
                         this, &NewFlightDialog::onCalendarWidget_clicked);
        QObject::connect(ui->calendarWidget, &QCalendarWidget::activated,
                         this, &NewFlightDialog::onCalendarWidget_selected);
    } else {
        DEB << "Disabling pop-up calendar widget...";
        ui->calendarWidget->removeEventFilter(this);
        ui->placeLabel1->removeEventFilter(this);
        ui->doftLineEdit->removeEventFilter(this);
        QObject::disconnect(ui->calendarWidget, &QCalendarWidget::clicked,
                            this, &NewFlightDialog::onCalendarWidget_clicked);
        QObject::disconnect(ui->calendarWidget, &QCalendarWidget::activated,
                            this, &NewFlightDialog::onCalendarWidget_selected);
    }
}

bool NewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->doftLineEdit && event->type() == QEvent::MouseButtonPress) {
        onDoftLineEdit_entered();
        return false; // let the event continue to the edit
    }

    auto line_edit = qobject_cast<QLineEdit*>(object);
    if (line_edit != nullptr) {
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::FocusIn) {
            mandatoryLineEdits.unvalidate(line_edit);
            DEB << "Editing " << line_edit->objectName();
            // set verification bit to false when entering a mandatory line edit
            return false;
        }
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::KeyPress) {
            // show completion menu when pressing down arrow
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                DEB << "Key down event.";
                line_edit->completer()->complete();
            }
            return false;
        }
        if (line_edit->objectName().contains("Name") && event->type() == QEvent::KeyPress) {
            // show completion menu when pressing down arrow
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                DEB << "Key down event.";
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
    // check if mandatory line edits are valid
    if (!mandatoryLineEdits.allOk()) {
        return;
    }

    //zero out labels and line edits to delete previous calculations
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit, ui->tSPMETimeLineEdit, ui->tMPTimeLineEdit,    ui->tIFRTimeLineEdit,
                              ui->tNIGHTTimeLineEdit,ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                              ui->tDUALTimeLineEdit, ui->tFITimeLineEdit,};
    QList<QLabel*>      LB = {ui->tSPSELabel, ui->tSPMELabel,  ui->tMPLabel,  ui->tIFRLabel,  ui->tNIGHTLabel,
                              ui->tPICLabel,  ui->tPICUSLabel, ui->tSICLabel, ui->tDUALLabel, ui->tFILabel};
    for(const auto& widget : LE) {widget->setText(EMPTY_STRING);}
    for(const auto& widget : LB) {widget->setText(Opl::Db::NULL_TIME_hhmm);}
    //Calculate block time
    const auto tofb = ATime::fromString(ui->tofbTimeLineEdit->text());
    const auto tonb = ATime::fromString(ui->tonbTimeLineEdit->text());
    const auto tblk = ATime::blocktime(tofb, tonb);
    const auto block_time_string = ATime::toString(tblk, flightTimeFormat);
    const auto block_minutes = ATime::toMinutes(tblk);

    ui->tblkTimeLineEdit->setText(block_time_string);
    // get acft data and fill deductible entries
    auto acft = aDB->getTailEntry(tailsIdMap.value(ui->acftLineEdit->text()));
    if (acft.getData().isEmpty())
        DEB << "Error: No valid aircraft object available, unable to deterime auto times.";


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
    // TOTAL
    ui->tblkLabel->setText("<b>" + block_time_string + "</b>");
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->tIFRTimeLineEdit->setText(block_time_string);
        ui->tIFRLabel->setText(block_time_string);
    }
    // Night
    auto dept_date = ui->doftLineEdit->text() + 'T'
            + ATime::toString(tofb);
    auto dept_date_time = QDateTime::fromString(dept_date, QStringLiteral("yyyy-MM-ddThh:mm"));
    const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();
    auto night_time = ATime::fromMinutes(ACalc::calculateNightTime(
                                             ui->deptLocLineEdit->text(),
                                             ui->destLocLineEdit->text(),
                                             dept_date_time,
                                             block_minutes,
                                             night_angle));

    ui->tNIGHTTimeLineEdit->setText(ATime::toString(night_time, flightTimeFormat));
    ui->tNIGHTLabel->setText(ATime::toString(night_time, flightTimeFormat));
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
}

/*!
 * \brief Collect input and create a Data map for the entry object.
 *
 * This function should only be called if input validation has been passed, since
 * no input validation is done in this step and input data is assumed to be valid.
 * \return
 */
RowData NewFlightDialog::collectInput()
{
    RowData newData;
    DEB << "Collecting Input...";
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
    newData.insert(Opl::Db::FLIGHTS_ACFT, tailsIdMap.value(ui->acftLineEdit->text()));
    // Pilots
    newData.insert(Opl::Db::FLIGHTS_PIC, pilotsIdMap.value(ui->picNameLineEdit->text()));
    newData.insert(Opl::Db::FLIGHTS_SECONDPILOT, pilotsIdMap.value(ui->secondPilotNameLineEdit->text()));
    newData.insert(Opl::Db::FLIGHTS_THIRDPILOT, pilotsIdMap.value(ui->thirdPilotNameLineEdit->text()));

    // Extra Times
    ui->tSPSETimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TSPSE, EMPTY_STRING)
              : newData.insert(Opl::Db::FLIGHTS_TSPSE, stringToMinutes(
                                   ui->tSPSETimeLineEdit->text(), flightTimeFormat));

    ui->tSPMETimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TSPME, EMPTY_STRING)
              : newData.insert(Opl::Db::FLIGHTS_TSPME, stringToMinutes(
                                   ui->tSPMETimeLineEdit->text(), flightTimeFormat));
    ui->tMPTimeLineEdit->text().isEmpty() ?
                newData.insert(Opl::Db::FLIGHTS_TMP, EMPTY_STRING)
              : newData.insert(Opl::Db::FLIGHTS_TMP, stringToMinutes(
                                   ui->tMPTimeLineEdit->text(), flightTimeFormat));

    if (ui->IfrCheckBox->isChecked()) {
        newData.insert(Opl::Db::FLIGHTS_TIFR, block_minutes);
    } else {
        newData.insert(Opl::Db::FLIGHTS_TIFR, EMPTY_STRING);
    }
    // Night
    const auto dept_date = ui->doftLineEdit->text() + 'T'
            + ATime::toString(tofb);
    const auto dept_date_time = QDateTime::fromString(dept_date, QStringLiteral("yyyy-MM-ddThh:mm"));
    const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();
    const auto night_time = ATime::fromMinutes(ACalc::calculateNightTime(
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
        newData.insert(Opl::Db::FLIGHTS_TPICUS, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TSIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TFI, EMPTY_STRING);
        break;
    case 1://PICUS
        newData.insert(Opl::Db::FLIGHTS_TPIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TSIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TFI, EMPTY_STRING);
        break;
    case 2://Co-Pilot
        newData.insert(Opl::Db::FLIGHTS_TPIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TSIC, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TFI, EMPTY_STRING);
        break;
    case 3://Dual
        newData.insert(Opl::Db::FLIGHTS_TPIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TSIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TFI, EMPTY_STRING);
        break;
    case 4://Instructor
        newData.insert(Opl::Db::FLIGHTS_TPIC, block_minutes);
        newData.insert(Opl::Db::FLIGHTS_TPICUS, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TSIC, EMPTY_STRING);
        newData.insert(Opl::Db::FLIGHTS_TDUAL, EMPTY_STRING);
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
            const auto dest_date = ui->doftLineEdit->text() + 'T'
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
    auto line_edits = this->findChildren<QLineEdit *>();
    QStringList line_edits_names;
    for (const auto& le : line_edits) {
        line_edits_names << le->objectName();
    }

    ui->acftLineEdit->setText(flightEntry.getRegistration());
    line_edits_names.removeOne(QStringLiteral("acftLineEdit"));

    for (const auto& data_key : flightEntry.getData().keys()) {
        auto rx = QRegularExpression(data_key + QStringLiteral("LineEdit"));//acftLineEdit
        for(const auto& leName : line_edits_names){
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
        for(const auto& leName : line_edits_names){
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
        for(const auto& leName : line_edits_names){
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
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                auto line_edits = this->findChild<QLineEdit *>(leName);
                if(line_edits != nullptr){
                    DEB << pilotsIdMap.key(1);
                    line_edits->setText(pilotsIdMap.key(flightEntry.getData().value(data_key).toInt()));
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
    if(app != EMPTY_STRING){
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
        ui->tIFRTimeLineEdit->setText(EMPTY_STRING);
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

    for(const auto& le : mandatoryLineEdits.lineEdits){
        emit le->editingFinished();
    }
}

bool NewFlightDialog::isLessOrEqualThanBlockTime(const QString time_string)
{
    if (!mandatoryLineEdits.allOk()){
        QMessageBox message_box(this);
        message_box.setText(tr("Unable to determine total block time.<br>"
                               "Please fill out all Mandatory Fields<br>"
                               "before manually editing these times."));
        message_box.exec();
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
        QMessageBox message_box(this);
        message_box.setWindowTitle(tr("Error"));
        message_box.setText(tr("The flight time you have entered is longer than the total blocktime:"
                               "<br><center><b>%1</b></center>"
                               ).arg(ATime::toString(block_time, flightTimeFormat)));
        message_box.exec();
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
        DEB << "Add new aircraft selected";
        // create and open new aircraft dialog
        NewTailDialog na(ui->acftLineEdit->text(), this);
        na.exec();
        // update map and list, set line edit
        tailsIdMap  = aDB->getIdMap(ADatabaseTarget::tails);
        tailsList   = aDB->getCompletionList(ADatabaseTarget::registrations);

        DEB << "New Entry added. Id:" << aDB->getLastEntry(ADatabaseTarget::tails);
        DEB << "AC Map: " << tailsIdMap;

        parent_line_edit->setText(tailsIdMap.key(aDB->getLastEntry(ADatabaseTarget::tails)));
        emit parent_line_edit->editingFinished();
    } else {
        parent_line_edit->setText(EMPTY_STRING);
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
        DEB << "Add new pilot selected";
        // create and open new pilot dialog
        NewPilotDialog np(this);
        np.exec();
        // update map and list, set line edit
        pilotsIdMap  = aDB->getIdMap(ADatabaseTarget::pilots);
        pilotList    = aDB->getCompletionList(ADatabaseTarget::pilots);
        DEB << "Setting new entry: " << pilotsIdMap.key(aDB->getLastEntry(ADatabaseTarget::pilots));
        parent_line_edit->setText(pilotsIdMap.key(aDB->getLastEntry(ADatabaseTarget::pilots)));
        emit parent_line_edit->editingFinished();
    } else {
        parent_line_edit->setText(EMPTY_STRING);
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
    for (const auto &line_edit : mandatoryLineEdits.lineEdits) {
        emit line_edit->editingFinished();
    }
    DEB << "editing finished emitted. good count: " << mandatoryLineEdits.countOk();
    if (!mandatoryLineEdits.allOk()) {
        QString missing_items;
        for (int i=0; i < mandatoryLineEdits.size(); i++) {
            if (!mandatoryLineEdits.okAt(i)){
                missing_items.append(MANDATORY_LINE_EDITS_DISPLAY_NAMES.value(i) + "<br>");
                mandatoryLineEdits[i]->setStyleSheet(QStringLiteral("border: 1px solid red"));
            }
        }

        QMessageBox message_box(this);
        message_box.setText(tr("Not all mandatory entries are valid.<br>"
                               "The following item(s) are empty or missing:"
                               "<br><br><center><b>%1</b></center><br>"
                               "Please go back and fill in the required data."
                               ).arg(missing_items));
        message_box.exec();
        return;
    }

    DEB << "Submit Button clicked. Mandatory good (out of 8): " << mandatoryLineEdits.countOk();
    auto newData = collectInput();
    DEB << "Setting Data for flightEntry...";
    flightEntry.setData(newData);
    DEB << "Committing...";
    if (!aDB->commit(flightEntry)) {
        QMessageBox message_box(this);
        message_box.setText(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(aDB->lastError.text()));
        message_box.setIcon(QMessageBox::Warning);
        message_box.exec();
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
    line_edit->setStyleSheet("");

    if (mandatoryLineEdits.contains(line_edit))
        mandatoryLineEdits.validate(line_edit);

    if (mandatoryLineEdits.allOk())
        onMandatoryLineEditsFilled();

    DEB << "Mandatory good: " << mandatoryLineEdits.countOk()
        << " (out of 7) " << mandatoryLineEdits.lineEdits;

}

void NewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB << line_edit->objectName() << " - Bad input received - " << line_edit->text();
    line_edit->setStyleSheet(QStringLiteral("border: 1px solid red"));

    DEB << "Mandatory Good: " << mandatoryLineEdits.countOk() << " out of "
        << mandatoryLineEdits.size() << ". Array: " << mandatoryLineEdits.lineEditsOk;
}

// capitalize input for dept, dest and registration input
void NewFlightDialog::onToUpperTriggered_textChanged(const QString &text)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB << "Text changed - " << line_edit->objectName() << line_edit->text();
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}

// update is disabled if the user chose to manually edit extra times
void NewFlightDialog::onMandatoryLineEditsFilled()
{
    if (!mandatoryLineEdits.allOk()) {
        DEB << "erroneously called.";
        return;
    };

    if (updateEnabled)
        fillDeductibleData();
    DEB << mandatoryLineEdits.lineEditsOk;
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
    DEB << line_edit->objectName() << "Editing finished - " << text;

    auto date = QDate::fromString(text, Qt::ISODate);
    if (date.isValid()) {
        label->setText(date.toString(Qt::TextDate));
        onGoodInputReceived(line_edit);
        return;
    }

    //try to correct input if only numbers are entered, eg 20200101
    if(text.length() == 8) {
        DEB << "Trying to fix input...";
        text.insert(4,'-');
        text.insert(7,'-');
        date = QDate::fromString(text, Qt::ISODate);
        if (date.isValid()) {
            line_edit->setText(date.toString(Qt::ISODate));
            label->setText(date.toString(Qt::TextDate));
            onGoodInputReceived(line_edit);
            return;
        }
    }
    label->setText("Invalid Date.");
    onBadInputReceived(line_edit);
}

void NewFlightDialog::onCalendarWidget_clicked(const QDate &date)
{
    const auto& le = ui->doftLineEdit;
    le->blockSignals(false);
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
}

void NewFlightDialog::onCalendarWidget_selected(const QDate &date)
{
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    ui->doftDisplayLabel->setText(date.toString(Qt::TextDate));
    const auto& le = ui->doftLineEdit;
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
    le->blockSignals(false);
}

void NewFlightDialog::onDoftLineEdit_entered()
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

void NewFlightDialog::on_calendarCheckBox_stateChanged(int arg1)
{
    ASettings::write(ASettings::NewFlight::CalendarCheckBox, ui->calendarCheckBox->isChecked());
    DEB << "Calendar check box state changed.";
    switch (arg1) {
    case 0: // unchecked
        setPopUpCalendarEnabled(false);
        break;
    case 2: // checked
        setPopUpCalendarEnabled(true);
        break;
    default:
        break;
    }
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
        airport_id = airportIataIdMap.value(text);
    } else {
        airport_id = airportIcaoIdMap.value(text);
    }
    // check result
    if (airport_id == 0) {
        // to do: prompt user how to handle unknown airport
        name_label->setText(tr("Unknown airport identifier"));
        onBadInputReceived(line_edit);
        return;
    }
    line_edit->setText(airportIcaoIdMap.key(airport_id));
    name_label->setText(airportNameIdMap.key(airport_id));
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
                line_edit->setText(EMPTY_STRING);
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
    auto line_edit = ui->acftLineEdit;
    //DEB << line_edit->objectName() << "Editing Finished!" << line_edit->text());

    if (tailsIdMap.value(line_edit->text()) != 0) {
        DEB << "Mapped: " << line_edit->text() << tailsIdMap.value(line_edit->text());
        auto acft = aDB->getTailEntry(tailsIdMap.value(line_edit->text()));
        ui->acftTypeLabel->setText(acft.type());
        onGoodInputReceived(line_edit);
        return;
    }

    // try to fix input
    if (!line_edit->completer()->currentCompletion().isEmpty()
            && !line_edit->text().isEmpty()) {
        DEB << "Trying to fix input...";
        line_edit->setText(line_edit->completer()->currentCompletion());
        emit line_edit->editingFinished();
        return;
    }

    // to do: promp user to add new
    onBadInputReceived(line_edit);
    ui->acftTypeLabel->setText(tr("Unknown Registration."));
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

    if(line_edit->text().contains(SELF_RX)) {
        DEB << "self recognized.";
        line_edit->setText(pilotsIdMap.key(1));
        auto pilot = aDB->getPilotEntry(1);
        ui->picCompanyLabel->setText(pilot.getData().value(Opl::Db::TAILS_COMPANY).toString());
        onGoodInputReceived(line_edit);
        return;
    }

    if(pilotsIdMap.value(line_edit->text()) != 0) {
        DEB << "Mapped: " << line_edit->text() << pilotsIdMap.value(line_edit->text());
        auto pilot = aDB->getPilotEntry(pilotsIdMap.value(line_edit->text()));
        ui->picCompanyLabel->setText(pilot.getData().value(Opl::Db::TAILS_COMPANY).toString());
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

    // to do: prompt user to add new
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
    if (mandatoryLineEdits.allOk() && updateEnabled)
        onMandatoryLineEditsFilled();
}

void NewFlightDialog::on_manualEditingCheckBox_stateChanged(int arg1)
{
    if (!(mandatoryLineEdits.allOk()) && ui->manualEditingCheckBox->isChecked()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Before editing times manually, please fill out the required fields "
                               "in the flight data tab, so that total time can be calculated."));
        message_box.exec();
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
            le->setStyleSheet("");
        }
        updateEnabled = true;
        if (mandatoryLineEdits.allOk() && updateEnabled)
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
    if(arg1 == QStringLiteral("ILS CAT III")){  //for a CAT III approach an Autoland is mandatory, so we can preselect it.
        ui->AutolandCheckBox->setCheckState(Qt::Checked);
        ui->AutolandSpinBox->setValue(1);
    }else{
        ui->AutolandCheckBox->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox->setValue(0);
    }

    if (arg1 != QStringLiteral("VISUAL"))
        ui->IfrCheckBox->setChecked(true);

    if (arg1 == QStringLiteral("OTHER")) {
        QMessageBox message_box(this);
        message_box.setText(tr("You can specify the approach type in the Remarks field."));
        message_box.exec();
    }
}

void NewFlightDialog::on_FunctionComboBox_currentIndexChanged(int)
{
    if (updateEnabled)
        fillDeductibleData();
}
