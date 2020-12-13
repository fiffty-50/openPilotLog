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
#include "debug.h"



/// =======================================================
/// Debug / WIP section
/// =======================================================


void NewFlightDialog::on_verifyButton_clicked()//debug button
{
    //fillExtrasLineEdits();
    collectBasicData();
    collectAdditionalData();
}

static const auto IATA_RX = QLatin1String("[a-zA-Z0-9]{3}");
static const auto ICAO_RX = QLatin1String("[a-zA-Z0-9]{4}");
static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-)?)");//(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-)?))?");
static const auto SELF_RX = QLatin1String("(self|SELF)");

/// Raw Input validation
static const auto TIME_VALID_RGX       = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
static const auto LOC_VALID_RGX        = QRegularExpression(IATA_RX + "|" + ICAO_RX);
static const auto AIRCRAFT_VALID_RGX   = QRegularExpression("[A-Z0-9]+\\-?[A-Z0-9]+");
static const auto PILOT_NAME_VALID_RGX = QRegularExpression(SELF_RX + QLatin1Char('|')
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",?\\s?" // up to 4 first names
                                                     + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );// up to 4 last names
static const auto DATE_VALID_RGX       = QRegularExpression("^([1-9][0-9]{3}).?(1[0-2]|0[1-9]).?(3[01]|0[1-9]|[12][0-9])?$");// . allows all seperators, still allows for 2020-02-31, additional verification via QDate::isValid()

/// Invalid characters (validators keep text even if it returns Invalid, see `onInputRejected` below)
static const auto TIME_INVALID_RGX       = QRegularExpression("[^0-9:]");
static const auto LOC_INVALID_RGX        = QRegularExpression("[^A-Z0-9]");
static const auto AIRCRAFT_INVALID_RGX   = QRegularExpression("[^a-zA-Z0-9\\-]");
static const auto PILOT_NAME_INVALID_RGX = QRegularExpression("[^\\p{L}|\\s|,]");
static const auto DATE_INVALID_RGX       = QRegularExpression("[^0-9|:|\\.|/|-]");
static const auto INVALID_CHARS_RGX      = QRegularExpression("[^\\p{L}|\\s|,|\\-|'|0-9|:|\\.]");

/// Sql columns
static const auto LOC_SQL_COL        = SqlColumnNum(1);  // TODO: locations are iata/icao so 1,2 merge columns in sql?
static const auto AIRCRAFT_SQL_COL   = SqlColumnNum(4);
static const auto PILOT_NAME_SQL_COL = SqlColumnNum(6);



/*
 * Window Construction
 */
//For adding a new Flight to the logbook
NewFlightDialog::NewFlightDialog(QWidget *parent, Db::editRole edRole) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    DEB("new NewFlight\n");
    ui->setupUi(this);

    role = edRole;
    doUpdate = true;
    setup();
    //set date
    auto date = QDate::currentDate();
    ui->doftLineEdit->setText(date.toString(Qt::ISODate));
    emit ui->doftLineEdit->editingFinished();


    // Visually mark mandatory fields
    ui->deptLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->destLocLineEdit->setStyleSheet("border: 1px solid orange");
    ui->tofbTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->tonbTimeLineEdit->setStyleSheet("border: 1px solid orange");
    ui->picNameLineEdit->setStyleSheet("border: 1px solid orange");
    ui->acftLineEdit->setStyleSheet("border: 1px solid orange");

    readSettings();
}
//For editing an existing flight
NewFlightDialog::NewFlightDialog(QWidget *parent, Flight oldFlight, Db::editRole edRole) :
    QDialog(parent),
    ui(new Ui::NewFlight)
{
    DEB("new NewFlight\n");
    ui->setupUi(this);

    role=edRole;
    entry = oldFlight;
    doUpdate = true;
    setup();
    formFiller(oldFlight);
}

NewFlightDialog::~NewFlightDialog()
{
    DEB("Deleting NewFlight\n");
    delete ui;
}

bool NewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    if(object == ui->doftLineEdit && event->type() == QEvent::MouseButtonPress) {
        on_doftLineEditEntered();
        return false; // let the event continue to the edit
    } /*else if (object == this && event->type() == QEvent::Leave) {
        DEB("calendarWidget left.");
        //ui->doftLineEdit->blockSignals(false);
        //ui->calendarWidget->hide();
        //ui->placeLabel1->resize(ui->placeLabel2->size());
        //ui->doftLineEdit->setFocus();
        return false;
    }*/
    return false;
}

void NewFlightDialog::setup(){
    auto db = Db::Database();

    const auto location_settings = \
         LineEditSettings(LOC_VALID_RGX, LOC_INVALID_RGX, LOC_SQL_COL);
    const auto aircraft_settings = \
         LineEditSettings(AIRCRAFT_VALID_RGX, AIRCRAFT_INVALID_RGX, AIRCRAFT_SQL_COL);
    const auto pilot_name_settings = \
         LineEditSettings(PILOT_NAME_VALID_RGX, PILOT_NAME_INVALID_RGX, PILOT_NAME_SQL_COL);
    const auto time_settings = \
         LineEditSettings(TIME_VALID_RGX, TIME_INVALID_RGX, SqlColumnNum());
    const auto doft_settings = \
         LineEditSettings(DATE_VALID_RGX, DATE_INVALID_RGX, SqlColumnNum());

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
        ui->doftLineEdit,
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
        else if (QRegularExpression("Time").match(le_name).hasMatch()) {
            setupLineEdit(line_edit, time_settings);
        }
        else if (QRegularExpression("acft").match(le_name).hasMatch()) {
            setupLineEdit(line_edit, aircraft_settings);
        }
        else if (QRegularExpression("Name").match(le_name).hasMatch()) {
            setupLineEdit(line_edit, pilot_name_settings);
        }
        else if(QRegularExpression("doft").match(le_name).hasMatch()) {
            setupLineEdit(line_edit, doft_settings);
        }
    }
    //fill Lists
    pilots   = experimental::aDB()->getCompletionList(experimental::ADataBase::pilots);
    tails    = experimental::aDB()->getCompletionList(experimental::ADataBase::registrations);
    airports = experimental::aDB()->getCompletionList(experimental::ADataBase::airports);

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

    ui->flightDataTabWidget->setCurrentIndex(0);
    ui->flightDataTabWidget->removeTab(2); // hide calendar widget
    ui->calendarWidget->installEventFilter(this);
    ui->placeLabel1->installEventFilter(this);
    ui->doftLineEdit->installEventFilter(this);
    ui->deptLocLineEdit->setFocus();

    connect(ui->calendarWidget, SIGNAL(clicked(const QDate &)), this, SLOT(date_clicked(const QDate &)));
    connect(ui->calendarWidget, SIGNAL(activated(const QDate &)), this, SLOT(date_selected(const QDate &)));
}

void NewFlightDialog::formFiller(Flight oldFlight)
{
    DEB("Filling Line Edits...");
    DEB("With Data: " << oldFlight.data);
    // Date
    //ui->doftLineEdit->setDate(QDate::fromString(oldFlight.data.value("doft"), Qt::ISODate));
    QStringList filled;

    // Line Edits
    auto line_edits = parent()->findChildren<QLineEdit *>();
    QStringList line_edits_names;
    for(const auto& le : line_edits){
        line_edits_names << le->objectName();
    }
    const QString& acft = Db::singleSelect("registration", "tails", "tail_id",
                                    oldFlight.data.value("acft"),
                                    Db::exactMatch);
    ui->acftLineEdit->setText(acft);
    line_edits_names.removeOne("acftLineEdit");

    for(const auto& key : oldFlight.data.keys()){
        auto rx = QRegularExpression(key + "LineEdit");//acftLineEdit
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Loc Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(oldFlight.data.value(key));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Loc\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Loc Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    le->setText(oldFlight.data.value(key));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Time\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Time Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    DEB("Setting " << le->objectName() << " to " << ACalc::minutesToString(oldFlight.data.value(key)));
                    le->setText(ACalc::minutesToString(
                                oldFlight.data.value(key)));
                    filled << leName;
                    line_edits_names.removeOne(leName);
                }
                break;
            }
        }
        rx = QRegularExpression(key + "Name\\w+?");
        for(const auto& leName : line_edits_names){
            if(rx.match(leName).hasMatch())  {
                //DEB("Time Match found: " << key << " - " << leName);
                auto le = parent()->findChild<QLineEdit *>(leName);
                if(le != nullptr){
                    const QString& column = "piclastname||', '||picfirstname";
                    const QString& name = Db::singleSelect(column, "pilots", "pilot_id",
                                                    oldFlight.data.value(key),
                                                    Db::exactMatch);
                    le->setText(name);
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
    const QString& app = oldFlight.data.value("ApproachType");
    if(app != " "){
        ui->ApproachComboBox->setCurrentText(app);
    }
    // Task and Rules
    qint8 PF = oldFlight.data.value("pilotFlying").toInt();
    if (PF > 0) {
        ui->PilotFlyingCheckBox->setChecked(true);
    } else {
        ui->PilotMonitoringCheckBox->setChecked(true);
    }
    qint8 FR = oldFlight.data.value("tIFR").toInt();
    if (FR > 0) {
        ui->IfrCheckBox->setChecked(true);
    } else {
        ui->tIFRTimeLineEdit->setText("00:00");
        ui->VfrCheckBox->setChecked(true);
    }
    // Take Off and Landing
    qint8 TO = oldFlight.data.value("toDay").toInt() + oldFlight.data.value("toNight").toInt();
    qint8 LDG = oldFlight.data.value("ldgDay").toInt() + oldFlight.data.value("ldgNight").toInt();
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
    qint8 AL = oldFlight.data.value("autoland").toInt();
    if(AL > 0) {
        ui->AutolandCheckBox->setChecked(true);
        ui->AutolandSpinBox->setValue(AL);
    }

    for(const auto& le : mandatoryLineEdits){
        emit le->editingFinished();
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
inline void NewFlightDialog::setupLineEdit(QLineEdit* line_edit, LineEditSettings settings)
{
    auto db = QSqlDatabase::database("qt_sql_default_connection");
    auto line_edit_objectName = line_edit->objectName();
    DEB("Setting validators for " << line_edit_objectName);
    auto [valid_rgx, invalid_rgx, sql_col] = settings.getAll();

    auto validator = new StrictRxValidator(valid_rgx, line_edit);

    auto comp_model = new QSqlRelationalTableModel(line_edit, db);
    comp_model->database().open();
    comp_model->setTable("ViewQCompleter");
    comp_model->select();

    auto completer = new QCompleter(comp_model, line_edit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCompletionColumn(sql_col.column());
    completer->setFilterMode(Qt::MatchContains);
    //if(QRegularExpression("\\w+Loc").match(line_edit_objectName).hasMatch()) { completer->setFilterMode(Qt::MatchContains); }


    line_edit->setValidator(validator);
    line_edit->setCompleter(completer);
}

/*!
 * \brief NewFlight::writeSettings Writes current selection for auto-logging
 * to settings file.
 */
void NewFlightDialog::writeSettings()
{
    DEB("Writing Settings...");

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
void NewFlightDialog::readSettings()
{
    DEB("Reading Settings...");
    QSettings settings;

    ui->FunctionComboBox->setCurrentText(Settings::read("NewFlight/FunctionComboBox").toString());
    ui->ApproachComboBox->setCurrentText(Settings::read("NewFlight/ApproachComboBox").toString());
    ui->PilotFlyingCheckBox->setChecked(Settings::read("NewFlight/PilotFlyingCheckBox").toBool());
    ui->PilotMonitoringCheckBox->setChecked(Settings::read("NewFlight/PilotMonitoringCheckBox").toBool());
    ui->TakeoffSpinBox->setValue(Settings::read("NewFlight/TakeoffSpinBox").toInt());
    ui->TakeoffCheckBox->setChecked(Settings::read("NewFlight/TakeoffCheckBox").toBool());
    ui->LandingSpinBox->setValue(Settings::read("NewFlight/LandingSpinBox").toInt());
    ui->LandingCheckBox->setChecked(Settings::read("NewFlight/LandingCheckBox").toBool());
    ui->AutolandSpinBox->setValue(Settings::read("NewFlight/AutolandSpinBox").toInt());
    ui->AutolandCheckBox->setChecked(Settings::read("NewFlight/AutolandCheckBox").toBool());
    ui->IfrCheckBox->setChecked(Settings::read("NewFlight/IfrCheckBox").toBool());
    ui->VfrCheckBox->setChecked(Settings::read("NewFlight/VfrCheckBox").toBool());
    ui->FlightNumberLineEdit->setText(Settings::read("flightlogging/flightnumberPrefix").toString());

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
void NewFlightDialog::addNewPilotMessageBox(QLineEdit *parent)
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
        DEB("Add new pilot selected");
        // create and open new pilot dialog
        auto np = NewPilotDialog(Db::createNew, this);
        np.exec();
        QString statement = "SELECT MAX(pilot_id)  FROM pilots";
        QString id = Db::customQuery(statement,1).first();
        Pilot newPilot = Pilot(id.toInt());
        parent->setText(newPilot.data.value("displayname"));
        emit parent->editingFinished();
    }
}
/*!
 * \brief NewFlight::addNewAircraftMessageBox If the user input is not in the aircraftList, the user
 * is prompted if he wants to add a new entry to the database
 */
void NewFlightDialog::addNewAircraftMessageBox(QLineEdit *parent)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "No Aircraft found",
                                  "No aircraft with this registration found.<br>"
                                  "If this is the first time you log a flight with this aircraft, you have to "
                                  "add the registration to the database first.<br><br>Would you like to add a new aircraft to the database?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        DEB("Add new aircraft selected");
        // create and open new aircraft dialog
        auto na = NewTailDialog(ui->acftLineEdit->text(), Db::createNew, this);
        na.exec();
        QString statement = "SELECT MAX(tail_id)  FROM tails";
        QString id = Db::customQuery(statement,1).first();
        auto newAcft = Aircraft(id.toInt());
        parent->setText(newAcft.data.value("registration"));
        emit parent->editingFinished();
    }
}
/// Input Verification and Collection

/*!
 * \brief NewFlight::update check if needed inputs are available and update form
 */
void NewFlightDialog::update()
{
    if(doUpdate){
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
            DEB("No update - not enough variables known.");
        }
    }
}

/*!
 * \brief NewFlight::collectBasicInput After mandatory inputs have passed
 * validation, collect values from line edits and combo boxes to create new Data
 * for flight object.
 */
void NewFlightDialog::collectBasicData()
{
    // purge old data to ensure database integrity
    newData.clear();
    DEB("Collecting Basic Input...");
    // Date of Flight
    if(QDate::fromString(ui->doftLineEdit->text(),Qt::ISODate).isValid()) {
        auto doft = ui->doftLineEdit->text();
        newData.insert("doft",doft);
    }

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
    //Block Time
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    QString blockTime = ACalc::blocktime(tofb, tonb).toString("hh:mm");
    QString blockMinutes = QString::number(ACalc::stringToMinutes(blockTime));
    newData.insert("tblk",blockMinutes);

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
}

void NewFlightDialog::collectAdditionalData()
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
    QString blockTime = ACalc::blocktime(tofb, tonb).toString("hh:mm");
    QString blockMinutes = QString::number(ACalc::stringToMinutes(blockTime));

    auto acft = Aircraft(newData.value("acft").toInt());

    if(!acft.data.isEmpty()){// valid aircraft
        // SP SE
        if(acft.data.value("singlepilot") == "1" && acft.data.value("singleengine") == "1"){
            newData.insert("tSPSE", blockMinutes);
            newData.insert("tSPME", "");
            newData.insert("tMP", "");
        }
        // SP ME
        if(acft.data.value("singlepilot") == "1" && acft.data.value("multiengine") == "1"){
            newData.insert("tSPSE", "");
            newData.insert("tSPME", blockMinutes);
            newData.insert("tMP", "");
        }
        // MP
        if(acft.data.value("multipilot") == "1"){
            newData.insert("tSPSE", "");
            newData.insert("tSPME", "");
            newData.insert("tMP", blockMinutes);
        }
    }else{DEB("Aircraft Details Empty");}//invalid aircraft

    // IFR
    if(ui->IfrCheckBox->isChecked()){
        newData.insert("tIFR",blockMinutes);
    } else {
        newData.insert("tIFR","");
    }
    // Night
    QString deptDate = ui->doftLineEdit->text() + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int tblk = blockMinutes.toInt();
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();

    QString nightTime = QString::number(
                        ACalc::calculateNightTime(
                        newData.value("dept"), newData.value("dest"),
                        deptDateTime, tblk, nightAngle));
    newData.insert("tNIGHT", nightTime);
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        newData.insert("tPIC", blockMinutes);
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 1://PICUS
        newData.insert("tPIC", "");
        newData.insert("tPICUS", blockMinutes);
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 2://Co-Pilot
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", blockMinutes);
        newData.insert("tDUAL", "");
        newData.insert("tFI", "");
        break;
    case 3://Dual
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", blockMinutes);
        newData.insert("tFI", "");
        break;
    case 4://Instructor
        newData.insert("tPIC", "");
        newData.insert("tPICUS", "");
        newData.insert("tSIC", "");
        newData.insert("tDUAL", "");
        newData.insert("tFI", blockMinutes);
    }
    // Pilot Flying
    newData.insert("pilotFlying", QString::number(ui->PilotFlyingCheckBox->isChecked()));
    // TO and LDG
    if(ui->TakeoffCheckBox->isChecked()) {
        if(nightTime == "0"){ // all day
            newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
            newData.insert("toNight", "0");
        }else if (nightTime == blockTime) { // all night
            newData.insert("toDay", "0");
            newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
        } else { //check
            if(ACalc::isNight(ui->deptLocLineEdit->text(), deptDateTime,  nightAngle)){
                newData.insert("toDay", "0");
                newData.insert("toNight", QString::number(ui->TakeoffSpinBox->value()));
            }else{
                newData.insert("toDay", QString::number(ui->TakeoffSpinBox->value()));
                newData.insert("toNight", "0");
            }
        }
    } else {
        newData.insert("toDay", "0");
        newData.insert("toNight", "0");
    }

    if(ui->LandingCheckBox->isChecked()) {
        if(nightTime == "0"){ // all day
            newData.insert("ldgDay", QString::number(ui->LandingSpinBox->value()));
            newData.insert("ldgNight", "0");
        }else if (nightTime == blockTime) { // all night
            newData.insert("ldgDay", "0");
            newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
        } else { //check
            QString destDate = ui->doftLineEdit->text() + 'T' + tonb.toString("hh:mm");
            QDateTime destDateTime = QDateTime::fromString(destDate,"yyyy-MM-ddThh:mm");
            if(ACalc::isNight(ui->destLocLineEdit->text(), destDateTime,  nightAngle)){
                newData.insert("ldgDay", "0");
                newData.insert("ldgNight", QString::number(ui->LandingSpinBox->value()));
            }else{
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
}
/*!
 * \brief NewFlight::fillExtrasLineEdits Fills the deductable items in the newData map and
 * additional flight time line edits according to ui selections.
 * Neccessary prerequisites are valid Date, Departure Time and Place, Destination Time and Place,
 * PIC name (pilot_id) and Aircraft (tail_id)
 */
void NewFlightDialog::fillExtras()
{
    //zero labels and line edits
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit, ui->tSPMETimeLineEdit, ui->tMPTimeLineEdit,    ui->tIFRTimeLineEdit,
                              ui->tNIGHTTimeLineEdit,ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                              ui->tDUALTimeLineEdit, ui->tFITimeLineEdit,};
    QList<QLabel*>      LB = {ui->tSPSELabel, ui->tSPMELabel,  ui->tMPLabel,  ui->tIFRLabel,  ui->tNIGHTLabel,
                              ui->tPICLabel,  ui->tPICUSLabel, ui->tSICLabel, ui->tDUALLabel, ui->tFILabel};
    for(const auto& widget : LE) {widget->setText("");}
    for(const auto& widget : LB) {widget->setText("00:00");}
    //Times
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    QString blockTime = ACalc::blocktime(tofb, tonb).toString("hh:mm");
    QString blockMinutes = QString::number(ACalc::stringToMinutes(blockTime));
    ui->tblkTimeLineEdit->setText(blockTime);
    auto acft = Aircraft(newData.value("acft").toInt());

    if(!acft.data.isEmpty()){// valid aircraft
        // SP SE
        if(acft.data.value("singlepilot") == "1" && acft.data.value("singleengine") == "1"){
            ui->tSPSETimeLineEdit->setText(blockTime);
            ui->tSPSELabel->setText(blockTime);
        }
        // SP ME
        if(acft.data.value("singlepilot") == "1" && acft.data.value("multiengine") == "1"){
            ui->tSPMETimeLineEdit->setText(blockTime);
            ui->tSPMELabel->setText(blockTime);
        }
        // MP
        if(acft.data.value("multipilot") == "1"){
            ui->tMPTimeLineEdit->setText(blockTime);
            ui->tMPLabel->setText(blockTime);
        }
    }else{DEB("Aircraft Details Empty");}//invalid aircraft

    // TOTAL
    ui->tblkLabel->setText("<b>" + blockTime + "</b>");
    ui->tblkLabel->setStyleSheet("color: green;");
    // IFR
    if(ui->IfrCheckBox->isChecked()){
        ui->tIFRTimeLineEdit->setText(blockTime);
        ui->tIFRLabel->setText(blockTime);
    }
    // Night
    QString deptDate = ui->doftLineEdit->text() + 'T' + tofb.toString("hh:mm");
    QDateTime deptDateTime = QDateTime::fromString(deptDate,"yyyy-MM-ddThh:mm");
    int tblk = blockMinutes.toInt();
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();

    QString nightTime = QString::number(
                        ACalc::calculateNightTime(
                        newData.value("dept"), newData.value("dest"),
                        deptDateTime, tblk, nightAngle));
    ui->tNIGHTTimeLineEdit->setText(ACalc::minutesToString(nightTime));
    ui->tNIGHTLabel->setText(ACalc::minutesToString(nightTime));
    // Function times
    switch (ui->FunctionComboBox->currentIndex()) {
    case 0://PIC
        ui->tPICTimeLineEdit->setText(blockTime);
        ui->tPICLabel->setText(blockTime);
        break;
    case 1://PICus
        ui->tPICUSTimeLineEdit->setText(blockTime);
        ui->tPICUSLabel->setText(blockTime);
        break;
    case 2://Co-Pilot
        ui->tSICTimeLineEdit->setText(blockTime);
        ui->tSICLabel->setText(blockTime);
        break;
    case 3://Dual
        ui->tDUALTimeLineEdit->setText(blockTime);
        ui->tDUALLabel->setText(blockTime);
        break;
    case 4://Instructor
        ui->tFITimeLineEdit->setText(blockTime);
        ui->tFILabel->setText(blockTime);
    }
}

bool NewFlightDialog::verifyInput()
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
        emit mandatoryFieldsValid(this);
        return true;
    }
}

/*!
 * ============================================================================
 * ============================================================================
 * Slots
 * ============================================================================
 * ============================================================================
 */

void NewFlightDialog::on_buttonBox_accepted()
{
    DEB("OK pressed");
    if(verifyInput()){
        DEB("Input verified");
        collectBasicData();
        collectAdditionalData();

        switch (role) {
        case Db::editExisting:
            entry.setData(newData);
            DEB("Editing entry: " << entry.position.first << " - " << entry.position.second);
            DEB("with Data: " << newData);
            DEB("Function Times: " << newData.value("tPIC") << newData.value("tPICus") << newData.value("tSIC")
                  << newData.value("tDUAL") << newData.value("tFI"));
            break;
        case Db::createNew:
            entry = Flight(newData);
            DEB("Creating New entry: " << entry.position.first << " - " << entry.position.second);
            DEB("with Data: " << newData);
            break;
        }

        if(entry.commit()){
            accept();
        }else{
            auto mb = new QMessageBox(this);
            auto errorMsg = QString("Unable to commit Flight to Logbook."
                                    "The following error has ocurred:\n\n");
            errorMsg.append(entry.error);
            mb->setText(errorMsg);
            mb->show();
        }
    }
}

void NewFlightDialog::on_buttonBox_rejected()
{
    DEB("CANCEL pressed." << newData);
    reject();
}
/*!
 * \brief onInputRejected Set `line_edit`'s border to red and check if `rgx` matches
 * in order to keep text on line. Ensures corresponding LineEdit bit is 0. Only
 * valid characters are kept on the line edit.
 */
void NewFlightDialog::onInputRejected(QLineEdit* line_edit, QRegularExpression rgx){
    DEB("Input rejected" << line_edit->text());
    line_edit->setStyleSheet("border: 1px solid red");
    this->allOkBits.setBit(this->lineEditBitMap[line_edit], false);
    auto text = line_edit->text();
    if(!rgx.match(text).hasMatch())
    {
        line_edit->setText(text);
    }
    if(INVALID_CHARS_RGX.match(text).hasMatch()){//remove globaly inacceptable chars
        DEB("Removing invalid char: " << text[text.length()-1]);
        text.chop(1);
        line_edit->setText(text);
    }
}

/*!
 * \brief onEditingFinishedCleanup resets styles and sets the corresponding bit to 1
 */
void NewFlightDialog::onEditingFinishedCleanup(QLineEdit* line_edit)
{
    //DEB("Input accepted" << line_edit << line_edit->text());
    line_edit->setStyleSheet("");
    this->allOkBits.setBit(this->lineEditBitMap[line_edit], true);
}

/// Date

void NewFlightDialog::on_doftLineEditEntered()
{
    const auto& cw = ui->calendarWidget;
    const auto& le = ui->doftLineEdit;
    const auto& anchor = ui->placeLabel1;

    if(cw->isVisible()){
        le->blockSignals(false);
        DEB("cw visible. Enabling line edit signals for: " << le->objectName());
        cw->hide();
        anchor->resize(ui->placeLabel2->size());
        le->setFocus();
    } else {
        le->blockSignals(true);
        DEB("Disabling line edit signals for: " << le->objectName());
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

void NewFlightDialog::date_clicked(const QDate &date)
{
    DEB("Date clicked: " << date);
    const auto& le = ui->doftLineEdit;
    le->blockSignals(false);
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
}

void NewFlightDialog::date_selected(const QDate &date)
{
    ui->calendarWidget->hide();
    ui->placeLabel1->resize(ui->placeLabel2->size());
    ui->doftDisplayLabel->setText(date.toString(Qt::TextDate));
    DEB("Date selected: " << date);
    const auto& le = ui->doftLineEdit;
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
    le->blockSignals(false);
    DEB("Enabling line edit signals for: " << le->objectName());
}

void NewFlightDialog::on_doftLineEdit_inputRejected()
{
    onInputRejected(ui->doftLineEdit, DATE_INVALID_RGX);
    ui->placeLabel1->resize(ui->placeLabel2->size());
    ui->calendarWidget->hide();
}

void NewFlightDialog::on_doftLineEdit_editingFinished()
{
    DEB(sender()->objectName() << "EDITING FINISHED.");
    auto line_edit = ui->doftLineEdit;
    auto text = ui->doftLineEdit->text();

    {//try to correct input if only numbers are entered, eg 20200101
        const QSignalBlocker blocker(line_edit);
        if(text.length() == 8) {
            text.insert(4,'-');
            text.insert(7,'-');
            DEB("editet text: " << text);
            auto date = QDate::fromString(text, Qt::ISODate);
            if(date.isValid()) {
                line_edit->setText(date.toString(Qt::ISODate));
                ui->doftDisplayLabel->setText(date.toString(Qt::TextDate));
            }
        }
    }
    auto date = QDate::fromString(text, Qt::ISODate);
    if(date.isValid()){
        onEditingFinishedCleanup(line_edit);
        const QSignalBlocker blocker(line_edit);
        ui->doftDisplayLabel->setText(date.toString(Qt::TextDate));
    } else {
        ui->doftDisplayLabel->setText("Invalid Date");
        emit line_edit->inputRejected();
    }
}

void NewFlightDialog::on_deptTZ_currentTextChanged(const QString &arg1)
{
    DEB(arg1);
    // currently only UTC time logging is supported
    ui->deptTZ->setCurrentIndex(0);
}

void NewFlightDialog::on_destTZ_currentIndexChanged(const QString &arg1)
{
    DEB(arg1);
    // currently only UTC time logging is supported
    ui->destTZ->setCurrentIndex(0);
}

/// Departure

void NewFlightDialog::on_deptLocLineEdit_inputRejected()
{
    //DEB("SENDER --->" << sender());
    ui->deptLocLineEdit->setText(ui->deptLocLineEdit->text().toUpper());
    onInputRejected(ui->deptLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlightDialog::on_deptLocLineEdit_textEdited(const QString &arg1)
{
    ui->deptLocLineEdit->setText(arg1.toUpper());
}

void NewFlightDialog::on_deptLocLineEdit_editingFinished()
{
    //DEB(sender()->objectName() << "EDITING FINISHED");
    auto line_edit = ui->deptLocLineEdit;
    auto text = ui->deptLocLineEdit->text();

    // check if iata exists, replace with icao code if it does.
    if(text.length() == 3){
        text = airportMap.value(text);
    }
    // Check if 4-letter code is in locationList
    if(text.length() == 4 && airports.indexOf(text) == -1){
        //DEB("Airport not found.");
        emit line_edit->inputRejected();
        return;
    }else{
        //DEB("Departure accepted: " << text);
        line_edit->setText(text);
        onEditingFinishedCleanup(line_edit); //reset style sheet
        ui->deptNameLabel->setText(Db::singleSelect("name","airports","icao",text,Db::exactMatch));
        update();
    }
}

void NewFlightDialog::on_tofbTimeLineEdit_inputRejected()
{
    onInputRejected(ui->tofbTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlightDialog::on_tofbTimeLineEdit_editingFinished()
{
    ui->tofbTimeLineEdit->setText(ACalc::formatTimeInput(ui->tofbTimeLineEdit->text()));
    const auto time = QTime::fromString(ui->tofbTimeLineEdit->text(),"hh:mm");

    auto line_edit = ui->tofbTimeLineEdit;
    onEditingFinishedCleanup(line_edit);

    if(time.isValid()){
        int minutes = time.hour() * 60 + time.minute();
        QString tofb = QString::number(minutes);
        //DEB("Time Off Blocks accepted: " << tofb << " minutes - " << Calc::minutesToString(tofb));
    }else{
        emit line_edit->inputRejected();
    }
    onEditingFinishedCleanup(line_edit);
    update();
}

/// Destination

void NewFlightDialog::on_destLocLineEdit_inputRejected()
{
    ui->destLocLineEdit->setText(ui->destLocLineEdit->text().toUpper());
    onInputRejected(ui->destLocLineEdit, QRegularExpression(LOC_INVALID_RGX));
}

void NewFlightDialog::on_destLocLineEdit_textEdited(const QString &arg1)
{
    ui->destLocLineEdit->setText(arg1.toUpper());
}

void NewFlightDialog::on_destLocLineEdit_editingFinished()
{
    //DEB(sender()->objectName() << "EDITING FINISHED");
    auto line_edit = ui->destLocLineEdit;
    auto text = ui->destLocLineEdit->text();

    // check if iata exists, replace with icao code if it does.
    if(text.length() == 3){
        text = airportMap.value(text);
    }
    // Check if 4-letter code is in locationList
    if(text.length() == 4 && airports.indexOf(text) == -1){
        //DEB("Airport not found.");
        emit line_edit->inputRejected();
        return;
    }else{
        //DEB("Destination accepted: " << text);
        line_edit->setText(text);
        onEditingFinishedCleanup(line_edit); //reset style sheet
        ui->destNameLabel->setText(Db::singleSelect("name","airports","icao",text,Db::exactMatch));
        update();
    }
}

void NewFlightDialog::on_tonbTimeLineEdit_inputRejected()
{
    onInputRejected(ui->tonbTimeLineEdit, QRegularExpression(TIME_INVALID_RGX));
}

void NewFlightDialog::on_tonbTimeLineEdit_editingFinished()
{
    ui->tonbTimeLineEdit->setText(ACalc::formatTimeInput(ui->tonbTimeLineEdit->text()));
    auto line_edit = ui->tonbTimeLineEdit;
    const auto time = QTime::fromString(ui->tonbTimeLineEdit->text(),"hh:mm");
    if(time.isValid()){
        int minutes = time.hour() * 60 + time.minute();
        QString tonb = QString::number(minutes);
        newData.insert("tonb",tonb);
        //DEB("Time On Blocks accepted: " << tonb << " minutes - " << Calc::minutesToString(tonb));
    }else{
        emit line_edit->inputRejected();
    }
    onEditingFinishedCleanup(line_edit);
    update();
}

/// Date

void NewFlightDialog::on_doftTimeEdit_editingFinished()
{
    update();
}

/// Aircraft

void NewFlightDialog::on_acftLineEdit_inputRejected()
{
    ui->acftLineEdit->setText(ui->acftLineEdit->text().toUpper());
    onInputRejected(ui->acftLineEdit, QRegularExpression(AIRCRAFT_INVALID_RGX));
}

void NewFlightDialog::on_acftLineEdit_editingFinished()
{
    auto registrationList = experimental::aDB()->getCompletionList(experimental::ADataBase::registrations);
    auto line_edit = ui->acftLineEdit;
    auto text = ui->acftLineEdit->text();

    QStringList match = registrationList.filter(line_edit->text(), Qt::CaseInsensitive);
    //DEB("aircraft accepted: " << match);
    if(match.length() != 0) {
        text = match[0];
        line_edit->setText(text.toUpper());
        onEditingFinishedCleanup(line_edit);
        QString column = "make||' '||model||'-'||variant";
        ui->acftTypeLabel->setText(
                    Db::singleSelect(column,"tails","registration",text,Db::exactMatch));
        update();
    }else{
        emit line_edit->inputRejected();
        addNewAircraftMessageBox(line_edit);
    }
}

/// Pilot(s)

void NewFlightDialog::on_picNameLineEdit_inputRejected()
{
    onInputRejected(ui->picNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlightDialog::on_picNameLineEdit_editingFinished()
{
    auto line_edit = ui->picNameLineEdit;
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is PIC
    {
        //DEB("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        ui->FunctionComboBox->setCurrentIndex(0);
        update();
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = experimental::aDB()->getCompletionList(experimental::ADataBase::pilots);
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            //DEB("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
            update();
        }else
        {
            DEB("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox(line_edit);
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

void NewFlightDialog::on_secondPilotNameLineEdit_inputRejected()
{
    onInputRejected(ui->secondPilotNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlightDialog::on_secondPilotNameLineEdit_editingFinished()
{
    auto line_edit = ui->secondPilotNameLineEdit;
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is Pilot
    {
        //DEB("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        ui->FunctionComboBox->setCurrentIndex(1);
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = experimental::aDB()->getCompletionList(experimental::ADataBase::pilots);
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            //DEB("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
        }else
        {
            DEB("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox(line_edit);
        }
    }
}

void NewFlightDialog::on_thirdPilotNameLineEdit_inputRejected()
{
    onInputRejected(ui->thirdPilotNameLineEdit, QRegularExpression(PILOT_NAME_INVALID_RGX));
}

void NewFlightDialog::on_thirdPilotNameLineEdit_editingFinished()
{
    auto line_edit = ui->thirdPilotNameLineEdit;
    auto text = line_edit->text();
    if(text == "self" || text == "SELF") // Logbook owner is Pilot
    {
        //DEB("Pilot selected: " << text);
        onEditingFinishedCleanup(line_edit);
        return;
    }else //check if entry is in pilotList
    {
        QStringList pilotList = experimental::aDB()->getCompletionList(experimental::ADataBase::pilots);
        QStringList match = pilotList.filter(line_edit->text().remove(" "), Qt::CaseInsensitive);

        if(match.length()!= 0)
        {
            QString pic = match[0];
            line_edit->setText(pic.insert(pic.indexOf(',')+1," "));
            //DEB("Pilot selected: " << pic);
            onEditingFinishedCleanup(line_edit);
        }else
        {
            DEB("Pilot not found.");
            emit line_edit->inputRejected();
            addNewPilotMessageBox(line_edit);
        }
    }
}

void NewFlightDialog::on_FlightNumberLineEdit_textChanged(const QString &arg1)
{
    ui->FlightNumberLineEdit->setText(arg1.toUpper());
}


/*
 * ============================================================================
 * Extras Tab - These are for user convenience. From many of
 * these selections, determinations can be made on how to log
 * details, so that the user does not have to enter each item
 * manually. See also fillExtrasLineEdits()
 * ============================================================================
 */

void NewFlightDialog::on_setAsDefaultButton_clicked()
{
    writeSettings();
}

void NewFlightDialog::on_restoreDefaultButton_clicked()
{
    readSettings();
}

/*!
 * \brief On a given flight, time can either be logged as Pilot Flying (PF) or
 * Pilot Monitoring (PM). Cases where controls are changed during the flight
 * are rare and can be logged by manually editing the extras.
 */
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

void NewFlightDialog::on_IfrCheckBox_stateChanged()
{
    update();
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

inline bool NewFlightDialog::isLessOrEqualToTotalTime(QString timeString)
{
    if(newData.value("tblk").isEmpty()){
        DEB("Total Time not set.");
        auto mb = new QMessageBox(this);
        mb->setText("Please fill out Departure and Arrival Time\n"
                    "before manually editing these times.");
        mb->show();
        return false;
    } else {
        int minutes = ACalc::stringToMinutes(timeString);
        if (minutes <= newData.value("tblk").toInt()) {
            return true;
        } else {
            auto mb = new QMessageBox(this);
            mb->setText("Cannot be more than Total Time of Flight:<br><br><center><b>"
                        + ACalc::minutesToString(newData.value("tblk"))
                        + "</b></center><br>");
            mb->show();
            return false;
        }
    }

}

void NewFlightDialog::on_tblkTimeLineEdit_editingFinished()
{
    const auto &le = ui->tblkTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    newData.insert("tblk",QString::number(ACalc::stringToMinutes(text)));
    le->setText(QString());
}

void NewFlightDialog::on_tSPSETimeLineEdit_editingFinished()
{
    const auto &le = ui->tSPSETimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSPSE",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tSPMETimeLineEdit_editingFinished()
{
    const auto &le = ui->tSPMETimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSPME",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tMPTimeLineEdit_editingFinished()
{
    const auto &le = ui->tMPTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tMP",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}
void NewFlightDialog::on_tIFRTimeLineEdit_editingFinished()
{
    const auto &le = ui->tIFRTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tIFR",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tNIGHTTimeLineEdit_editingFinished()
{
    const auto &le = ui->tNIGHTTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tNIGHT",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tPICTimeLineEdit_editingFinished()
{
    const auto &le = ui->tPICTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tPIC",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tSICTimeLineEdit_editingFinished()
{
    const auto &le = ui->tSICTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tSIC",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tDUALTimeLineEdit_editingFinished()
{
    const auto &le = ui->tDUALTimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tDUAL",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}

void NewFlightDialog::on_tFITimeLineEdit_editingFinished()
{
    const auto &le = ui->tFITimeLineEdit;
    le->setText(ACalc::formatTimeInput(le->text()));
    const auto &text = le->text();

    if(isLessOrEqualToTotalTime(text)){
        newData.insert("tFI",QString::number(ACalc::stringToMinutes(text)));
    } else {
        le->setText(QString());
    }
}


void NewFlightDialog::on_manualEditingCheckBox_stateChanged(int arg1)
{
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit, ui->tSPMETimeLineEdit, ui->tMPTimeLineEdit,    ui->tIFRTimeLineEdit,
                              ui->tNIGHTTimeLineEdit,ui->tPICTimeLineEdit,  ui->tPICUSTimeLineEdit, ui->tSICTimeLineEdit,
                              ui->tDUALTimeLineEdit, ui->tFITimeLineEdit,};
    switch (arg1) {
    case 0:
        for(const auto& le : LE){
            le->setFocusPolicy(Qt::NoFocus);
            doUpdate = true;
            update();
        }
        break;
    case 2:
        for(const auto& le : LE){
            le->setFocusPolicy(Qt::StrongFocus);
            doUpdate = false;
        }
        break;
    default:
        break;
    }
}

void NewFlightDialog::on_FunctionComboBox_currentTextChanged()
{
    DEB("Current Index:" << ui->FunctionComboBox->currentIndex());
    update();
}

void NewFlightDialog::on_TakeoffSpinBox_valueChanged(int arg1)
{
    if(arg1 > 0) {
        ui->TakeoffCheckBox->setChecked(true);
    }
}

void NewFlightDialog::on_LandingSpinBox_valueChanged(int arg1)
{
    if(arg1 > 0) {
        ui->LandingCheckBox->setChecked(true);
    }
}

void NewFlightDialog::on_AutolandSpinBox_valueChanged(int arg1)
{
    if(arg1 > 0) {
        ui->AutolandCheckBox->setChecked(true);
    }
}

void NewFlightDialog::on_TakeoffCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0) {
        ui->TakeoffSpinBox->setValue(0);
    } else if ( arg1 == 2) {
        ui->TakeoffSpinBox->setValue(1);
    }
}

void NewFlightDialog::on_LandingCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0) {
        ui->LandingSpinBox->setValue(0);
    } else if ( arg1 == 2) {
        ui->LandingSpinBox->setValue(1);
    }
}

void NewFlightDialog::on_AutolandCheckBox_stateChanged(int arg1)
{
    if(arg1 == 0) {
        ui->AutolandSpinBox->setValue(0);
    } else if ( arg1 == 2) {
        ui->AutolandSpinBox->setValue(1);
    }
}
