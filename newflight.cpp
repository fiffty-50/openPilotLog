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
#include "newflight.h"
#include "ui_newflight.h"


/// =======================================================
/// Debug / WIP section
/// =======================================================
#define DEBUG(expr) \
    qDebug() << "~DEBUG" << __func__ << expr

void NewFlight::on_verifyButton_clicked()//debug button
{
    //fillExtrasLineEdits();
    //qDebug() << testlist;
    qDebug() << parent();
}
/*!
 * \brief NewFlight::nope for features that are not yet implemented
 */
void NewFlight::nope()
{
    QMessageBox nope(this); //error box
    nope.setText("This feature is not yet available!");
    nope.exec();
}

/// =======================================================


/// Initialising variables used for storing user input
/// Variables are initalised invalid to later fill them with
/// meaningful inputs once they have been validated
QVector<QString>    flight;
QDate               date(QDate::currentDate());
QString             doft(QDate::currentDate().toString(Qt::ISODate));
QString             dept                =   "INVA";
QString             dest                =   "INVA";
QTime               tofb;
QTime               tonb;
QTime               tblk;
QString             pic                 =   "-1";
QString             acft                =   "-1";
// extras
QString             secondPilot         =   "-1";
QString             thirdPilot          =   "-1";
QString             pilotFunction       =   "-1";
QString             pilotTask           =   "-1";
QString             takeoff             =   "0";
QString             landing             =   "0";
QString             autoland            =   "0";
QString             approachType        =   "-1";
// extra times
QString             tSPSE               =   "00:00";
QString             tSPME               =   "00:00";
QString             tMP                 =   "00:00";
/* If on submitting, not all checks are passed, user input is stored
 * in the scratchpad table to later re-fill the form enabling correction.*/
bool                hasOldInput         = dbFlight::checkScratchpad();

/// Raw Input validation
const QString TIME_REGEX_PATTERN        =   "([01]?[0-9]?|2[0-3]):?[0-5][0-9]?";// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
const QString IATA                      =   "[a-zA-Z0-9]{3}";
const QString ICAO                      =   "[a-zA-Z0-9]{4}";
const QString LOC_REGEX_PATTERN         =   IATA + "|" + ICAO;
const QString AIRCRAFT_REGEX_PATTERN    =   "[\\w0-9]+-?([\\w0-9]?)+";
const QString PILOT_NAME_REGEX_PATTERN  =   "[\\w]+,? ?[\\w]+";

/// Invalid characters (validators keep text even if it returns Invalid, see `onInputRejected` below)
const QString TIME_INVALID_RGX          =   "[^\\d:]";
const QString LOC_INVALID_RGX           =   "[^a-zA-Z0-9]";
const QString AIRCRAFT_INVALID_RGX      =   "[^A-Z0-9\\-]";
const QString PILOT_NAME_INVALID_RGX    =   "[^a-zA-Z, ]";

/// Input max lengths
const qint8 TIME_MAX_LENGTH             = 5; //to allow for ':' e.g. "08:45"
const qint8 LOC_MAX_LENGTH              = 4;
const qint8 AIRCRAFT_MAX_LENGTH         = 10;
const qint8 PILOT_NAME_MAX_LENGTH       = 15;


/*!
 * \brief setLineEditValidator set Validators for QLineEdits that end with Time, Loc,
 * Aircraft or Name
 */
static inline void setupLineEdit(QLineEdit* line_edit, QVector<QStringList> completionLists)
{
    auto line_edit_objectName = line_edit->objectName();
    DEBUG("Setting validators for " << line_edit_objectName);
    static const
    QVector<std::tuple<QRegularExpression, QRegularExpression,
            qint8, QStringList>> objectName_inputValidation_rgxs = {
        {QRegularExpression("\\w+Time"),    QRegularExpression(TIME_REGEX_PATTERN),
         TIME_MAX_LENGTH,                   completionLists[0]},
        {QRegularExpression("\\w+Loc"),     QRegularExpression(LOC_REGEX_PATTERN),
         LOC_MAX_LENGTH,                    completionLists[1]},
        {QRegularExpression("\\w+Acft"),    QRegularExpression(AIRCRAFT_REGEX_PATTERN),
         AIRCRAFT_MAX_LENGTH,               completionLists[2]},
        {QRegularExpression("\\w+Name"),    QRegularExpression(PILOT_NAME_REGEX_PATTERN),
         PILOT_NAME_MAX_LENGTH,             completionLists[3]},
    };

    auto validator = new StrictRegularExpressionValidator();
    for(auto tuple : objectName_inputValidation_rgxs)
    {
        auto objName_rgx = std::get<0>(tuple);
        auto input_rgx = std::get<1>(tuple);
        auto max_length = std::get<2>(tuple);
        auto completer_list = std::get<3>(tuple);
        if(objName_rgx.match(line_edit_objectName).hasMatch())
        {
            validator->setRegularExpression(input_rgx);
            line_edit->setValidator(validator);
            line_edit->setMaxLength(max_length);
            QCompleter* completer = new QCompleter(completer_list, line_edit);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setCompletionMode(QCompleter::PopupCompletion);
            if(objName_rgx != QRegularExpression("\\w+Loc")){
                completer->setFilterMode(Qt::MatchContains);
            }
            line_edit->setCompleter(completer);
            return;
        }
    }
    DEBUG("Couldnt find QLineEdit" << line_edit_objectName);
}

/*!
 * \brief NewFlight::storeSettings Commits current selection for auto-logging
 * to the database.
 */
void NewFlight::storeSettings()
{
    qDebug() << "Storing Settings...";
    dbSettings::storeSetting(100, ui->FunctionComboBox->currentText());
    dbSettings::storeSetting(101, ui->ApproachComboBox->currentText());
    dbSettings::storeSetting(102, QString::number(ui->PilotFlyingCheckBox->isChecked()));
    dbSettings::storeSetting(103, QString::number(ui->PilotMonitoringCheckBox->isChecked()));
    dbSettings::storeSetting(104, QString::number(ui->TakeoffSpinBox->value()));
    dbSettings::storeSetting(105, QString::number(ui->TakeoffCheckBox->isChecked()));
    dbSettings::storeSetting(106, QString::number(ui->LandingSpinBox->value()));
    dbSettings::storeSetting(107, QString::number(ui->LandingCheckBox->isChecked()));
    dbSettings::storeSetting(108, QString::number(ui->AutolandSpinBox->value()));
    dbSettings::storeSetting(109, QString::number(ui->AutolandCheckBox->isChecked()));
    dbSettings::storeSetting(110, QString::number(ui->IfrCheckBox->isChecked()));
    dbSettings::storeSetting(111, QString::number(ui->VfrCheckBox->isChecked()));
}
/*!
 * \brief NewFlight::restoreSettings Retreives auto-logging settings from database
 * and sets up ui accordingly
 */
void NewFlight::restoreSettings()
{
    qDebug() << "Restoring Settings...";//crashes if db is empty due to QVector index out of range.
    ui->FunctionComboBox->setCurrentText(dbSettings::retreiveSetting(100));
    ui->ApproachComboBox->setCurrentText(dbSettings::retreiveSetting(101));
    ui->PilotFlyingCheckBox->setChecked(dbSettings::retreiveSetting(102).toInt());
    ui->PilotMonitoringCheckBox->setChecked(dbSettings::retreiveSetting(103).toInt());
    ui->TakeoffSpinBox->setValue(dbSettings::retreiveSetting(104).toInt());
    ui->TakeoffCheckBox->setChecked(dbSettings::retreiveSetting(105).toInt());
    ui->LandingSpinBox->setValue(dbSettings::retreiveSetting(106).toInt());
    ui->LandingCheckBox->setChecked(dbSettings::retreiveSetting(107).toInt());
    ui->AutolandSpinBox->setValue(dbSettings::retreiveSetting(108).toInt());
    ui->AutolandCheckBox->setChecked(dbSettings::retreiveSetting(109).toInt());
    ui->IfrCheckBox->setChecked(dbSettings::retreiveSetting(110).toInt());
    ui->VfrCheckBox->setChecked(dbSettings::retreiveSetting(111).toInt());
    ui->flightNumberPrefixLabel->setText(dbSettings::retreiveSetting(50) + QLatin1Char('-'));
}

/*
 * Window Construction
 */

NewFlight::NewFlight(QWidget *parent, QVector<QStringList> completionLists) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);

    // Set up Line Edits with QValidators, QCompleters and set Max length
    auto line_edits = ui->flightDataTab->findChildren<QLineEdit*>() +
                ui->extraTimes->findChildren<QLineEdit*>();
        for(auto line_edit : line_edits)
        {
            setupLineEdit(line_edit, completionLists);
        }


    // Groups for CheckBoxes
    QButtonGroup *FlightRulesGroup = new QButtonGroup(this);
    FlightRulesGroup->addButton(ui->IfrCheckBox);
    FlightRulesGroup->addButton(ui->VfrCheckBox);

    QButtonGroup *PilotTaskGroup = new QButtonGroup(this);
    PilotTaskGroup->addButton(ui->PilotFlyingCheckBox);
    PilotTaskGroup->addButton(ui->PilotMonitoringCheckBox);

    ui->deptTZ->setFocusPolicy(Qt::NoFocus);
    ui->destTZ->setFocusPolicy(Qt::NoFocus);
    ui->newDoft->setDate(QDate::currentDate());

    // Visually mark mandatory fields
    ui->newDeptLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->newDestLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->newDeptTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->newDestTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->newPicNameLineEdit->setStyleSheet("border: 1px solid orange");
    ui->newAcft->setStyleSheet("border: 1px solid orange");

    restoreSettings();  // settings for auto-logging are stored in the database.
    //Restore inputs if commiting to DB has been rejected.
    qDebug() << "Hasoldinput? = " << hasOldInput;
    if(hasOldInput) // Re-populate the Form
    {
        flight = dbFlight::retreiveScratchpad();
        qDebug() << "Re-Filling Form from Scratchpad";
        returnInput(flight);
    }
    ui->newDeptLocLineEdit->setFocus();
}

NewFlight::~NewFlight()
{
    delete ui;
}

/*!
 * ============================================================================
 * Slots
 * ============================================================================
 */

/*!
 * \brief onInputRejected Set `line_edit`'s border to red and check if `rgx` matches
 * in order to keep text on line.
 */
static void onInputRejected(QLineEdit* line_edit, QRegularExpression rgx){
    DEBUG("Input rejected" << line_edit->text());
    line_edit->setStyleSheet("border: 1px solid red");
    if(auto text = line_edit->text();
            rgx.match(text).hasMatch() == false)
    {
        line_edit->setText(line_edit->text());
    }
}

/*!
 * \brief onEditingFinished signal is emitted if input passed raw validation
 */
static void onEditingFinished(QLineEdit* line_edit){
    DEBUG("Input accepted" << line_edit->text() << line_edit->metaObject()->className());
    line_edit->setStyleSheet("");
}

void NewFlight::on_deptTZ_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Local"){nope();}  // currently only UTC time logging is supported
    ui->deptTZ->setCurrentIndex(0);
}

void NewFlight::on_destTZ_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "Local"){nope();}  // currently only UTC time logging is supported
    ui->destTZ->setCurrentIndex(0);
}



/// Departure

void NewFlight::on_newDeptLocLineEdit_inputRejected()
{
    ui->newDeptLocLineEdit->setText(ui->newDeptLocLineEdit->text().toUpper());
    onInputRejected(ui->newDeptLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlight::on_newDeptLocLineEdit_textEdited(const QString &arg1)
{
    ui->newDeptLocLineEdit->setText(arg1.toUpper());
}

void NewFlight::on_newDeptLocLineEdit_editingFinished()
{
    QStringList locationList = dbAirport::retreiveIataIcaoList(); //To be moved outside of dialog eventually
    //DEBUG(locationList);
    auto line_edit = ui->newDeptLocLineEdit;
    onEditingFinished(line_edit); //reset style sheet
    dept = line_edit->text();

    // check if iata exists, replace with icao code if it does.
    if(dept.length() == 3){
        int index = locationList.indexOf(dept);
        if(index == -1){// Not in locationList
            DEBUG("Airport not found.");
            emit line_edit->inputRejected();
        }else{
            dept = locationList[index -1];
        }
    }
    // Check if 4-letter code is in locationList
    if(dept.length() == 4 && locationList.indexOf(dept) == -1){
        DEBUG("Airport not found.");
        emit line_edit->inputRejected();
    }

    line_edit->setText(dept);
    DEBUG("Departure set: " << dept);
}

void NewFlight::on_newDeptTimeLineEdit_inputRejected()
{
    onInputRejected(ui->newDeptTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlight::on_newDeptTimeLineEdit_editingFinished()
{
    ui->newDeptTimeLineEdit->setText(calc::formatTimeInput(ui->newDeptTimeLineEdit->text()));
    tofb = QTime::fromString(ui->newDeptTimeLineEdit->text(),"hh:mm");

    auto line_edit = ui->newDeptTimeLineEdit;
    onEditingFinished(line_edit);

    if(tofb.isValid()){
        // continue
        DEBUG("Time Off Blocks is valid:" << tofb);
    }else{
        emit line_edit->inputRejected();
    }
}

/// Destination

void NewFlight::on_newDestLocLineEdit_inputRejected()
{
    ui->newDestLocLineEdit->setText(ui->newDestLocLineEdit->text().toUpper());
    onInputRejected(ui->newDestLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlight::on_newDestLocLineEdit_textEdited(const QString &arg1)
{
    ui->newDestLocLineEdit->setText(arg1.toUpper());
}

void NewFlight::on_newDestLocLineEdit_editingFinished()
{
    QStringList locationList = dbAirport::retreiveIataIcaoList(); //To be moved outside of dialog eventually

    auto line_edit = ui->newDestLocLineEdit;
    onEditingFinished(line_edit); //reset style sheet
    dest = line_edit->text();

    // check if iata exists, replace with icao code if it does.
    if(dest.length() == 3){
        int index = locationList.indexOf(dest);
        if(index == -1){// Not in locationList
            DEBUG("Airport not found.");
            emit line_edit->inputRejected();
        }else{
            dest = locationList[index -1];
        }
    }
    // Check if 4-letter code is in locationList
    if(dest.length() == 4 && locationList.indexOf(dest) == -1){
        DEBUG("Airport not found.");
        emit line_edit->inputRejected();
    }

    line_edit->setText(dest);
    DEBUG("Destination set: " << dest);
}

void NewFlight::on_newDestTimeLineEdit_inputRejected()
{
    onInputRejected(ui->newDestTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlight::on_newDestTimeLineEdit_editingFinished()
{
    ui->newDestTimeLineEdit->setText(calc::formatTimeInput(ui->newDestTimeLineEdit->text()));
    tonb = QTime::fromString(ui->newDestTimeLineEdit->text(),"hh:mm");

    auto line_edit = ui->newDestTimeLineEdit;
    onEditingFinished(line_edit);

    if(tonb.isValid()){
        // continue
        DEBUG("Time On Blocks is valid:" << tonb);
    }else{
        emit line_edit->inputRejected();
    }
}

/// Date

void NewFlight::on_newDoft_editingFinished()
{
    date = ui->newDoft->date();
    doft = date.toString(Qt::ISODate);
}

/// Aircraft

void NewFlight::on_newAcft_inputRejected()
{
    onInputRejected(ui->newAcft, QRegularExpression(AIRCRAFT_INVALID_RGX));
}

void NewFlight::on_newAcft_editingFinished()
{
    acft = "-1";// set invalid

    auto registrationList = dbAircraft::retreiveRegistrationList();
    auto line_edit = ui->newAcft;

    QStringList match = registrationList.filter(line_edit->text(), Qt::CaseInsensitive);

    if(match.length() != 0) {
        acft = match[0];
        line_edit->setText(acft);
        onEditingFinished(line_edit);
    }else{
        DEBUG("Registration not in List!");
        emit line_edit->inputRejected();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "No aircraft found", "No aircraft found.\n Would you like to add a new aircraft to the database?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            NewAcft na(this);
            na.exec();
            ui->newAcft->setText("");
            ui->newAcft->setFocus();
        }
    }
}

/// Pilot(s)

/*!
 * \brief NewFlight::addNewPilotMessageBox If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlight::addNewPilotMessageBox()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "No Pilot found",
                                  "No pilot found.\n Would you like to add a new pilot to the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qDebug() << "Add new pilot selected";
        DEBUG("This feature is not yet available.");
        // create and open new pilot dialog
    }
}

void NewFlight::on_newPicNameLineEdit_inputRejected()
{
    onInputRejected(ui->newPicNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlight::on_newPicNameLineEdit_editingFinished()
{
    auto line_edit = ui->newPicNameLineEdit;
    pic = "-1"; // set invalid
    if(ui->newPicNameLineEdit->text() == "self") // Logbook owner is PIC
    {
        pic = "self";
        DEBUG("Pilot selected: " << pic);
    }else //check if entry is in pilotList
    {
        QStringList pilotList = dbPilots::retreivePilotList();
        QStringList match = pilotList.filter(line_edit->text(), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            pic = match[0];
            line_edit->setText(pic);
            DEBUG("Pilot selected: " << pic);
            onEditingFinished(line_edit);
        }else
        {
            DEBUG("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox();
        }
    }
}


/*!
 * ============================================================================
 * The above entris are mandatory for logging a flight,
 * the rest of the entries are either optional or can
 * be determined from the entries already made.
 * ============================================================================
 */

void NewFlight::on_secondPilotNameLineEdit_inputRejected()
{
    onInputRejected(ui->secondPilotNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlight::on_secondPilotNameLineEdit_editingFinished()
{
    auto line_edit = ui->secondPilotNameLineEdit;
    secondPilot = "-1"; // set invalid
    if(ui->newPicNameLineEdit->text() == "self") // Logbook owner is PIC
    {
        secondPilot = "self";
        DEBUG("Pilot selected: " << secondPilot);
    }else //check if entry is in pilotList
    {
        QStringList pilotList = dbPilots::retreivePilotList();
        QStringList match = pilotList.filter(line_edit->text(), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            secondPilot = match[0];
            line_edit->setText(secondPilot);
            DEBUG("Pilot selected: " << secondPilot);
            onEditingFinished(line_edit);
        }else
        {
            DEBUG("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox();
        }
    }
}

void NewFlight::on_thirdPilotNameLineEdit_inputRejected()
{
    onInputRejected(ui->thirdPilotNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlight::on_thirdPilotNameLineEdit_editingFinished()
{
    auto line_edit = ui->thirdPilotNameLineEdit;
    thirdPilot = "-1"; // set invalid
    if(ui->newPicNameLineEdit->text() == "self") // Logbook owner is PIC
    {
        thirdPilot = "self";
        DEBUG("Pilot selected: " << thirdPilot);
    }else //check if entry is in pilotList
    {
        QStringList pilotList = dbPilots::retreivePilotList();
        QStringList match = pilotList.filter(line_edit->text(), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            thirdPilot = match[0];
            line_edit->setText(thirdPilot);
            DEBUG("Pilot selected: " << thirdPilot);
            onEditingFinished(line_edit);
        }else
        {
            DEBUG("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox();
        }
    }
}

void NewFlight::on_FlightNumberLineEdit_editingFinished()
{
    qDebug() << "tbd: FlightNumber slot";
    // To Do: Store input in variable, perform some checks
    // Setting for optional Prefix (e.g. LH, LX etc.)
}

/*
 * ============================================================================
 * Extras Tab - These are for user convenience. From many of
 * these selections, determinations can be made on how to log
 * details, so that the user does not have to enter each item
 * manually. See also fillExtrasLineEdits()
 * ============================================================================
 */

void NewFlight::on_setAsDefaultButton_clicked()
{
    storeSettings();
}

void NewFlight::on_restoreDefaultButton_clicked()
{
    restoreSettings();
}

/*!
 * \brief On a given flight, time can either be logged as Pilot Flying (PF) or
 * Pilot Monitoring (PM). Cases where controls are changed during the flight
 * are rare and can be logged by manually editing the extras.
 */
void NewFlight::on_PilotFlyingCheckBox_stateChanged(int)
{
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

void NewFlight::on_ApproachComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ILS CAT III"){  //for a CAT III approach an Autoland is mandatory, so we can preselect it.
        ui->AutolandCheckBox->setCheckState(Qt::Checked);
        ui->AutolandSpinBox->setValue(1);
    }else{
        ui->AutolandCheckBox->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox->setValue(0);
    }
    approachType = arg1;
    qDebug() << "Approach Type: " << approachType;
}




/*
 * Extra Times - These line edits should be filled out automatically,
 * based on the ui selections and the user provided input. However,
 * manual adjustments are possible to cater for situations where for
 * example one portion of the flight is logged under different rules
 * than the rest of it.
 *
 * For example,
 * if we know the aircraft details we can determine how to log these times.
 * Some times are mutually exclusive, others can be combined.
 *
 * For example,
 * for a commercial Passenger flight, the commander can log all time as
 * Total Time and PIC time. If the aircraft is a multi-engine jet he can
 * also log Multi-Pilot time, and if he is an instructor, instructor time.
 *
 * It is not possible, however to log flight time as VFR or IFR time
 * simultaneously, as a flight at any given point in time can only follow
 * one set of rules. It is possible, to change flight rules and log the first
 * x minutes as VFR and the rest of it as IFR, for example. Hence the need
 * for the possibility to edit these times manually.
 *
 * The most complex time to determine is night time, see documentation of
 * the calc class for details.
 *
 * In General, the idea is to automatically fill as much as possible, but
 * if the user decides to change these times, accept the inputs, as long as
 * they are generally valid. We cannot cater for all possibilities, so as long
 * as the time the user has input is a valid time <= Total Time, it can be
 * accepted to the database.
 */

void NewFlight::on_spseTimeLineEdit_editingFinished()
{
    ui->spseTimeLineEdit->setText(calc::formatTimeInput(ui->spseTimeLineEdit->text()));
    if(ui->spseTimeLineEdit->text() == ""){
        ui->spseTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->spseTimeLineEdit->setFocus();
    }else{
        ui->spseTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_spmeTimeLineEdit_editingFinished()
{
    ui->spmeTimeLineEdit->setText(calc::formatTimeInput(ui->spmeTimeLineEdit->text()));
    if(ui->spmeTimeLineEdit->text() == ""){
        ui->spmeTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->spmeTimeLineEdit->setFocus();
    }else{
        ui->spmeTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_mpTimeLineEdit_editingFinished()
{
    ui->mpTimeLineEdit->setText(calc::formatTimeInput(ui->mpTimeLineEdit->text()));
    if(ui->mpTimeLineEdit->text() == ""){
        ui->mpTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->mpTimeLineEdit->setFocus();
    }else{
        ui->mpTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_totalTimeLineEdit_editingFinished()
{
    ui->totalTimeLineEdit->setText(calc::formatTimeInput(ui->totalTimeLineEdit->text()));
    if(ui->totalTimeLineEdit->text() == ""){
        ui->totalTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->totalTimeLineEdit->setFocus();
    }else{
        ui->totalTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_ifrTimeLineEdit_editingFinished()
{
    ui->ifrTimeLineEdit->setText(calc::formatTimeInput(ui->ifrTimeLineEdit->text()));
    if(ui->ifrTimeLineEdit->text() == ""){
        ui->ifrTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->ifrTimeLineEdit->setFocus();
    }else{
        ui->ifrTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_vfrTimeLineEdit_editingFinished()
{
    ui->vfrTimeLineEdit->setText(calc::formatTimeInput(ui->vfrTimeLineEdit->text()));
    if(ui->vfrTimeLineEdit->text() == ""){
        ui->vfrTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->vfrTimeLineEdit->setFocus();
    }else{
        ui->vfrTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_nightTimeLineEdit_editingFinished()
{
    ui->nightTimeLineEdit->setText(calc::formatTimeInput(ui->nightTimeLineEdit->text()));
    if(ui->nightTimeLineEdit->text() == ""){
        ui->nightTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->nightTimeLineEdit->setFocus();
    }else{
        ui->nightTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_xcTimeLineEdit_editingFinished()
{
    ui->xcTimeLineEdit->setText(calc::formatTimeInput(ui->xcTimeLineEdit->text()));
    if(ui->xcTimeLineEdit->text() == ""){
        ui->xcTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->xcTimeLineEdit->setFocus();
    }else{
        ui->xcTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_picTimeLineEdit_editingFinished()
{
    ui->picTimeLineEdit->setText(calc::formatTimeInput(ui->picTimeLineEdit->text()));
    if(ui->picTimeLineEdit->text() == ""){
        ui->picTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->picTimeLineEdit->setFocus();
    }else{
        ui->picTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_copTimeLineEdit_editingFinished()
{
    ui->copTimeLineEdit->setText(calc::formatTimeInput(ui->copTimeLineEdit->text()));
    if(ui->copTimeLineEdit->text() == ""){
        ui->copTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->copTimeLineEdit->setFocus();
    }else{
        ui->copTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_dualTimeLineEdit_editingFinished()
{
    ui->dualTimeLineEdit->setText(calc::formatTimeInput(ui->dualTimeLineEdit->text()));
    if(ui->dualTimeLineEdit->text() == ""){
        ui->dualTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->dualTimeLineEdit->setFocus();
    }else{
        ui->dualTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_fiTimeLineEdit_editingFinished()
{
    ui->fiTimeLineEdit->setText(calc::formatTimeInput(ui->fiTimeLineEdit->text()));
    if(ui->fiTimeLineEdit->text() == ""){
        ui->fiTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->fiTimeLineEdit->setFocus();
    }else{
        ui->fiTimeLineEdit->setStyleSheet("");
    }
}

void NewFlight::on_simTimeLineEdit_editingFinished()
{
    ui->simTimeLineEdit->setText(calc::formatTimeInput(ui->simTimeLineEdit->text()));
    if(ui->simTimeLineEdit->text() == ""){
        ui->simTimeLineEdit->setStyleSheet("border: 1px solid red");
        ui->simTimeLineEdit->setFocus();
    }else{
        ui->simTimeLineEdit->setStyleSheet("");
    }
}


/*
 * Control Buttons
 */

void NewFlight::on_buttonBox_accepted()
{
    on_newDoft_editingFinished();// - activate slots in case user has been active in last input before clicking submit
    //on_newTonb_editingFinished();
    //on_newTofb_editingFinished();
    //on_newDept_editingFinished();
    //on_newDest_editingFinished();
    on_newAcft_editingFinished();
    on_newPicNameLineEdit_editingFinished();

        QVector<QString> flight;
        flight = collectInput();
        if(verifyInput())
        {
            dbFlight::commitFlight(flight);
            qDebug() << flight << "Has been commited.";
            QMessageBox msgBox(this);
            msgBox.setText("Flight has been commited.");
            msgBox.exec();
        }else
        {
            qDebug() << "Invalid Input. No entry has been made in the database.";
            dbFlight::commitToScratchpad(flight);
            QMessageBox msgBox(this);
            msgBox.setText("Invalid entries detected. Please check your input.");
            msgBox.exec();
            //NewFlight nf(this);
            //nf.exec();
        }
}

void NewFlight::on_buttonBox_rejected()
{
    qDebug() << "NewFlight: Rejected\n";
}

/// Input Verification and Collection

QVector<QString> NewFlight::collectInput()
{
    // Collect input from the user fields and return a vector containing the flight information
    QString doft = date.toString(Qt::ISODate); // Format Date
    QTime tblk = calc::blocktime(tofb,tonb);   // Calculate Blocktime


    // Prepare Vector for commit to database
    flight = dbFlight::createFlightVectorFromInput(doft, dept, tofb, dest, tonb, tblk, pic, acft);
    qDebug() << "Created flight vector:" << flight;

    return flight;
}

/*!
 * \brief NewFlight::verifyInput checks if input exists in database.
 * \return
 */
bool NewFlight::verifyInput()
{

    bool deptValid = false;
    bool tofbValid = false;
    bool destValid = false;
    bool tonbValid = false;
    bool tblkValid = false;
    bool picValid = false;
    bool acftValid = false;

    QTime tblk = calc::blocktime(tofb,tonb);
    int checktblk = calc::time_to_minutes(tblk);

    bool doftValid = true; //doft assumed to be always valid due to QDateTimeEdit constraints
    qDebug() << "NewFlight::verifyInput() says: Date:" << doft << " - Valid?\t" << doftValid;

    deptValid = dbAirport::checkICAOValid(dept);
    qDebug() << "NewFlight::verifyInput() says: Departure is:\t" << dept << " - Valid?\t" << deptValid;

    destValid = dbAirport::checkICAOValid(dest);
    qDebug() << "NewFlight::verifyInput() says: Destination is:\t" << dest << " - Valid?\t" << destValid;

    tofbValid = (unsigned)(calc::time_to_minutes(tofb)-0) <= (1440-0) && tofb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes. 0 is allowed (midnight) & that it is not empty.
    qDebug() << "NewFlight::verifyInput() says: tofb is:\t\t" << tofb.toString("hh:mm") << " - Valid?\t" << tofbValid;

    tonbValid = (unsigned)(calc::time_to_minutes(tonb)-0) <= (1440-0) && tonb.toString("hh:mm") != ""; // Make sure time is within range, DB 1 day = 1440 minutes
    qDebug() << "NewFlight::verifyInput() says: tonb is:\t\t" << tonb.toString("hh:mm")<< " - Valid?\t" << tonbValid;

    picValid = (pic.toInt() > 0); // pic should be a pilot_id retreived from the database
    qDebug() << "NewFlight::verifyInput() says: pic is pilotd_id:\t" << pic << " - Valid?\t" << picValid;
    if(!picValid)
    {
        QMessageBox msgBox(this);
        msgBox.setText("You cannot log a flight without a valid pilot");
        msgBox.exec();
    }

    acftValid = (acft.toInt() > 0);
    qDebug() << "NewFlight::verifyInput() says: acft is tail_id:\t" << acft << " - Valid?\t" << acftValid;
    if(!acftValid)
    {
        QMessageBox msgBox(this);
        msgBox.setText("You cannot log a flight without a valid aircraft");
        msgBox.exec();
    }


    tblkValid = checktblk != 0;
    qDebug() << "NewFlight::verifyInput() says: tblk is:\t\t" << tblk.toString("hh:mm") << " - Valid?\t" << tblkValid;



    if(deptValid && tofbValid  && destValid && tonbValid
             && tblkValid && picValid && acftValid)
    {
        qDebug() << "====================================================";
        qDebug() << "NewFlight::verifyInput() says: All checks passed! Very impressive. ";
        return 1;
    }else
    {
        qDebug() << "====================================================";
        qDebug() << "NewFlight::verifyInput() says: Flight is invalid.";
        return 0;
    }
    return 0;

}

void NewFlight::returnInput(QVector<QString> flight)
{
    // Re-populates the input masks with the selected fields if input was erroneous to allow for corrections to be made
    qDebug() << "return Input: " << flight;
    ui->newDoft->setDate(QDate::fromString(flight[1],Qt::ISODate));
    ui->newDeptLocLineEdit->setText(flight[2]);
    ui->newDeptTimeLineEdit->setText(flight[3]);
    ui->newDestLocLineEdit->setText(flight[4]);
    ui->newDestTimeLineEdit->setText(flight[5]);
    // flight[6] is blocktime
    ui->newPicNameLineEdit->setText(dbPilots::retreivePilotNameFromID(flight[7]));
    ui->newAcft->setText(dbAircraft::retreiveRegistration(flight[8]));
}


/*!
 * \brief NewFlight::fillExtrasLineEdits Fills the flight time line edits according to ui selections.
 * Neccessary inputs are valid Date, Departure Time and Place, Destination Time and Place,
 * PIC name (pilot_id) and Aircraft (tail_id)
 */
void NewFlight::fillExtrasLineEdits()
{
    QString blockTime = calc::blocktime(tofb,tonb).toString("hh:mm");
    DEBUG(blockTime);

    QVector<QString> aircraftDetails = dbAircraft::retreiveAircraftDetails(dbAircraft::retreiveAircraftId(acft));
    DEBUG("aircraftDetails: " << aircraftDetails);
    if(!aircraftDetails.isEmpty()){// valid aircraft
        // SP SE
        if(aircraftDetails[0] == "1" && aircraftDetails[2] == "1"){
            DEBUG("SPSE yes");
            tSPSE = blockTime;
            ui->spseTimeLineEdit->setText(blockTime);
        }
        // SP ME
        if(aircraftDetails[0] == "1" && aircraftDetails[3] == "1"){
            DEBUG("SP ME yes");
            tSPME = blockTime;
            ui->spmeTimeLineEdit->setText(blockTime);
        }
        // MP
        if(aircraftDetails[1] == "1"){
            DEBUG("Multipilot yes");
            tMP = blockTime;
            ui->mpTimeLineEdit->setText(blockTime);
        }
    }else{DEBUG("Aircraft Details Empty");}//invalid aircraft

    // TOTAL
    ui->totalTimeLineEdit->setText(blockTime);
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->ifrTimeLineEdit->setText(blockTime);
        ui->vfrTimeLineEdit->setText("");
    }
    // VFR
    if(ui->VfrCheckBox->isChecked()){
        ui->vfrTimeLineEdit->setText(blockTime);
        ui->ifrTimeLineEdit->setText("");
    }

    // Night
    QString deptDate = date.toString(Qt::ISODate) + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int tblk = calc::time_to_minutes(calc::blocktime(tofb,tonb));

    QString nightTime = calc::minutes_to_string(
                        QString::number(
                        calc::calculateNightTime(
                        dept, dest, deptDateTime, tblk)));
    ui->nightTimeLineEdit->setText(nightTime);

    // XC - Cross-country flight, if more than 50nm long
    if(calc::greatCircleDistanceBetweenAirports(dept,dest) >= 50){
        qDebug() << "Cross-country Flight: nm = " << calc::greatCircleDistanceBetweenAirports(dept,dest);
        ui->xcTimeLineEdit->setText(blockTime);
    }else{ui->xcTimeLineEdit->setText("00:00");}
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        ui->picTimeLineEdit->setText(blockTime);
        ui->copTimeLineEdit->setText("");
        ui->dualTimeLineEdit->setText("");
        ui->fiTimeLineEdit->setText("");
        break;
    case 1://Co-Pilot
        ui->picTimeLineEdit->setText("");
        ui->copTimeLineEdit->setText(blockTime);
        ui->dualTimeLineEdit->setText("");
        ui->fiTimeLineEdit->setText("");
        break;
    case 2://Dual
        ui->picTimeLineEdit->setText("");
        ui->copTimeLineEdit->setText("");
        ui->dualTimeLineEdit->setText(blockTime);
        ui->fiTimeLineEdit->setText("");
        break;
    case 3://Instructor
        ui->picTimeLineEdit->setText("");
        ui->copTimeLineEdit->setText("");
        ui->dualTimeLineEdit->setText("");
        ui->fiTimeLineEdit->setText(blockTime);
    }
    // SIM
}

/*!
 * \brief In case the user wants to manually edit the extra times
 * he can do so in this tab.
 */
void NewFlight::on_tabWidget_currentChanged(int index)
{
    if(index == 1){// Edit Details tab
        if(verifyInput()){
            fillExtrasLineEdits();
        }else{
            auto errorbox = new QMessageBox;
            errorbox->setText("Invalid Inputs. Unable to determine times automatically.\nPlease verify your inputs.");
            errorbox->exec();
        }
    }
}




