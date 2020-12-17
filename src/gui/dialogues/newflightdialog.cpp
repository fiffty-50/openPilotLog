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
#include "src/testing/adebug.h"

using namespace experimental;

/////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
void NewFlightDialog::onInputRejected()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Input Rejected - " << line_edit->text());
}
/////////////////////////////////////// DEBUG /////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                         constants                                           ///
///////////////////////////////////////////////////////////////////////////////////////////////////

static const auto TIME_FORMAT = QLatin1String("hh:mm");
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
}

NewFlightDialog::NewFlightDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    flightEntry = aDB()->getFlightEntry(row_id);
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
    updateEnabled = true;
    setupButtonGroups();
    setupRawInputValidation();
    setupLineEditSignalsAndSlots();
    readSettings();

    // Visually mark mandatory fields
    ui->deptLocLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->destLocLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->tofbTimeLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->tonbTimeLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->picNameLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->acftLineEdit->setStyleSheet("border: 0.1ex solid #3daee9");

    ui->doftLineEdit->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();
    ui->deptLocLineEdit->setFocus();

    if(ASettings::read("NewFlight/FunctionComboBox").toString() == "PIC"){
        ui->picNameLineEdit->setText("self");
        emit ui->picNameLineEdit->editingFinished();
    }
}
void NewFlightDialog::readSettings()
{
    DEB("Reading Settings...");
    QSettings settings;

    ui->FunctionComboBox->setCurrentText(ASettings::read("NewFlight/FunctionComboBox").toString());
    ui->ApproachComboBox->setCurrentText(ASettings::read("NewFlight/ApproachComboBox").toString());
    ui->PilotFlyingCheckBox->setChecked(ASettings::read("NewFlight/PilotFlyingCheckBox").toBool());
    ui->PilotMonitoringCheckBox->setChecked(ASettings::read("NewFlight/PilotMonitoringCheckBox").toBool());
    ui->TakeoffSpinBox->setValue(ASettings::read("NewFlight/TakeoffSpinBox").toInt());
    ui->TakeoffCheckBox->setChecked(ASettings::read("NewFlight/TakeoffCheckBox").toBool());
    ui->LandingSpinBox->setValue(ASettings::read("NewFlight/LandingSpinBox").toInt());
    ui->LandingCheckBox->setChecked(ASettings::read("NewFlight/LandingCheckBox").toBool());
    ui->AutolandSpinBox->setValue(ASettings::read("NewFlight/AutolandSpinBox").toInt());
    ui->AutolandCheckBox->setChecked(ASettings::read("NewFlight/AutolandCheckBox").toBool());
    ui->IfrCheckBox->setChecked(ASettings::read("NewFlight/IfrCheckBox").toBool());
    ui->VfrCheckBox->setChecked(ASettings::read("NewFlight/VfrCheckBox").toBool());
    ui->FlightNumberLineEdit->setText(ASettings::read("flightlogging/flightnumberPrefix").toString());
    ui->calendarCheckBox->setChecked(ASettings::read("NewFlight/calendarCheckBox").toBool());

    setPopUpCalendarEnabled(ASettings::read("NewFlight/calendarCheckBox").toBool());

    if (ASettings::read("NewFlight/FunctionComboBox").toString() == "Co-Pilot") {
        ui->picNameLineEdit->setText("");
        ui->secondPilotNameLineEdit->setText("self");
    }
}

void NewFlightDialog::writeSettings()
{
    DEB("Writing Settings...");

    ASettings::write("NewFlight/FunctionComboBox",ui->FunctionComboBox->currentText());
    ASettings::write("NewFlight/ApproachComboBox",ui->ApproachComboBox->currentText());
    ASettings::write("NewFlight/PilotFlyingCheckBox",ui->PilotFlyingCheckBox->isChecked());
    ASettings::write("NewFlight/PilotMonitoringCheckBox",ui->PilotMonitoringCheckBox->isChecked());
    ASettings::write("NewFlight/TakeoffSpinBox",ui->TakeoffSpinBox->value());
    ASettings::write("NewFlight/TakeoffCheckBox",ui->TakeoffCheckBox->isChecked());
    ASettings::write("NewFlight/LandingSpinBox",ui->LandingSpinBox->value());
    ASettings::write("NewFlight/LandingCheckBox",ui->LandingCheckBox->isChecked());
    ASettings::write("NewFlight/AutolandSpinBox",ui->AutolandSpinBox->value());
    ASettings::write("NewFlight/AutolandCheckBox",ui->AutolandCheckBox->isChecked());
    ASettings::write("NewFlight/IfrCheckBox",ui->IfrCheckBox->isChecked());
    ASettings::write("NewFlight/VfrCheckBox",ui->VfrCheckBox->isChecked());
    ASettings::write("NewFlight/calendarCheckBox",ui->calendarCheckBox->isChecked());
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
    pilotsIdMap      = aDB()->getIdMap(ADataBase::pilots);
    tailsIdMap       = aDB()->getIdMap(ADataBase::tails);
    airportIcaoIdMap = aDB()->getIdMap(ADataBase::airport_identifier_icao);
    airportIataIdMap = aDB()->getIdMap(ADataBase::airport_identifier_iata);
    airportNameIdMap = aDB()->getIdMap(ADataBase::airport_names);
    //get Completer Lists
    pilotList   = aDB()->getCompletionList(ADataBase::pilots);
    tailsList   = aDB()->getCompletionList(ADataBase::registrations);
    airportList = aDB()->getCompletionList(ADataBase::airport_identifier_all);
    auto tempList = QStringList();
    // define tuples
    const std::tuple<QString, QStringList*, QRegularExpression>
            location_line_edit_settings = {"Loc", &airportList, LOC_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            name_line_edit_settings = {"Name", &pilotList, NAME_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            acft_line_edit_settings = {"acft", &tailsList, AIRCRAFT_VALID_RGX};
    const std::tuple<QString, QStringList*, QRegularExpression>
            time_line_edit_settings = {"Time", &tempList, TIME_VALID_RGX};
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
                DEB("Setting up: " << line_edit->objectName());
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
    mandatoryLineEditsGood.resize(mandatoryLineEdits.size());
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

void NewFlightDialog::setupLineEditSignalsAndSlots()
{

    auto line_edits = this->findChildren<QLineEdit*>();

    for (const auto &line_edit : line_edits){
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        QObject::connect(line_edit, &QLineEdit::inputRejected,
                         this, &NewFlightDialog::onInputRejected);
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        line_edit->installEventFilter(this);
        if(line_edit->objectName().contains("Loc")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &NewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("Name")){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onPilotNameLineEdit_editingFinished);
        }
        if(line_edit->objectName().contains("Time")){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &NewFlightDialog::onTimeLineEdit_editingFinished);
        }
    }
    for (const auto &line_edit : mandatoryLineEdits) {
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::highlighted),
                         this, &NewFlightDialog::onCompleterHighlighted);
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::activated),
                         this, &NewFlightDialog::onCompleterActivated);
    }
    QObject::connect(this, &NewFlightDialog::goodInputReceived,
                     this, &NewFlightDialog::onGoodInputReceived);
    QObject::connect(this, &NewFlightDialog::badInputReceived,
                     this, &NewFlightDialog::onBadInputReceived);
    QObject::connect(this, &NewFlightDialog::locationEditingFinished,
                     this, &NewFlightDialog::onLocLineEdit_editingFinished);
    QObject::connect(this, &NewFlightDialog::timeEditingFinished,
                     this, &NewFlightDialog::onTimeLineEdit_editingFinished);
    QObject::connect(this, &NewFlightDialog::mandatoryLineEditsFilled,
                     this, &NewFlightDialog::onMandatoryLineEditsFilled);
}


void NewFlightDialog::setPopUpCalendarEnabled(bool state)
{
    ui->flightDataTabWidget->setCurrentIndex(0);
    ui->flightDataTabWidget->removeTab(2); // hide calendar widget

    if (state) {
        DEB("Enabling pop-up calendar widget...");
        ui->calendarWidget->installEventFilter(this);
        ui->placeLabel1->installEventFilter(this);
        ui->doftLineEdit->installEventFilter(this);

        QObject::connect(ui->calendarWidget, &QCalendarWidget::clicked,
                         this, &NewFlightDialog::onDateClicked);
        QObject::connect(ui->calendarWidget, &QCalendarWidget::activated,
                         this, &NewFlightDialog::onDateSelected);
    } else {
        DEB("Disabling pop-up calendar widget...");
        ui->calendarWidget->removeEventFilter(this);
        ui->placeLabel1->removeEventFilter(this);
        ui->doftLineEdit->removeEventFilter(this);
        QObject::disconnect(ui->calendarWidget, &QCalendarWidget::clicked,
                            this, &NewFlightDialog::onDateClicked);
        QObject::disconnect(ui->calendarWidget, &QCalendarWidget::activated,
                            this, &NewFlightDialog::onDateSelected);
    }
}

bool NewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->doftLineEdit && event->type() == QEvent::MouseButtonPress) {
        onDoftLineEditEntered();
        return false; // let the event continue to the edit
    }

    auto line_edit = qobject_cast<QLineEdit*>(object);
    if (line_edit != nullptr) {
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::FocusIn) {
            mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), false);
            DEB("Editing " << line_edit->objectName());
            // set verification bit to false when entering a mandatory line edit
        }
        if (mandatoryLineEdits.contains(line_edit) && event->type() == QEvent::KeyPress) {
            // show completion menu when pressing down arrow
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Down) {
                DEB("Key down event.");
                line_edit->completer()->complete();
            }
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
    for(const auto& widget : LE) {widget->setText("");}
    for(const auto& widget : LB) {widget->setText("00:00");}
    //Calculate block time
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(), TIME_FORMAT);
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(), TIME_FORMAT);
    QString block_time = ACalc::blocktime(tofb, tonb).toString(TIME_FORMAT);
    QString block_minutes = QString::number(ACalc::stringToMinutes(block_time));
    ui->tblkTimeLineEdit->setText(block_time);
    // get acft data and fill deductible entries
    auto acft = aDB()->getTailEntry(tailsIdMap.value(ui->acftLineEdit->text()));
    DEB("Created acft object with data: " << acft.getData());
    if (acft.getData().isEmpty())
        DEB("No valid aircraft object available.");
    // SP SE
    if(acft.getData().value("singlepilot") == "1" && acft.getData().value("singleengine") == "1"){
        ui->tSPSETimeLineEdit->setText(block_time);
        ui->tSPSELabel->setText(block_time);
    }
    // SP ME
    if(acft.getData().value("singlepilot") == "1" && acft.getData().value("multiengine") == "1"){
        ui->tSPMETimeLineEdit->setText(block_time);
        ui->tSPMELabel->setText(block_time);
    }
    // MP
    if(acft.getData().value("multipilot") == "1"){
        ui->tMPTimeLineEdit->setText(block_time);
        ui->tMPLabel->setText(block_time);
    }
    // TOTAL
    ui->tblkLabel_2->setText("<b>" + block_minutes + "</b>");
    ui->tblkLabel_2->setStyleSheet("color: green;");
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->tIFRTimeLineEdit->setText(block_time);
        ui->tIFRLabel->setText(block_time);
    }
    // Night
    QString dept_date = ui->doftLineEdit->text() + 'T' + tofb.toString(TIME_FORMAT);
    QDateTime dept_date_time = QDateTime::fromString(dept_date,"yyyy-MM-ddThh:mm");
    int tblk = block_minutes.toInt();
    const int night_angle = ASettings::read("flightlogging/nightangle").toInt();
    QString night_minutes = QString::number(
                ACalc::calculateNightTime(
                    ui->deptLocLineEdit->text(),
                    ui->destLocLineEdit->text(),
                    dept_date_time,
                    tblk,
                    night_angle));

    ui->tNIGHTTimeLineEdit->setText(ACalc::minutesToString(night_minutes));
    ui->tNIGHTLabel->setText(ACalc::minutesToString(night_minutes));
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        ui->tPICTimeLineEdit->setText(block_time);
        ui->tPICLabel->setText(block_time);
        break;
    case 1://PICus
        ui->tPICUSTimeLineEdit->setText(block_time);
        ui->tPICUSLabel->setText(block_time);
        break;
    case 2://Co-Pilot
        ui->tSICTimeLineEdit->setText(block_time);
        ui->tSICLabel->setText(block_time);
        break;
    case 3://Dual
        ui->tDUALTimeLineEdit->setText(block_time);
        ui->tDUALLabel->setText(block_time);
        break;
    case 4://Instructor
        ui->tFITimeLineEdit->setText(block_time);
        ui->tFILabel->setText(block_time);
    }
}
/*!
 * \brief Collect input and create a Data map for the entry object.
 *
 * This function should only be called if input validation has been passed, since
 * no input validation is done in this step and input data is assumed to be valid.
 * \return
 */
TableData NewFlightDialog::collectInput()
{
    TableData newData;
    DEB("Collecting Input...");
    // Mandatory data
    newData.insert("doft", ui->doftLineEdit->text());
    newData.insert("dept",ui->deptLocLineEdit->text());
    newData.insert("tofb", QString::number(
                       ACalc::stringToMinutes(ui->tofbTimeLineEdit->text())));
    newData.insert("dest",ui->destLocLineEdit->text());
    newData.insert("tonb", QString::number(
                       ACalc::stringToMinutes(ui->tonbTimeLineEdit->text())));
    //Block Time
    const auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(), TIME_FORMAT);
    const auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(), TIME_FORMAT);
    const QString block_time = ACalc::blocktime(tofb, tonb).toString(TIME_FORMAT);
    const QString block_minutes = QString::number(ACalc::stringToMinutes(block_time));

    newData.insert("tblk", block_minutes);
    // Aircraft
    newData.insert("acft", QString::number(tailsIdMap.value(ui->acftLineEdit->text())));
    // Pilots
    newData.insert("pic", QString::number(pilotsIdMap.value(ui->picNameLineEdit->text())));
    newData.insert("secondPilot", QString::number(pilotsIdMap.value(ui->secondPilotNameLineEdit->text())));
    newData.insert("thirdPilot", QString::number(pilotsIdMap.value(ui->thirdPilotNameLineEdit->text())));

    // Extra Times
    auto acft = ATailEntry(newData.value("acft").toInt());
    if (acft.getData().isEmpty())
        DEB("Invalid Aircraft. Unable to automatically determine extra times.");

    if (acft.getData().value("multipilot") == "1") {
        newData.insert("tSPSE", "");
        newData.insert("tSPME", "");
        newData.insert("tMP", block_minutes);
    } else if (acft.getData().value("singlepilot") == "1" && acft.getData().value("singleengine") == "1") {
        newData.insert("tSPSE", block_minutes);
        newData.insert("tSPME", "");
        newData.insert("tMP", "");
    } else if (acft.getData().value("singlepilot") == "1" && acft.getData().value("multiengine") == "1") {
        newData.insert("tSPSE", "");
        newData.insert("tSPME", block_minutes);
        newData.insert("tMP", "");
    }

    if (ui->IfrCheckBox->isChecked()) {
        newData.insert("tIFR", block_minutes);
    } else {
        newData.insert("tIFR", "");
    }
    // Night
    const auto dept_date = ui->doftLineEdit->text() + 'T' + tofb.toString(TIME_FORMAT);
    const auto dept_date_time = QDateTime::fromString(dept_date,"yyyy-MM-ddThh:mm");
    const auto tblk = block_minutes.toInt();
    const auto night_angle = ASettings::read("flightlogging/nightangle").toInt();
    const auto night_minutes = QString::number(
                ACalc::calculateNightTime(
                    ui->deptLocLineEdit->text(),
                    ui->destLocLineEdit->text(),
                    dept_date_time,
                    tblk,
                    night_angle));
    newData.insert("tNIGHT", night_minutes);

    // Function times - This is a little explicit but these are mutually exclusive so its better to be safe than sorry here.
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        newData.insert("tPIC", block_minutes);
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 1://PICUS
        newData.insert("tPIC", "");
        newData.insert("tPICUS", block_minutes);
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 2://Co-Pilot
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", block_minutes);
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 3://Dual
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", block_minutes);
        newData.insert("tFI", "");
        break;
    case 4://Instructor
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", block_minutes);
    }
    // Pilot Flying
    newData.insert("pilotFlying", QString::number(ui->PilotFlyingCheckBox->isChecked()));
    // TO and LDG - again a bit explicit, but we  need to check for both night to day as well as day to night transitions.
    if (ui->TakeoffCheckBox->isChecked()) {
        if (night_minutes == "0") { // all day
            newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
            newData.insert("toNight", "0");
        } else if (night_minutes == block_minutes) { // all night
            newData.insert("toDay", "0");
            newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
        } else {
            if(ACalc::isNight(ui->deptLocLineEdit->text(), dept_date_time,  night_angle)) {
                newData.insert("toDay", "0");
                newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
            } else {
                newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
                newData.insert("toNight", "0");
            }
        }
    } else {
        newData.insert("toDay", "0");
        newData.insert("toNight", "0");
    }

    if (ui->LandingCheckBox->isChecked()) {
        if (night_minutes == "0") { // all day
            newData.insert("ldgDay", QString::number(ui->LandingSpinBox->value()));
            newData.insert("ldgNight", "0");
        } else if (night_minutes == block_minutes) { // all night
            newData.insert("ldgDay", "0");
            newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
        } else { //check
            const auto dest_date = ui->doftLineEdit->text() + 'T' + tonb.toString(TIME_FORMAT);
            const auto dest_date_time = QDateTime::fromString(dest_date,"yyyy-MM-ddThh:mm");
            if (ACalc::isNight(ui->destLocLineEdit->text(), dest_date_time,  night_angle)) {
                newData.insert("ldgDay", "0");
                newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
            } else {
                newData.insert("ldgDay", QString::number(ui->LandingSpinBox->value()));
                newData.insert("ldgNight", "0");
            }
        }
    } else {
        newData.insert("ldgDay", "0");
        newData.insert("ldgNight", "0");
    }


    newData.insert("autoland", QString::number(ui->AutolandSpinBox->value()));
    newData.insert("ApproachType", ui->ApproachComboBox->currentText());
    newData.insert("FlightNumber", ui->FlightNumberLineEdit->text());
    newData.insert("Remarks", ui->RemarksLineEdit->text());

    return newData;
}

void NewFlightDialog::formFiller()
{
    DEB("Filling Line Edits...");
    DEB("With Data: " << flightEntry.getData());
    // Date
    //ui->doftLineEdit->setDate(QDate::fromString(flightEntry.getData().value("doft"), Qt::ISODate));
    QStringList filled;

    // get Line Edits
    auto line_edits = this->findChildren<QLineEdit *>();
    QStringList line_edits_names;
    for (const auto& le : line_edits) {
        line_edits_names << le->objectName();
    }

    ui->acftLineEdit->setText(flightEntry.getRegistration());
    line_edits_names.removeOne("acftLineEdit");

    for (const auto& data_key : flightEntry.getData().keys()) {
        auto rx = QRegularExpression(data_key + "LineEdit");//acftLineEdit
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Loc Match found: " << key << " - " << leName);
                auto le = this->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(flightEntry.getData().value(data_key));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + "Loc\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Loc Match found: " << key << " - " << leName);
                auto le = this->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(flightEntry.getData().value(data_key));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + "Time\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Time Match found: " << key << " - " << leName);
                auto le = this->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    DEB("Setting " << le->objectName() << " to " << ACalc::minutesToString(flightEntry.getData().value(data_key)));
                    le->setText(ACalc::minutesToString(
                                    flightEntry.getData().value(data_key)));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(data_key + "Name\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                DEB("Name Match found: " << data_key << " - " << leName);
                auto le = this->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    DEB(pilotsIdMap.key(1));
                    DEB("Matching name..." << data_key << " to " << flightEntry.getData().value(data_key).toInt());
                    le->setText(pilotsIdMap.key(flightEntry.getData().value(data_key).toInt()));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
    }
    //FunctionComboBox
    QList<QLineEdit*> FCB = {ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                             ui->tDUALTimeLineEdit, ui->tFITimeLineEdit};
    for(const auto& le : FCB){
        if(le->text() != "00:00"){
            QString name = le->objectName();
            name.chop(12);
            name.remove(0,1);
            ui->FunctionComboBox->setCurrentText(name);
        }
    }
    // Approach Combo Box
    const QString& app = flightEntry.getData().value("ApproachType");
    if(app != " "){
        ui->ApproachComboBox->setCurrentText(app);
    }
    // Task and Rules
    qint8 PF = flightEntry.getData().value("pilotFlying").toInt();
    if (PF > 0) {
        ui->PilotFlyingCheckBox->setChecked(true);
    } else {
        ui->PilotMonitoringCheckBox->setChecked(true);
    }
    qint8 FR = flightEntry.getData().value("tIFR").toInt();
    if (FR > 0) {
        ui->IfrCheckBox->setChecked(true);
    } else {
        ui->tIFRTimeLineEdit->setText("00:00");
        ui->VfrCheckBox->setChecked(true);
    }
    // Take Off and Landing
    qint8 TO = flightEntry.getData().value("toDay").toInt() + flightEntry.getData().value("toNight").toInt();
    qint8 LDG = flightEntry.getData().value("ldgDay").toInt() + flightEntry.getData().value("ldgNight").toInt();
    DEB("TO and LDG:" << TO << LDG);
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
    qint8 AL = flightEntry.getData().value("autoland").toInt();
    if(AL > 0) {
        ui->AutolandCheckBox->setChecked(true);
        ui->AutolandSpinBox->setValue(AL);
    }

    for(const auto& le : mandatoryLineEdits){
        emit le->editingFinished();
    }
}

bool NewFlightDialog::isLessOrEqualThanBlockTime(const QString time_string)
{
    if (mandatoryLineEditsGood.count(true) != 7){
        auto message_box = QMessageBox(this);
        message_box.setText("Unable to determine total block time.\n"
                            "Please fill out Departure and Arrival Time\n"
                            "before manually editing these times.");
        message_box.exec();
        return false;
    }

    auto extra_time = QTime::fromString(time_string,TIME_FORMAT);
    auto block_time = ACalc::blocktime(QTime::fromString(
                                           ui->tofbTimeLineEdit->text(),TIME_FORMAT),
                                       QTime::fromString(
                                           ui->tonbTimeLineEdit->text(), TIME_FORMAT));
    if (extra_time <= block_time) {
        return true;
    } else {
        auto message_box = QMessageBox(this);
        message_box.setText("Cannot be more than Total Time of Flight:<br><br><center><b>"
                            + block_time.toString(TIME_FORMAT)
                            + "</b></center><br>");
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
    reply = QMessageBox::question(this, "No Aircraft found",
                                  "No aircraft with this registration found.<br>"
                                  "If this is the first time you log a flight with this aircraft, you have to "
                                  "add the registration to the database first.<br><br>Would you like to add a new aircraft to the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DEB("Add new aircraft selected");
        // create and open new aircraft dialog
        auto na = NewTailDialog(ui->acftLineEdit->text(), this);
        na.exec();
        // update map and list, set line edit
        tailsIdMap  = aDB()->getIdMap(ADataBase::tails);
        tailsList   = aDB()->getCompletionList(ADataBase::registrations);

        parent_line_edit->setText(tailsIdMap.key(aDB()->getLastEntry(ADataBase::tails)));
        emit parent_line_edit->editingFinished();
    } else {
        parent_line_edit->setText("");
    }
}

/*!
 * \brief NewFlight::addNewPilot If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlightDialog::addNewPilot(QLineEdit *parent_line_edit)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "No Pilot found",
                                  "No pilot found.<br>Please enter the Name as"
                                  "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
                                  "If this is the first time you log a flight with this pilot, you have to "
                                  "add the name to the database first.<br><br>Would you like to add a new pilot to the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        DEB("Add new pilot selected");
        // create and open new pilot dialog
        auto np = NewPilotDialog(this);
        np.exec();
        // update map and list, set line edit
        pilotsIdMap  = aDB()->getIdMap(ADataBase::pilots);
        pilotList    = aDB()->getCompletionList(ADataBase::pilots);
        DEB("Setting new entry: " << pilotsIdMap.key(aDB()->getLastEntry(ADataBase::pilots)));
        parent_line_edit->setText(pilotsIdMap.key(aDB()->getLastEntry(ADataBase::pilots)));
        emit parent_line_edit->editingFinished();
    } else {
        parent_line_edit->setText("");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                               Flight Data Tab Slots                                         ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void NewFlightDialog::on_cancelButton_clicked()
{
    DEB("Cancel Button clicked.");
    reject();
}

// submit button is set enabled when all mandatory line edits are valid
void NewFlightDialog::on_submitButton_clicked()
{
    for (const auto &line_edit : mandatoryLineEdits) {
        emit line_edit->editingFinished();
    }
    DEB("editing finished emitted. good count: " << mandatoryLineEditsGood.count(true));
    if (mandatoryLineEditsGood.count(true) != 7) {
        auto message_box = QMessageBox(this);
        message_box.setText("Not all mandatory entries are valid");
        message_box.exec();
        return;
    }

    DEB("Submit Button clicked. Mandatory good (out of 7): " << mandatoryLineEditsGood.count(true));
    auto newData = collectInput();
    DEB("Setting Data for flightEntry...");
    flightEntry.setData(newData);
    DEB("Committing...");
    if (!aDB()->commit(flightEntry)) {
        auto message_box = QMessageBox(this);
        message_box.setText("An error has ocurred. Your entry has not been saved.");
        message_box.setIcon(QMessageBox::Warning);
        message_box.exec();
        return;
        /// [F] To do: get error info and display here.
    }
    QDialog::accept();
}



/*
 * Shared Slots
 */

void NewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Good input received - " << line_edit->text());
    line_edit->setStyleSheet("");

    if (mandatoryLineEdits.contains(line_edit))
        mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), true);

    if (mandatoryLineEditsGood.count(true) == 7)
        emit mandatoryLineEditsFilled();

    DEB("Mandatory good: " << mandatoryLineEditsGood.count(true)
        << " (out of 7) " << mandatoryLineEditsGood);

}

void NewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
{
    DEB(line_edit->objectName() << " - Bad input received - " << line_edit->text());
    line_edit->setStyleSheet("border: 1px solid red");

    if (mandatoryLineEdits.contains(line_edit)) {
        mandatoryLineEditsGood.setBit(mandatoryLineEdits.indexOf(line_edit), false);
        DEB("Disabling commit button...");
        ui->submitButton->setEnabled(false);
    }

    DEB("Mandatory Good: " << mandatoryLineEditsGood.count(true) << " out of "
        << mandatoryLineEditsGood.size() << ". Array: " << mandatoryLineEditsGood);
}

// capitalize input for dept, dest and registration input
void NewFlightDialog::onTextChangedToUpper(const QString &text)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB("Text changed - " << line_edit->objectName() << line_edit->text());
    {
        const QSignalBlocker blocker(line_edit);
        line_edit->setText(text.toUpper());
    }
}

// update is disabled if the user chose to manually edit extra times
void NewFlightDialog::onMandatoryLineEditsFilled()
{
    DEB(mandatoryLineEditsGood);
    if(!ui->submitButton->isEnabled()) {
        ui->submitButton->setEnabled(true);
        ui->submitButton->setStyleSheet("border: 1px solid rgb(78, 154, 6);"
                                        "color: rgb(78, 154, 6);");
        DEB("Commit Button Enabled. Ready for commit!");
    }

    if (updateEnabled)
        fillDeductibleData();
    DEB(mandatoryLineEditsGood);
}

// make sure that when using keyboard to scroll through completer sugggestions, line edit is up to date
void NewFlightDialog::onCompleterHighlighted(const QString &completion)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB("Completer highlighted - " << line_edit->objectName() << completion);
    line_edit->setText(completion);
}

void NewFlightDialog::onCompleterActivated(const QString &)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB("Line edit " << line_edit->objectName() << "completer activated.");
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
    DEB(line_edit->objectName() << "Editing finished - " << text);

    auto date = QDate::fromString(text, Qt::ISODate);
    if (date.isValid()) {
        label->setText(date.toString(Qt::TextDate));
        emit goodInputReceived(line_edit);
        return;
    }

    //try to correct input if only numbers are entered, eg 20200101
    if(text.length() == 8) {
        DEB("Trying to fix input...");
        text.insert(4,'-');
        text.insert(7,'-');
        date = QDate::fromString(text, Qt::ISODate);
        if (date.isValid()) {
            line_edit->setText(date.toString(Qt::ISODate));
            label->setText(date.toString(Qt::TextDate));
            emit goodInputReceived(line_edit);
            return;
        }
    }
    label->setText("Invalid Date.");
    emit badInputReceived(line_edit);
}

void NewFlightDialog::onDateClicked(const QDate &date)
{
    const auto& le = ui->doftLineEdit;
    le->blockSignals(false);
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
}

void NewFlightDialog::onDateSelected(const QDate &date)
{
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    ui->doftDisplayLabel->setText(date.toString(Qt::TextDate));
    const auto& le = ui->doftLineEdit;
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
    le->blockSignals(false);
}

void NewFlightDialog::onDoftLineEditEntered()
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
    ASettings::write("NewFlight/calendarCheckBox", ui->calendarCheckBox->isChecked());

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
    emit locationEditingFinished(ui->deptLocLineEdit, ui->deptNameLabel);
}

void NewFlightDialog::on_destLocLineEdit_editingFinished()
{
    emit locationEditingFinished(ui->destLocLineEdit, ui->destNameLabel);
}

void NewFlightDialog::onLocLineEdit_editingFinished(QLineEdit *line_edit, QLabel *name_label)
{
    const auto &text = line_edit->text();
    //DEB(line_edit->objectName() << " Editing finished. " << text);
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
        name_label->setText("Unknown airport identifier");
        emit badInputReceived(line_edit);
        return;
    }
    line_edit->setText(airportIcaoIdMap.key(airport_id));
    name_label->setText(airportNameIdMap.key(airport_id));
    emit goodInputReceived(line_edit);
}

/*
 * Time Line Edits
 */

void NewFlightDialog::onTimeLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    //DEB(line_edit->objectName() << "Editing Finished -" << line_edit->text());

    line_edit->setText(ACalc::formatTimeInput(line_edit->text()));
    const auto time = QTime::fromString(line_edit->text(),TIME_FORMAT);
    if(time.isValid()){
        if(primaryTimeLineEdits.contains(line_edit)) {
            emit goodInputReceived(line_edit);
        } else { // is extra time line edit

        }
    } else {
        emit badInputReceived(line_edit);
    }
}

/*
 * Aircraft Line Edit
 */

void NewFlightDialog::on_acftLineEdit_editingFinished()
{
    auto line_edit = ui->acftLineEdit;
    //DEB(line_edit->objectName() << "Editing Finished!" << line_edit->text());

    if (tailsIdMap.value(line_edit->text()) != 0) {
        DEB("Mapped: " << line_edit->text() << tailsIdMap.value(line_edit->text()));
        auto acft = aDB()->getTailEntry(tailsIdMap.value(line_edit->text()));
        ui->acftTypeLabel->setText(acft.type());
        emit goodInputReceived(line_edit);
        return;
    }

    // try to fix input
    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB("Trying to fix input...");
        line_edit->setText(line_edit->completer()->currentCompletion());
        emit line_edit->editingFinished();
        return;
    }

    // to do: promp user to add new
    emit badInputReceived(line_edit);
    ui->acftTypeLabel->setText("Unknown Registration.");
    addNewTail(line_edit);
}

/*
 * Pilot Line Edits
 */

void NewFlightDialog::onPilotNameLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    //DEB(line_edit->objectName() << "Editing Finished -" << line_edit->text());

    if(line_edit->text().contains(SELF_RX)) {
        DEB("self recognized.");
        line_edit->setText(pilotsIdMap.key(1));
        auto pilot = aDB()->getPilotEntry(1);
        ui->picCompanyLabel->setText(pilot.getData().value("company"));
        emit goodInputReceived(line_edit);
        return;
    }

    if(pilotsIdMap.value(line_edit->text()) != 0) {
        DEB("Mapped: " << line_edit->text() << pilotsIdMap.value(line_edit->text()));
        auto pilot = aDB()->getPilotEntry(pilotsIdMap.value(line_edit->text()));
        ui->picCompanyLabel->setText(pilot.getData().value("company"));
        emit goodInputReceived(line_edit);
        return;
    }

    if (line_edit->text().isEmpty()) {
        return;
    }

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB("Trying to fix input...");
        line_edit->setText(line_edit->completer()->currentCompletion());
        emit line_edit->editingFinished();
        return;
    }

    // to do: prompt user to add new
    emit badInputReceived(line_edit);
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
    DEB("PF checkbox state changed.");
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
    if (mandatoryLineEditsGood.count(true) == 7 && updateEnabled)
        emit mandatoryLineEditsFilled();
}

void NewFlightDialog::on_manualEditingCheckBox_stateChanged(int arg1)
{
    if (!(mandatoryLineEditsGood.count(true) == 7) && ui->manualEditingCheckBox->isChecked()) {
        auto message_box = QMessageBox(this);
        message_box.setText("Before editing times manually, please fill out the required fields in the flight data tab,"
                            " so that total time can be calculated.");
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
        if (mandatoryLineEditsGood.count(true) == 7 && updateEnabled)
            emit mandatoryLineEditsFilled();
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
    if(arg1 == "ILS CAT III"){  //for a CAT III approach an Autoland is mandatory, so we can preselect it.
        ui->AutolandCheckBox->setCheckState(Qt::Checked);
        ui->AutolandSpinBox->setValue(1);
    }else{
        ui->AutolandCheckBox->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox->setValue(0);
    }
}

void NewFlightDialog::on_FunctionComboBox_currentIndexChanged(int)
{
    if (updateEnabled)
        fillDeductibleData();
}
