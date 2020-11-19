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

#include <QSqlRelationalTableModel>

/// =======================================================
/// Debug / WIP section
/// =======================================================
#define DEBUG(expr) \
    qDebug() << "~DEBUG" << __func__ << expr

void NewFlight::on_verifyButton_clicked()//debug button
{
    //fillExtrasLineEdits();
    collectBasicData();
    collectAdditionalData();
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


static const auto IATA_RX = QLatin1String("[a-zA-Z0-9]{3}");
static const auto ICAO_RX = QLatin1String("[a-zA-Z0-9]{4}");
static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-)?)");//(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-)?))?");
static const auto SELF_RX = QLatin1String("(self|SELF)");

/// Raw Input validation
const auto TIME_VALID_RGX       = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
const auto LOC_VALID_RGX        = QRegularExpression(IATA_RX + "|" + ICAO_RX);
const auto AIRCRAFT_VALID_RGX   = QRegularExpression("[A-Z0-9]+\\-?[A-Z0-9]+");
const auto PILOT_NAME_VALID_RGX = QRegularExpression(SELF_RX + QLatin1Char('|')
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",\\s?" // up to 4 first names
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );// up to 4 last names

/// Invalid characters (validators keep text even if it returns Invalid, see `onInputRejected` below)
const auto TIME_INVALID_RGX       = QRegularExpression("[^0-9:]");
const auto LOC_INVALID_RGX        = QRegularExpression("[^A-Z0-9]");
const auto AIRCRAFT_INVALID_RGX   = QRegularExpression("[^a-zA-Z0-9\\-]");
const auto PILOT_NAME_INVALID_RGX = QRegularExpression("[^\\p{L}|\\s|,]");

/// Sql columns
const auto LOC_SQL_COL        = SqlColumnNum(1);  // TODO: locations are iata/icao so 1,2 merge columns in sql?
const auto AIRCRAFT_SQL_COL   = SqlColumnNum(4);
const auto PILOT_NAME_SQL_COL = SqlColumnNum(6);



/*
 * Window Construction
 */
//For adding a new Flight to the logbook
NewFlight::NewFlight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    setup();
}
//For editing an existing flight
NewFlight::NewFlight(QWidget *parent, Flight oldFlight) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    ui->setupUi(this);
    setup();
    formFiller(oldFlight);
}

NewFlight::~NewFlight()
{
    delete ui;
}

void NewFlight::setup(){
    auto db = Db::Database();

    const auto location_settings = \
         LineEditSettings(LOC_VALID_RGX, LOC_INVALID_RGX, LOC_SQL_COL);
    const auto aircraft_settings = \
         LineEditSettings(AIRCRAFT_VALID_RGX, AIRCRAFT_INVALID_RGX, AIRCRAFT_SQL_COL);
    const auto pilot_name_settings = \
         LineEditSettings(PILOT_NAME_VALID_RGX, PILOT_NAME_INVALID_RGX, PILOT_NAME_SQL_COL);
    const auto time_settings = \
         LineEditSettings(TIME_VALID_RGX, TIME_INVALID_RGX, SqlColumnNum());

//     Set up Line Edits with QValidators, QCompleters
    auto line_edits = ui->flightDataTab->findChildren<QLineEdit*>();
    auto le_size = line_edits.size();
    for(auto i = 0; i < le_size; ++i){
//        auto bit_array = new QBitArray(le_size);
//        bit_array->setBit(i, true);
        this->lineEditBitMap[line_edits[i]] = i;
    }
    this->allOkBits.resize(line_edits.size());
    this->mandatoryLineEdits = {
        ui->deptLocLineEdit,
        ui->destLocLineEdit,
        ui->tofbTimeLineEdit,
        ui->tonbTimeLineEdit,
        ui->picNameLineEdit,
        ui->acftLineEdit,
    };

    for(auto line_edit : line_edits)
    {
        auto le_name = line_edit->objectName();
        if(QRegularExpression("Loc").match(le_name).hasMatch()){
            setupLineEdit(line_edit, location_settings);
        }
        else if (QRegularExpression("Time").match(le_name).hasMatch())
        {
            setupLineEdit(line_edit, time_settings);
        }
        else if (QRegularExpression("acft").match(le_name).hasMatch())
        {
            setupLineEdit(line_edit, aircraft_settings);
        }
        else if (QRegularExpression("Name").match(le_name).hasMatch())
        {
            setupLineEdit(line_edit, pilot_name_settings);
        }
    }
    //fill Lists
    pilots = CompletionList(CompleterTarget::pilots).list;
    tails = CompletionList(CompleterTarget::registrations).list;
    airports = CompletionList(CompleterTarget::airports).list;
    QString statement = "SELECT iata, icao FROM airports";
    auto result = Db::customQuery(statement,2);
    for(int i=0; i<result.length()-2; i += 2){
        airportMap.insert(result[i],result[i+1]);
    }

    // Groups for CheckBoxes
    QButtonGroup *FlightRulesGroup = new QButtonGroup(this);
    FlightRulesGroup->addButton(ui->IfrCheckBox);
    FlightRulesGroup->addButton(ui->VfrCheckBox);

    QButtonGroup *PilotTaskGroup = new QButtonGroup(this);
    PilotTaskGroup->addButton(ui->PilotFlyingCheckBox);
    PilotTaskGroup->addButton(ui->PilotMonitoringCheckBox);

    //set date for new object
    auto date = QDate::currentDate();
    ui->newDoft->setDate(date);

    // Visually mark mandatory fields
    ui->deptLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->destLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->tofbTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->tonbTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->picNameLineEdit->setStyleSheet("border: 1px solid orange");
    ui->acftLineEdit->setStyleSheet("border: 1px solid orange");

    readSettings();
    ui->tabWidget->setCurrentIndex(0);
    ui->deptLocLineEdit->setFocus();
}

void NewFlight::formFiller(Flight oldFlight)
{
    DEBUG("Filling Line Edits...");
    auto line_edits = parent()->findChildren<QLineEdit *>();
    QStringList line_edits_names;
    for(const auto& le : line_edits){
        line_edits_names << le->objectName();
    }
    for(const auto& key : oldFlight.data.keys()){
        auto rx = QRegularExpression(key + "LineEdit");//acftLineEdit
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEBUG("Loc Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    const QString& acft = Db::singleSelect("registration", "tails", "tail_id",
                                                    oldFlight.data.value(key),
                                                    Db::exactMatch);
                    le->setText(acft);
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Loc\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEBUG("Loc Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(oldFlight.data.value(key));
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Time\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEBUG("Time Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(Calc::minutesToString(
                                oldFlight.data.value(key)));
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Name\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEBUG("Time Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    const QString& column = "piclastname||', '||picfirstname";
                    const QString& name = Db::singleSelect(column, "pilots", "pilot_id",
                                                    oldFlight.data.value(key),
                                                    Db::exactMatch);
                    le->setText(name);
                }
                break;
            }
        }
    }
}

/*
 * ============================================================================
 * Functions
 * ============================================================================
 */

/*!
 * \brief setLineEditValidator set Validators for QLineEdits that end with Time, Loc,
 * Aircraft or Name
 */
inline void NewFlight::setupLineEdit(QLineEdit* line_edit, LineEditSettings settings)
{
    auto db = QSqlDatabase::database("qt_sql_default_connection");
    auto line_edit_objectName = line_edit->objectName();
    DEBUG("Setting validators for " << line_edit_objectName);
    auto [valid_rgx, invalid_rgx, sql_col] = settings.getAll();

    auto validator = new StrictRxValidator(valid_rgx, line_edit);

    auto comp_model = new QSqlRelationalTableModel(line_edit, db);
    comp_model->database().open();
    comp_model->setTable("QCompleterView");
    comp_model->select();

    auto completer = new QCompleter(comp_model, line_edit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCompletionColumn(sql_col.column());
    completer->setFilterMode(Qt::MatchContains);
    /*if(QRegularExpression("\\w+Loc").match(line_edit_objectName).hasMatch()) { completer->setFilterMode(Qt::MatchContains); }
    if(QRegularExpression("\\w+Acft").match(line_edit_objectName).hasMatch()) { completer->setFilterMode(Qt::MatchContains); }
    if(QRegularExpression("\\w+Name").match(line_edit_objectName).hasMatch()) { completer->setFilterMode(Qt::MatchContains); }*/

    line_edit->setValidator(validator);
    line_edit->setCompleter(completer);
}

/*!
 * \brief NewFlight::writeSettings Writes current selection for auto-logging
 * to settings file.
 */
void NewFlight::writeSettings()
{
    DEBUG("Writing Settings...");

    Settings::write("NewFlight/FunctionComboBox",ui->FunctionComboBox->currentText());
    Settings::write("NewFlight/ApproachComboBox",ui->ApproachComboBox->currentText());
    Settings::write("NewFlight/PilotFlyingCheckBox",ui->PilotFlyingCheckBox->isChecked());
    Settings::write("NewFlight/PilotMonitoringCheckBox",ui->PilotMonitoringCheckBox->isChecked());
    Settings::write("NewFlight/TakeoffSpinBox",ui->TakeoffSpinBox->value());
    Settings::write("NewFlight/TakeoffCheckBox",ui->TakeoffCheckBox->isChecked());
    Settings::write("NewFlight/LandingSpinBox",ui->LandingSpinBox->value());
    Settings::write("NewFlight/LandingCheckBox",ui->LandingCheckBox->isChecked());
    Settings::write("NewFlight/AutolandSpinBox",ui->AutolandSpinBox->value());
    Settings::write("NewFlight/AutolandCheckBox",ui->AutolandCheckBox->isChecked());
    Settings::write("NewFlight/IfrCheckBox",ui->IfrCheckBox->isChecked());
    Settings::write("NewFlight/VfrCheckBox",ui->VfrCheckBox->isChecked());
}

/*!
 * \brief NewFlight::readSettings Retreives auto-logging settings
 * and sets up ui accordingly
 */
void NewFlight::readSettings()
{
    DEBUG("Reading Settings...");
    QSettings settings;

    ui->FunctionComboBox->setCurrentText(Settings::read("NewFlight/FunctionComboBox").toString());
    ui->ApproachComboBox->setCurrentText(Settings::read("NewFlight/ApproachComboBox").toString());
    ui->PilotFlyingCheckBox->setChecked(Settings::read("NewFlight/PilotFlyingCheckBox").toInt());
    ui->PilotMonitoringCheckBox->setChecked(Settings::read("NewFlight/PilotMonitoringCheckBox").toInt());
    ui->TakeoffSpinBox->setValue(Settings::read("NewFlight/TakeoffSpinBox").toInt());
    ui->TakeoffCheckBox->setChecked(Settings::read("NewFlight/TakeoffCheckBox").toInt());
    ui->LandingSpinBox->setValue(Settings::read("NewFlight/LandingSpinBox").toInt());
    ui->LandingCheckBox->setChecked(Settings::read("NewFlight/LandingCheckBox").toInt());
    ui->AutolandSpinBox->setValue(Settings::read("NewFlight/AutolandSpinBox").toInt());
    ui->AutolandCheckBox->setChecked(Settings::read("NewFlight/AutolandCheckBox").toInt());
    ui->IfrCheckBox->setChecked(Settings::read("NewFlight/IfrCheckBox").toInt());
    ui->VfrCheckBox->setChecked(Settings::read("NewFlight/VfrCheckBox").toInt());
    ui->flightNumberPrefixLabel->setText(Settings::read("userdata/flightnumberPrefix").toString() + QLatin1Char('-'));

    if(Settings::read("NewFlight/FunctionComboBox").toString() == "PIC"){
        ui->picNameLineEdit->setText("self");
        ui->secondPilotNameLineEdit->setText("");
    }else if (Settings::read("NewFlight/FunctionComboBox").toString() == "Co-Pilot") {
        ui->picNameLineEdit->setText("");
        ui->secondPilotNameLineEdit->setText("self");
    }
}
/*!
 * \brief NewFlight::addNewPilotMessageBox If the user input is not in the pilotNameList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlight::addNewPilotMessageBox()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "No Pilot found",
                                  "No pilot found.<br>Please enter the Name as"
                                  "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
                                  "If this is the first time you log a flight with this pilot, you have to "
                                  "add the name to the database first.<br><br>Would you like to add a new pilot to the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        qDebug() << "Add new pilot selected";
        // create and open new pilot dialog
        auto np = NewPilot(Db::createNew, this);
        np.exec();
    }
}

/// Input Verification and Collection

/*!
 * \brief NewFlight::update check if needed inputs are available and update form
 */
void NewFlight::update()
{
    QVector<QLineEdit*> notFilled;
    for(auto lineEdit : mandatoryLineEdits)
    {
        if(!allOkBits.testBit(lineEditBitMap[lineEdit]))
        {
            notFilled.push_back(lineEdit);
        }
    }
    if(notFilled.empty()){
        collectBasicData();
        fillExtras();
    } else {
        DEBUG("No update - not enough variables known.");
    }
}

/*!
 * \brief NewFlight::collectBasicInput After mandatory inputs have passed
 * validation, collect values from line edits and combo boxes to create new Data
 * for flight object.
 */
void NewFlight::collectBasicData()
{
    DEBUG("Collecting Basic Input...");
    // Date of Flight
    auto date = ui->newDoft->date();
    auto doft = date.toString(Qt::ISODate);
    newData.insert("doft",doft);
    // Departure Loc
    newData.insert("dept",ui->deptLocLineEdit->text());
    // Time Off Blocks
    auto timeOff = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");
    if(timeOff.isValid()){
        int tofb = timeOff.hour() * 60 + timeOff.minute();
        newData.insert("tofb",QString::number(tofb));
    }
    // Destination Loc
    newData.insert("dest",ui->destLocLineEdit->text());
    // Time On Blocks
    auto timeOn = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    if(timeOn.isValid()){
        int tonb = timeOn.hour() * 60 + timeOn.minute();
        newData.insert("tonb",QString::number(tonb));
    }

    // Aircraft
    QString reg = ui->acftLineEdit->text();
    QString acft = Db::singleSelect("tail_id","tails","registration",reg,Db::exactMatch);
    if(!acft.isEmpty()){
        newData.insert("acft",acft);
    } else {
        emit ui->acftLineEdit->inputRejected();
    }
    // Pilot
    if(ui->picNameLineEdit->text() == "self" || ui->picNameLineEdit->text() == "self"){
        newData.insert("pic","1");
    } else {
        QString name = ui->picNameLineEdit->text();
        QStringList names = name.split(',');
        if(names.length()==2){
            QString firstNames = names[1].simplified();
            QString lastNames = names[0].simplified();
            QString query = "SELECT pilot_id FROM pilots WHERE piclastname = \"" + lastNames
                    + "\" AND picfirstname = \"" + firstNames + "\"";
            QVector<QString> pic = Db::customQuery(query,1);
            if(!pic.isEmpty()){
                newData.insert("pic",pic.first());
            }else {
                emit ui->picNameLineEdit->inputRejected();
            }
        }
    }
    DEBUG(newData);
    DEBUG("Basic Input Collected. Should be 7 items. is:" << newData.count());
}

void NewFlight::collectAdditionalData()
{
    // Pilot 2
    if(!ui->secondPilotNameLineEdit->text().isEmpty()){
        if(ui->secondPilotNameLineEdit->text() == "self" || ui->secondPilotNameLineEdit->text() == "self"){
            newData.insert("secondPilot","1");
        } else {
            QString name = ui->secondPilotNameLineEdit->text();
            QStringList names = name.split(',');
            if(names.length()==2){
                QString firstNames = names[1].simplified();
                QString lastNames = names[0].simplified();
                QString query = "SELECT pilot_id FROM pilots WHERE piclastname = \"" + lastNames
                        + "\" AND picfirstname = \"" + firstNames + "\"";
                QVector<QString> pic = Db::customQuery(query,1);
                if(!pic.isEmpty()){
                    newData.insert("secondPilot",pic.first());
                }else {
                    emit ui->secondPilotNameLineEdit->inputRejected();
                }
            }
        }
    }

    // Pilot 3
    if(!ui->thirdPilotNameLineEdit->text().isEmpty()){
        if(ui->thirdPilotNameLineEdit->text() == "self" || ui->thirdPilotNameLineEdit->text() == "self"){
            newData.insert("thirdPilot","1");
        } else {
            QString name = ui->thirdPilotNameLineEdit->text();
            QStringList names = name.split(',');
            if(names.length()==2){
                QString firstNames = names[1].simplified();
                QString lastNames = names[0].simplified();
                QString query = "SELECT pilot_id FROM pilots WHERE piclastname = \"" + lastNames
                        + "\" AND picfirstname = \"" + firstNames + "\"";
                QVector<QString> pic = Db::customQuery(query,1);
                if(!pic.isEmpty()){
                    newData.insert("thirdPilot",pic.first());
                }else {
                    emit ui->thirdPilotNameLineEdit->inputRejected();
                }
            }
        }
    }

    // Extra Times
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    QString blockTime = Calc::blocktime(tofb, tonb).toString("hh:mm");
    QString blockMinutes = QString::number(Calc::stringToMinutes(blockTime));
    DEBUG("Blocktime: " << blockTime << " (" << blockMinutes << " minutes)");


    auto acft = Aircraft(newData.value("acft").toInt());

    DEBUG("aircraftDetails: " << acft);
    if(!acft.data.isEmpty()){// valid aircraft
        // SP SE
        if(acft.data.value("singlepilot") == "1" && acft.data.value("singleengine") == "1"){
            DEBUG("SPSE yes");
            newData.insert("tSPSE", blockMinutes);
        }
        // SP ME
        if(acft.data.value("singlepilot") == "1" && acft.data.value("multiengine") == "1"){
            DEBUG("SP ME yes");
            newData.insert("tSPME", blockMinutes);
        }
        // MP
        if(acft.data.value("multipilot") == "1"){
            DEBUG("Multipilot yes");
            newData.insert("tMP", blockMinutes);
        }
    }else{DEBUG("Aircraft Details Empty");}//invalid aircraft

    // TOTAL
    newData.insert("tblk",blockMinutes);
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        newData.insert("tIFR",blockMinutes);

    }
    // Night
    QString deptDate = ui->newDoft->date().toString(Qt::ISODate) + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int tblk = blockMinutes.toInt();
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();

    QString nightTime = QString::number(
                        Calc::calculateNightTime(
                        newData.value("dept"), newData.value("dest"),
                        deptDateTime, tblk, nightAngle));
    newData.insert("tNIGHT", nightTime);
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        newData.insert("tPIC",blockMinutes);
        break;
    case 1://Co-Pilot
        newData.insert("tSIC",blockMinutes);
        break;
    case 2://Dual
        newData.insert("tDual", blockMinutes);
        break;
    case 3://Instructor
        newData.insert("tFI", blockMinutes);
    }
    // Auto-Logging
    newData.insert("pilotFlying", QString::number(ui->PilotFlyingCheckBox->isChecked()));
    if(nightTime == "0"){ // all day
        DEBUG("All Day.");
        newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
        newData.insert("ldgDay", QString::number(ui->LandingSpinBox->value()));
        newData.insert("toNight", "0");
        newData.insert("ldgNight", "0");
    }else if (nightTime == blockTime) { // all night
        DEBUG("All Night.");
        newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
        newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
        newData.insert("toDay", "0");
        newData.insert("ldgDay", "0");
    } else {
        if(Calc::isNight(ui->deptLocLineEdit->text(), deptDateTime,  nightAngle)){
            newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
            newData.insert("toDay", "0");
        }else{
            newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
            newData.insert("toNight", "0");
        }
        QString destDate = ui->newDoft->date().toString(Qt::ISODate) + 'T' + tonb.toString("hh:mm");
        QDateTime destDateTime = QDateTime::fromString(destDate,"yyyy-MM-ddThh:mm");
        if(Calc::isNight(ui->destLocLineEdit->text(), destDateTime,  nightAngle)){
            newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
            newData.insert("ldgDay", "0");
        }else{
            newData.insert("ldgDay", QString::number(ui->LandingSpinBox->value()));
            newData.insert("ldgNight", "0");
        }
    }
    newData.insert("autoland", QString::number(ui->AutolandSpinBox->value()));
    newData.insert("ApproachType", ui->ApproachComboBox->currentText());
    newData.insert("FlightNumber", ui->FlightNumberLineEdit->text());
    newData.insert("Remarks", ui->RemarksLineEdit->text());
    DEBUG("Collected Data: " << newData);
}
/*!
 * \brief NewFlight::fillExtrasLineEdits Fills the deductable items in the newData map and
 * additional flight time line edits according to ui selections.
 * Neccessary prerequisites are valid Date, Departure Time and Place, Destination Time and Place,
 * PIC name (pilot_id) and Aircraft (tail_id)
 */
void NewFlight::fillExtras()
{
    //Times
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    QString blockTime = Calc::blocktime(tofb, tonb).toString("hh:mm");
    QString blockMinutes = QString::number(Calc::stringToMinutes(blockTime));
    DEBUG("Blocktime: " << blockTime << " (" << blockMinutes << " minutes)");
    auto acft = Aircraft(newData.value("acft").toInt());

    DEBUG("aircraftDetails: " << acft);
    if(!acft.data.isEmpty()){// valid aircraft
        // SP SE
        if(acft.data.value("singlepilot") == "1" && acft.data.value("singleengine") == "1"){
            ui->tSPSETimeLineEdit->setText(blockTime);
        }
        // SP ME
        if(acft.data.value("singlepilot") == "1" && acft.data.value("multiengine") == "1"){
            ui->tSPMETimeLineEdit->setText(blockTime);
        }
        // MP
        if(acft.data.value("multipilot") == "1"){
            ui->tMPTimeLineEdit->setText(blockTime);
        }
    }else{DEBUG("Aircraft Details Empty");}//invalid aircraft

    // TOTAL
    ui->tblkTimeLineEdit->setText(blockTime);
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->tIFRTimeLineEdit->setText(blockTime);
    }
    // Night
    QString deptDate = ui->newDoft->date().toString(Qt::ISODate) + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int tblk = blockMinutes.toInt();
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();

    QString nightTime = QString::number(
                        Calc::calculateNightTime(
                        newData.value("dept"), newData.value("dest"),
                        deptDateTime, tblk, nightAngle));
    ui->tNIGHTTimeLineEdit->setText(Calc::minutesToString(nightTime));
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        ui->tPICTimeLineEdit->setText(blockTime);
        ui->tSICTimeLineEdit->setText("");
        ui->tDualTimeLineEdit->setText("");
        ui->tFITimeLineEdit->setText("");
        break;
    case 1://Co-Pilot
        ui->tPICTimeLineEdit->setText("");
        ui->tSICTimeLineEdit->setText(blockTime);
        ui->tDualTimeLineEdit->setText("");
        ui->tFITimeLineEdit->setText("");
        break;
    case 2://Dual
        ui->tPICTimeLineEdit->setText("");
        ui->tSICTimeLineEdit->setText("");
        ui->tDualTimeLineEdit->setText(blockTime);
        ui->tFITimeLineEdit->setText("");
        break;
    case 3://Instructor
        ui->tPICTimeLineEdit->setText("");
        ui->tSICTimeLineEdit->setText("");
        ui->tDualTimeLineEdit->setText("");
        ui->tFITimeLineEdit->setText(blockTime);
    }
}

bool NewFlight::verifyInput()
{
    QVector<QLineEdit*> notFilled;
    for(auto lineEdit : mandatoryLineEdits)
    {
        if(!allOkBits.testBit(lineEditBitMap[lineEdit]))
        {
            notFilled.push_back(lineEdit);
        }
    }
    if(!notFilled.empty()){
        QMessageBox notFilledMessageBox(this);
        auto errorMsg = QString("Not all required fields are filled out.\n\n"
                                "Plase make sure the following are not empty:\n\n"
                                "Departure\nDestination\nTime Off Blocks\nTime On Blocks\n"
                                "Pilot in Command\nAircraft Registration\n\n");
        notFilledMessageBox.setText(errorMsg);
        notFilledMessageBox.exec();
        return false;
    } else {
        QStringList lineEditText;
        for(auto line_edit : mandatoryLineEdits)
        {
            lineEditText.push_back(line_edit->text());
        }
        this->result = lineEditText;
        emit mandatoryFieldsValid(this);
        return true;
    }
}
/*!
 * ============================================================================
 * Slots
 * ============================================================================
 */
void NewFlight::on_buttonBox_accepted()
{
    DEBUG("OK pressed");
    if(verifyInput()){
        DEBUG("Input verified");
        collectBasicData();
        collectAdditionalData();
        auto newFlight = Flight(newData);
        if(newFlight.commit()){
            accept();
        } else {
            auto mb = new QMessageBox(this);
            auto errorMsg = QString("Unable to commit Flight to Logbook."
                                    "The following error has ocurred:\n\n");
            errorMsg.append(newFlight.error);
            mb->setText(errorMsg);
            mb->show();
        }
    }
}

void NewFlight::on_buttonBox_rejected()
{
    DEBUG("CANCEL pressed." << newData);
    reject();
}
/*!
 * \brief onInputRejected Set `line_edit`'s border to red and check if `rgx` matches
 * in order to keep text on line. Ensures corresponding LineEdit bit is 0.
 */
void NewFlight::onInputRejected(QLineEdit* line_edit, QRegularExpression rgx){
    DEBUG("Input rejected" << line_edit->text());
    line_edit->setStyleSheet("border: 1px solid red");
    this->allOkBits.setBit(this->lineEditBitMap[line_edit], false);
    auto text = line_edit->text();
    if(rgx.match(text).hasMatch())
    {
        text.chop(1);
        //line_edit->setText(line_edit->text());
        line_edit->setText(text);
    }
}

/*!
 * \brief onEditingFinishedCleanup resets styles and sets the corresponding bit to 1
 */
void NewFlight::onEditingFinishedCleanup(QLineEdit* line_edit)
{
    DEBUG("Input accepted" << line_edit << line_edit->text());
    line_edit->setStyleSheet("");
    this->allOkBits.setBit(this->lineEditBitMap[line_edit], true);
}

QStringList* NewFlight::getResult() { return &this->result; }

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

void NewFlight::on_deptLocLineEdit_inputRejected()
{
    DEBUG("SENDER --->" << sender());
    ui->deptLocLineEdit->setText(ui->deptLocLineEdit->text().toUpper());
    onInputRejected(ui->deptLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlight::on_deptLocLineEdit_textEdited(const QString &arg1)
{
    ui->deptLocLineEdit->setText(arg1.toUpper());
}

void NewFlight::on_deptLocLineEdit_editingFinished()
{
    DEBUG(sender()->objectName() << "EDITING FINISHED");
    auto line_edit = ui->deptLocLineEdit;
    auto text = ui->deptLocLineEdit->text();

    // check if iata exists, replace with icao code if it does.
    if(text.length() == 3){
        text = airportMap.value(text);
    }
    // Check if 4-letter code is in locationList
    if(text.length() == 4 && airports.indexOf(text) == -1){
        DEBUG("Airport not found.");
        emit line_edit->inputRejected();
        return;
    }else{
        DEBUG("Departure accepted: " << text);
        line_edit->setText(text);
        onEditingFinishedCleanup(line_edit); //reset style sheet
        ui->deptNameLabel->setText(Db::singleSelect("name","airports","icao",text,Db::exactMatch));
        update();
    }
}

void NewFlight::on_tofbTimeLineEdit_inputRejected()
{
    onInputRejected(ui->tofbTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlight::on_tofbTimeLineEdit_editingFinished()
{
    ui->tofbTimeLineEdit->setText(Calc::formatTimeInput(ui->tofbTimeLineEdit->text()));
    const auto time = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");

    auto line_edit = ui->tofbTimeLineEdit;
    onEditingFinishedCleanup(line_edit);

    if(time.isValid()){
        int minutes = time.hour() * 60 + time.minute();
        QString tofb = QString::number(minutes);
        DEBUG("Time Off Blocks accepted: " << tofb << " minutes - " << Calc::minutesToString(tofb));
    }else{
        emit line_edit->inputRejected();
    }
    onEditingFinishedCleanup(line_edit);
    update();
}

/// Destination

void NewFlight::on_destLocLineEdit_inputRejected()
{
    ui->destLocLineEdit->setText(ui->destLocLineEdit->text().toUpper());
    onInputRejected(ui->destLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlight::on_destLocLineEdit_textEdited(const QString &arg1)
{
    ui->destLocLineEdit->setText(arg1.toUpper());
}

void NewFlight::on_destLocLineEdit_editingFinished()
{
    DEBUG(sender()->objectName() << "EDITING FINISHED");
    auto line_edit = ui->destLocLineEdit;
    auto text = ui->destLocLineEdit->text();

    // check if iata exists, replace with icao code if it does.
    if(text.length() == 3){
        text = airportMap.value(text);
    }
    // Check if 4-letter code is in locationList
    if(text.length() == 4 && airports.indexOf(text) == -1){
        DEBUG("Airport not found.");
        emit line_edit->inputRejected();
        return;
    }else{
        DEBUG("Destination accepted: " << text);
        line_edit->setText(text);
        onEditingFinishedCleanup(line_edit); //reset style sheet
        ui->destNameLabel->setText(Db::singleSelect("name","airports","icao",text,Db::exactMatch));
        update();
    }
}

void NewFlight::on_tonbTimeLineEdit_inputRejected()
{
    onInputRejected(ui->tonbTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlight::on_tonbTimeLineEdit_editingFinished()
{
    ui->tonbTimeLineEdit->setText(Calc::formatTimeInput(ui->tonbTimeLineEdit->text()));
    auto line_edit = ui->tonbTimeLineEdit;
    const auto time = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    if(time.isValid()){
        int minutes = time.hour() * 60 + time.minute();
        QString tonb = QString::number(minutes);
        newData.insert("tonb",tonb);
        DEBUG("Time On Blocks accepted: " << tonb << " minutes - " << Calc::minutesToString(tonb));
    }else{
        emit line_edit->inputRejected();
    }
    onEditingFinishedCleanup(line_edit);
    update();
}

/// Date

void NewFlight::on_newDoft_editingFinished()
{
    update();
}

/// Aircraft

void NewFlight::on_acftLineEdit_inputRejected()
{
    ui->acftLineEdit->setText(ui->acftLineEdit->text().toUpper());
    onInputRejected(ui->acftLineEdit, QRegularExpression(AIRCRAFT_INVALID_RGX));
}

void NewFlight::on_acftLineEdit_editingFinished()
{
    auto registrationList = CompletionList(CompleterTarget::registrations).list;
    auto line_edit = ui->acftLineEdit;
    auto text = ui->acftLineEdit->text();

    QStringList match = registrationList.filter(line_edit->text(), Qt::CaseInsensitive);
    DEBUG("aircraft accepted: " << match);
    if(match.length() != 0) {
        text = match[0];
        line_edit->setText(text.toUpper());
        onEditingFinishedCleanup(line_edit);
        QString column = "make||' '||model||'-'||variant";
        ui->acftTypeLabel->setText(
                    Db::singleSelect(column,"tails","registration",text,Db::exactMatch));
        update();
    }else{
        DEBUG("Registration not in List!");
        emit line_edit->inputRejected();
    }
}

/// Pilot(s)

void NewFlight::on_picNameLineEdit_inputRejected()
{
    onInputRejected(ui->picNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlight::on_picNameLineEdit_editingFinished()
{
    auto line_edit = ui->picNameLineEdit;
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is PIC
    {
        DEBUG("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        ui->FunctionComboBox->setCurrentIndex(0);
        update();
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = CompletionList(CompleterTarget::pilots).list;
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            DEBUG("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
            update();
        }else
        {
            DEBUG("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox();
        }
    }
}


/*
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
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is Pilot
    {
        DEBUG("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        ui->FunctionComboBox->setCurrentIndex(1);
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = CompletionList(CompleterTarget::pilots).list;
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            DEBUG("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
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
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is Pilot
    {
        DEBUG("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = CompletionList(CompleterTarget::pilots).list;
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            DEBUG("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
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
    writeSettings();
}

void NewFlight::on_restoreDefaultButton_clicked()
{
    readSettings();
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
 * as the time the user has put in is a valid time <= Total Time, it can be
 * accepted to the database.
 */

inline bool NewFlight::isLessOrEqualToTotalTime(QString timeString)
{
    if(newData.value("tblk").isEmpty()){
        DEBUG("Total Time not set.");
        return false;
    } else {
        int minutes = Calc::stringToMinutes(timeString);
        if (minutes <= newData.value("tblk").toInt()) {
            return true;
        } else {
            auto mb = new QMessageBox(this);
            mb->setText("Cannot be more than Total Time of Flight (" + Calc::minutesToString(newData.value("tblk")) + ')');
            mb->show();
            return false;
        }
    }

}

void NewFlight::on_tSPSETimeLineEdit_editingFinished()
{
    const auto &le = ui->tSPSETimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSPSE",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tSPMETimeLineEdit_editingFinished()
{
    const auto &le = ui->tSPMETimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSPME",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tMPTimeLineEdit_editingFinished()
{
    const auto &le = ui->tMPTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tMP",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}
void NewFlight::on_tIFRTimeLineEdit_editingFinished()
{
    const auto &le = ui->tIFRTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tIFR",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tNIGHTTimeLineEdit_editingFinished()
{
    const auto &le = ui->tNIGHTTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tNIGHT",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tPICTimeLineEdit_editingFinished()
{
    const auto &le = ui->tPICTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tPIC",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tSICTimeLineEdit_editingFinished()
{
    const auto &le = ui->tSICTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSIC",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tDualTimeLineEdit_editingFinished()
{
    const auto &le = ui->tDualTimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tDual",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlight::on_tFITimeLineEdit_editingFinished()
{
    const auto &le = ui->tFITimeLineEdit;
    le->setText(Calc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tFI",QString::number(Calc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}
