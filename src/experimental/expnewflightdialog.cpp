#include "expnewflightdialog.h"
#include "ui_expnewflightdialog.h"

using namespace experimental;

/////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
void ExpNewFlightDialog::onInputRejected()
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

static const auto IATA_RX = QLatin1String("[a-zA-Z0-9]{3}");
static const auto ICAO_RX = QLatin1String("[a-zA-Z0-9]{4}");
static const auto NAME_RX = QLatin1String("(\\p{L}+('|\\-|,)?\\p{L}+?)");
static const auto ADD_NAME_RX = QLatin1String("(\\s?(\\p{L}+('|\\-|,)?\\p{L}+?))?");
static const auto SELF_RX = QLatin1String("(self|SELF)");

/// Raw Input validation
static const auto TIME_VALID_RGX       = QRegularExpression("([01]?[0-9]|2[0-3]):?[0-5][0-9]?");// We only want to allow inputs that make sense as a time, e.g. 99:99 is not a valid time
static const auto LOC_VALID_RGX        = QRegularExpression(IATA_RX + "|" + ICAO_RX);
static const auto AIRCRAFT_VALID_RGX   = QRegularExpression("\\w+\\-?(\\w+)?");
static const auto NAME_VALID_RGX       = QRegularExpression(SELF_RX + QLatin1Char('|')
                                                      + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ",?\\s?" // up to 4 first names
                                                      + NAME_RX + ADD_NAME_RX + ADD_NAME_RX + ADD_NAME_RX );// up to 4 last names
static const auto DATE_VALID_RGX       = QRegularExpression("^([1-9][0-9]{3}).?(1[0-2]|0[1-9]).?(3[01]|0[1-9]|[12][0-9])?$");// . allows all seperators, still allows for 2020-02-31, additional verification via QDate::isValid()


///////////////////////////////////////////////////////////////////////////////////////////////////
///                                      Construction                                           ///
///////////////////////////////////////////////////////////////////////////////////////////////////

/// [F] The general idea for this dialog is this:
/// - Most line edits have validators and completers.
/// - Validators are based on regular expressions, serving as raw input validation
/// - The Completers are based off the database and provide auto-completion
/// - mandatory line edits only emit editing finished if their content is
/// valid. This means mapping user inputs to database keys where required.
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

ExpNewFlightDialog::ExpNewFlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = AFlightEntry();
    setup();
}

ExpNewFlightDialog::ExpNewFlightDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpNewFlightDialog)
{
    ui->setupUi(this);
    flightEntry = aDB()->getFlightEntry(row_id);
    setup();
}

ExpNewFlightDialog::~ExpNewFlightDialog()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                        Methods - setup and maintenance of dialog                            ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void ExpNewFlightDialog::setup()
{
    updateEnabled = true;
    setupButtonGroups();
    setupRawInputValidation();
    setupLineEditSignalsAndSlots();
    readSettings();

    // Visually mark mandatory fields
    ui->deptLocLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->destLocLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->tofbTimeLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->tonbTimeLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->picNameLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");
    ui->acftLineEdit_2->setStyleSheet("border: 0.1ex solid #3daee9");

    ui->doftLineEdit_2->setText(QDate::currentDate().toString(Qt::ISODate));
    emit ui->doftLineEdit_2->editingFinished();
    ui->deptLocLineEdit_2->setFocus();

    if(ASettings::read("NewFlight/FunctionComboBox").toString() == "PIC"){
        ui->picNameLineEdit_2->setText("self");
        emit ui->picNameLineEdit_2->editingFinished();
    }
}
void ExpNewFlightDialog::readSettings()
{
    DEB("Reading Settings...");
    QSettings settings;

    ui->FunctionComboBox_2->setCurrentText(ASettings::read("NewFlight/FunctionComboBox").toString());
    ui->ApproachComboBox_2->setCurrentText(ASettings::read("NewFlight/ApproachComboBox").toString());
    ui->PilotFlyingCheckBox_2->setChecked(ASettings::read("NewFlight/PilotFlyingCheckBox").toBool());
    ui->PilotMonitoringCheckBox_2->setChecked(ASettings::read("NewFlight/PilotMonitoringCheckBox").toBool());
    ui->TakeoffSpinBox_2->setValue(ASettings::read("NewFlight/TakeoffSpinBox").toInt());
    ui->TakeoffCheckBox_2->setChecked(ASettings::read("NewFlight/TakeoffCheckBox").toBool());
    ui->LandingSpinBox_2->setValue(ASettings::read("NewFlight/LandingSpinBox").toInt());
    ui->LandingCheckBox_2->setChecked(ASettings::read("NewFlight/LandingCheckBox").toBool());
    ui->AutolandSpinBox_2->setValue(ASettings::read("NewFlight/AutolandSpinBox").toInt());
    ui->AutolandCheckBox_2->setChecked(ASettings::read("NewFlight/AutolandCheckBox").toBool());
    ui->IfrCheckBox_2->setChecked(ASettings::read("NewFlight/IfrCheckBox").toBool());
    ui->VfrCheckBox_2->setChecked(ASettings::read("NewFlight/VfrCheckBox").toBool());
    ui->FlightNumberLineEdit_2->setText(ASettings::read("flightlogging/flightnumberPrefix").toString());
    ui->calendarCheckBox->setChecked(ASettings::read("NewFlight/calendarCheckBox").toBool());

    setPopUpCalendarEnabled(ASettings::read("NewFlight/calendarCheckBox").toBool());

    if (ASettings::read("NewFlight/FunctionComboBox").toString() == "Co-Pilot") {
        ui->picNameLineEdit_2->setText("");
        ui->secondPilotNameLineEdit_2->setText("self");
    }
}

void ExpNewFlightDialog::writeSettings()
{
    DEB("Writing Settings...");

    ASettings::write("NewFlight/FunctionComboBox",ui->FunctionComboBox_2->currentText());
    ASettings::write("NewFlight/ApproachComboBox",ui->ApproachComboBox_2->currentText());
    ASettings::write("NewFlight/PilotFlyingCheckBox",ui->PilotFlyingCheckBox_2->isChecked());
    ASettings::write("NewFlight/PilotMonitoringCheckBox",ui->PilotMonitoringCheckBox_2->isChecked());
    ASettings::write("NewFlight/TakeoffSpinBox",ui->TakeoffSpinBox_2->value());
    ASettings::write("NewFlight/TakeoffCheckBox",ui->TakeoffCheckBox_2->isChecked());
    ASettings::write("NewFlight/LandingSpinBox",ui->LandingSpinBox_2->value());
    ASettings::write("NewFlight/LandingCheckBox",ui->LandingCheckBox_2->isChecked());
    ASettings::write("NewFlight/AutolandSpinBox",ui->AutolandSpinBox_2->value());
    ASettings::write("NewFlight/AutolandCheckBox",ui->AutolandCheckBox_2->isChecked());
    ASettings::write("NewFlight/IfrCheckBox",ui->IfrCheckBox_2->isChecked());
    ASettings::write("NewFlight/VfrCheckBox",ui->VfrCheckBox_2->isChecked());
    ASettings::write("NewFlight/calendarCheckBox",ui->calendarCheckBox->isChecked());
}

void ExpNewFlightDialog::setupButtonGroups()
{
    QButtonGroup *FlightRulesGroup = new QButtonGroup(this);
    FlightRulesGroup->addButton(ui->IfrCheckBox_2);
    FlightRulesGroup->addButton(ui->VfrCheckBox_2);

    QButtonGroup *PilotTaskGroup = new QButtonGroup(this);
    PilotTaskGroup->addButton(ui->PilotFlyingCheckBox_2);
    PilotTaskGroup->addButton(ui->PilotMonitoringCheckBox_2);
}

void ExpNewFlightDialog::setupRawInputValidation()
{
    // get Maps
    pilotsIdMap      = aDB()->getIdMap(ADataBase::pilots);
    tailsIdMap       = aDB()->getIdMap(ADataBase::tails);
    airportIcaoIdMap = aDB()->getIdMap(ADataBase::airport_identifier_icao);
    airportIataIdMap = aDB()->getIdMap(ADataBase::airport_identifier_iata);
    airportNameIdMap = aDB()->getIdMap(ADataBase::airport_names);
    pilotsIdMap.insert("SELF", 1);
    pilotsIdMap.insert("self", 1);
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
    auto line_edits = ui->flightDataTab_2->findChildren<QLineEdit*>();

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
        ui->doftLineEdit_2,
        ui->deptLocLineEdit_2,
        ui->destLocLineEdit_2,
        ui->tofbTimeLineEdit_2,
        ui->tonbTimeLineEdit_2,
        ui->picNameLineEdit_2,
        ui->acftLineEdit_2,
    };
    mandatoryLineEditsGood.resize(mandatoryLineEdits.size());
    primaryTimeLineEdits = {
        ui->tofbTimeLineEdit_2,
        ui->tonbTimeLineEdit_2
    };
}

void ExpNewFlightDialog::setupLineEditSignalsAndSlots()
{

    auto line_edits = this->findChildren<QLineEdit*>();

    for(const auto &line_edit : line_edits){
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        QObject::connect(line_edit, &QLineEdit::inputRejected,
                         this, &ExpNewFlightDialog::onInputRejected);
        /////////////////////////////////////// DEBUG /////////////////////////////////////////////////////
        QObject::connect(line_edit->completer(), QOverload<const QString &>::of(&QCompleter::highlighted),
                         this, &ExpNewFlightDialog::onCompleterHighlighted);
        if(line_edit->objectName().contains("Loc") || line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &ExpNewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("acft")){
            QObject::connect(line_edit, &QLineEdit::textChanged,
                             this, &ExpNewFlightDialog::onTextChangedToUpper);
        }
        if(line_edit->objectName().contains("Name")){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &ExpNewFlightDialog::onPilotLineEdit_editingFinished);
        }
        if(line_edit->objectName().contains("Time")){
            QObject::connect(line_edit, &QLineEdit::editingFinished,
                             this, &ExpNewFlightDialog::onTimeLineEdit_editingFinished);
        }
    }
    QObject::connect(this, &ExpNewFlightDialog::goodInputReceived,
                     this, &ExpNewFlightDialog::onGoodInputReceived);
    QObject::connect(this, &ExpNewFlightDialog::badInputReceived,
                     this, &ExpNewFlightDialog::onBadInputReceived);
    QObject::connect(this, &ExpNewFlightDialog::locationEditingFinished,
                     this, &ExpNewFlightDialog::onLocLineEdit_editingFinished);
    QObject::connect(this, &ExpNewFlightDialog::timeEditingFinished,
                     this, &ExpNewFlightDialog::onTimeLineEdit_editingFinished);
    QObject::connect(this, &ExpNewFlightDialog::mandatoryLineEditsFilled,
                     this, &ExpNewFlightDialog::onMandatoryLineEditsFilled);
}

void ExpNewFlightDialog::setPopUpCalendarEnabled(bool state)
{
    ui->flightDataTabWidget->setCurrentIndex(0);
    ui->flightDataTabWidget->removeTab(2); // hide calendar widget

    if (state) {
    DEB("Enabling pop-up calendar widget...");
    ui->calendarWidget_2->installEventFilter(this);
    ui->placeLabel1_2->installEventFilter(this);
    ui->doftLineEdit_2->installEventFilter(this);

    QObject::connect(ui->calendarWidget_2, &QCalendarWidget::clicked,
                     this, &ExpNewFlightDialog::onDateClicked);
    QObject::connect(ui->calendarWidget_2, &QCalendarWidget::activated,
                     this, &ExpNewFlightDialog::onDateSelected);
    } else {
        DEB("Disabling pop-up calendar widget...");
        ui->calendarWidget_2->removeEventFilter(this);
        ui->placeLabel1_2->removeEventFilter(this);
        ui->doftLineEdit_2->removeEventFilter(this);
        QObject::disconnect(ui->calendarWidget_2, &QCalendarWidget::clicked,
                            this, &ExpNewFlightDialog::onDateClicked);
        QObject::disconnect(ui->calendarWidget_2, &QCalendarWidget::activated,
                         this, &ExpNewFlightDialog::onDateSelected);
    }
}

bool ExpNewFlightDialog::eventFilter(QObject* object, QEvent* event)
{
    if(object == ui->doftLineEdit_2 && event->type() == QEvent::MouseButtonPress) {
        onDoftLineEditEntered();
        return false; // let the event continue to the edit
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///                                 Methods - Input Processing                                  ///
///////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief Fills the deductable items in the Dialog based on mandatory data ui selections.
 */
void ExpNewFlightDialog::fillDeductibleData()
{

    //zero out labels and line edits to delete previous calculations
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit_2, ui->tSPMETimeLineEdit_2, ui->tMPTimeLineEdit_2,    ui->tIFRTimeLineEdit_2,
                              ui->tNIGHTTimeLineEdit_2,ui->tPICTimeLineEdit_2,  ui->tPICUSTimeLineEdit_2, ui->tSICTimeLineEdit_2,
                              ui->tDUALTimeLineEdit_2, ui->tFITimeLineEdit_2,};
    QList<QLabel*>      LB = {ui->tSPSELabel_2, ui->tSPMELabel_2,  ui->tMPLabel_2,  ui->tIFRLabel_2,  ui->tNIGHTLabel_2,
                              ui->tPICLabel_2,  ui->tPICUSLabel_2, ui->tSICLabel_2, ui->tDUALLabel_2, ui->tFILabel_2};
    for(const auto& widget : LE) {widget->setText("");}
    for(const auto& widget : LB) {widget->setText("00:00");}

    //Calculate block time
    auto tofb = QTime::fromString(ui->tofbTimeLineEdit_2->text(), TIME_FORMAT);
    auto tonb = QTime::fromString(ui->tonbTimeLineEdit_2->text(), TIME_FORMAT);
    QString block_time = ACalc::blocktime(tofb, tonb).toString(TIME_FORMAT);
    QString block_minutes = QString::number(ACalc::stringToMinutes(block_time));
    ui->tblkTimeLineEdit_2->setText(block_minutes);

    // get acft data and fill deductible entries
    auto acft = aDB()->getTailEntry(tailsIdMap.value(ui->acftLineEdit_2->text()));
    DEB("Created acft object with data: " << acft.getData());

    if (acft.getData().isEmpty())
        DEB("No valid aircraft object available.");
    // SP SE
    if(acft.getData().value("singlepilot") == "1" && acft.getData().value("singleengine") == "1"){
        ui->tSPSETimeLineEdit_2->setText(block_minutes);
        ui->tSPSELabel_2->setText(block_minutes);
    }
    // SP ME
    if(acft.getData().value("singlepilot") == "1" && acft.getData().value("multiengine") == "1"){
        ui->tSPMETimeLineEdit_2->setText(block_minutes);
        ui->tSPMELabel_2->setText(block_minutes);
    }
    // MP
    if(acft.getData().value("multipilot") == "1"){
        ui->tMPTimeLineEdit_2->setText(block_minutes);
        ui->tMPLabel_2->setText(block_minutes);
    }
    // TOTAL
    ui->tblkLabel_3->setText("<b>" + block_minutes + "</b>");
    ui->tblkLabel_3->setStyleSheet("color: green;");
    // IFR
    if(ui->IfrCheckBox_2->isChecked()){
        ui->tIFRTimeLineEdit_2->setText(block_minutes);
        ui->tIFRLabel_2->setText(block_minutes);
    }
    // Night
    QString dept_date = ui->doftLineEdit_2->text() + 'T' + tofb.toString(TIME_FORMAT);
    QDateTime dept_date_time = QDateTime::fromString(dept_date,"yyyy-MM-ddThh:mm");
    int tblk = block_minutes.toInt();
    const int night_angle = ASettings::read("flightlogging/nightangle").toInt();
    QString night_minutes = QString::number(
                ACalc::calculateNightTime(
                ui->deptLocLineEdit_2->text(),
                ui->destLocLineEdit_2->text(),
                dept_date_time,
                tblk,
                night_angle));

    ui->tNIGHTTimeLineEdit_2->setText(ACalc::minutesToString(night_minutes));
    ui->tNIGHTLabel_2->setText(ACalc::minutesToString(night_minutes));
    // Function times
    switch (ui->FunctionComboBox_2->currentIndex()) {
    case 0://PIC
        ui->tPICTimeLineEdit_2->setText(block_minutes);
        ui->tPICLabel_2->setText(block_minutes);
        break;
    case 1://PICus
        ui->tPICUSTimeLineEdit_2->setText(block_minutes);
        ui->tPICUSLabel_2->setText(block_minutes);
        break;
    case 2://Co-Pilot
        ui->tSICTimeLineEdit_2->setText(block_minutes);
        ui->tSICLabel_2->setText(block_minutes);
        break;
    case 3://Dual
        ui->tDUALTimeLineEdit_2->setText(block_minutes);
        ui->tDUALLabel_2->setText(block_minutes);
        break;
    case 4://Instructor
        ui->tFITimeLineEdit_2->setText(block_minutes);
        ui->tFILabel_2->setText(block_minutes);
    }
}
/*!
 * \brief Collect input and create a Data map for the entry object.
 *
 * This function should only be called if input validation has been passed, since
 * no input validation is done in this step and input data is assumed to be valid.
 * \return
 */
TableData ExpNewFlightDialog::collectInput()
{
    TableData newData;
    DEB("Collecting Input...");
    // Mandatory data
    newData.insert("doft", ui->doftLineEdit_2->text());
    newData.insert("dept",ui->deptLocLineEdit_2->text());
    newData.insert("tofb", QString::number(
                       ACalc::stringToMinutes(ui->tofbTimeLineEdit_2->text())));
    newData.insert("dest",ui->destLocLineEdit_2->text());
    newData.insert("tonb", QString::number(
                       ACalc::stringToMinutes(ui->tonbTimeLineEdit_2->text())));
    //Block Time
    const auto tofb = QTime::fromString(ui->tofbTimeLineEdit_2->text(), TIME_FORMAT);
    const auto tonb = QTime::fromString(ui->tonbTimeLineEdit_2->text(), TIME_FORMAT);
    const QString block_time = ACalc::blocktime(tofb, tonb).toString(TIME_FORMAT);
    const QString block_minutes = QString::number(ACalc::stringToMinutes(block_time));

    newData.insert("tblk", block_minutes);
    // Aircraft and Pilots
    newData.insert("acft", QString::number(tailsIdMap.value(ui->acftLineEdit_2->text())));
    newData.insert("pic", QString::number(pilotsIdMap.value(ui->picNameLineEdit_2->text())));
    newData.insert("secondPilot", QString::number(pilotsIdMap.value(ui->secondPilotNameLineEdit_2->text())));
    newData.insert("thirdPilot", QString::number(pilotsIdMap.value(ui->thirdPilotNameLineEdit_2->text())));

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

    if (ui->IfrCheckBox_2->isChecked()) {
        newData.insert("tIFR", block_minutes);
    } else {
        newData.insert("tIFR", "");
    }
    // Night
    const auto dept_date = ui->doftLineEdit_2->text() + 'T' + tofb.toString(TIME_FORMAT);
    const auto dept_date_time = QDateTime::fromString(dept_date,"yyyy-MM-ddThh:mm");
    const auto tblk = block_minutes.toInt();
    const auto night_angle = ASettings::read("flightlogging/nightangle").toInt();
    const auto night_minutes = QString::number(
                ACalc::calculateNightTime(
                ui->deptLocLineEdit_2->text(),
                ui->destLocLineEdit_2->text(),
                dept_date_time,
                tblk,
                night_angle));
    newData.insert("tNIGHT", night_minutes);

    // Function times - This is a little explicit but these are mutually exclusive so its better to be safe than sorry here.
    switch (ui->FunctionComboBox_2->currentIndex()) {
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
    newData.insert("pilotFlying", QString::number(ui->PilotFlyingCheckBox_2->isChecked()));
    // TO and LDG - again a bit explicit, but we  need to check for both night to day as well as day to night transitions.
    if (ui->TakeoffCheckBox_2->isChecked()) {
        if (night_minutes == "0") { // all day
            newData.insert("toDay", QString::number(ui->TakeoffSpinBox_2->value()));
            newData.insert("toNight", "0");
        } else if (night_minutes == block_minutes) { // all night
            newData.insert("toDay", "0");
            newData.insert("toNight", QString::number(ui->TakeoffSpinBox_2->value()));
        } else {
            if(ACalc::isNight(ui->deptLocLineEdit_2->text(), dept_date_time,  night_angle)) {
                newData.insert("toDay", "0");
                newData.insert("toNight", QString::number(ui->TakeoffSpinBox_2->value()));
            } else {
                newData.insert("toDay", QString::number(ui->TakeoffSpinBox_2->value()));
                newData.insert("toNight", "0");
            }
        }
    } else {
        newData.insert("toDay", "0");
        newData.insert("toNight", "0");
    }

    if (ui->LandingCheckBox_2->isChecked()) {
        if (night_minutes == "0") { // all day
            newData.insert("ldgDay", QString::number(ui->LandingSpinBox_2->value()));
            newData.insert("ldgNight", "0");
        } else if (night_minutes == block_minutes) { // all night
            newData.insert("ldgDay", "0");
            newData.insert("ldgNight", QString::number(ui->LandingSpinBox_2->value()));
        } else { //check
            const auto dest_date = ui->doftLineEdit_2->text() + 'T' + tonb.toString(TIME_FORMAT);
            const auto dest_date_time = QDateTime::fromString(dest_date,"yyyy-MM-ddThh:mm");
            if (ACalc::isNight(ui->destLocLineEdit_2->text(), dest_date_time,  night_angle)) {
                newData.insert("ldgDay", "0");
                newData.insert("ldgNight", QString::number(ui->LandingSpinBox_2->value()));
            } else {
                newData.insert("ldgDay", QString::number(ui->LandingSpinBox_2->value()));
                newData.insert("ldgNight", "0");
            }
        }
    } else {
        newData.insert("ldgDay", "0");
        newData.insert("ldgNight", "0");
    }


    newData.insert("autoland", QString::number(ui->AutolandSpinBox_2->value()));
    newData.insert("ApproachType", ui->ApproachComboBox_2->currentText());
    newData.insert("FlightNumber", ui->FlightNumberLineEdit_2->text());
    newData.insert("Remarks", ui->RemarksLineEdit_2->text());

    return newData;
}

bool ExpNewFlightDialog::isLessOrEqualThanBlockTime(const QString time_string)
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
                                           ui->tofbTimeLineEdit_2->text(),TIME_FORMAT),
                                       QTime::fromString(
                                           ui->tonbTimeLineEdit_2->text(), TIME_FORMAT));
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///                               Flight Data Tab Slots                                         ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void ExpNewFlightDialog::on_cancelButton_clicked()
{
    DEB("Cancel Button clicked.");
    //reject();
}

// submit button can only be clicked if mandatory line edits are valid.
void ExpNewFlightDialog::on_submitButton_clicked()
{
    DEB("Submit Button clicked.");
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

void ExpNewFlightDialog::onGoodInputReceived(QLineEdit *line_edit)
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

void ExpNewFlightDialog::onBadInputReceived(QLineEdit *line_edit)
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
void ExpNewFlightDialog::onTextChangedToUpper(const QString &text)
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
void ExpNewFlightDialog::onMandatoryLineEditsFilled()
{
    ui->submitButton->setEnabled(true);
    ui->submitButton->setStyleSheet("border: 1px solid rgb(78, 154, 6);"
                                    "color: rgb(78, 154, 6);");
    DEB("Commit Button Enabled. Ready for commit!");
    if (updateEnabled)
        fillDeductibleData();
}

// make sure that when using keyboard to scroll through completer sugggestions, line edit is up to date
void ExpNewFlightDialog::onCompleterHighlighted(const QString &completion)
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB("Completer highlighted - " << line_edit->objectName() << completion);
    line_edit->setText(completion);
}

/*
 *  Date of Flight
 */

void ExpNewFlightDialog::on_doftLineEdit_2_editingFinished()
{
    auto line_edit = ui->doftLineEdit_2;
    auto text = ui->doftLineEdit_2->text();
    auto label = ui->doftDisplayLabel_2;
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

void ExpNewFlightDialog::onDateClicked(const QDate &date)
{
    const auto& le = ui->doftLineEdit_2;
    le->blockSignals(false);
    ui->calendarWidget_2->hide();
    ui->placeLabel1_2->resize(ui->placeLabel2_2->size());
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
}

void ExpNewFlightDialog::onDateSelected(const QDate &date)
{
    ui->calendarWidget_2->hide();
    ui->placeLabel1_2->resize(ui->placeLabel2_2->size());
    ui->doftDisplayLabel_2->setText(date.toString(Qt::TextDate));
    const auto& le = ui->doftLineEdit_2;
    le->setText(date.toString(Qt::ISODate));
    le->setFocus();
    le->blockSignals(false);
}

void ExpNewFlightDialog::onDoftLineEditEntered()
{
    const auto& cw = ui->calendarWidget_2;
    const auto& le = ui->doftLineEdit_2;
    const auto& anchor = ui->placeLabel1_2;

    if(cw->isVisible()){
        le->blockSignals(false);
        cw->hide();
        anchor->resize(ui->placeLabel2_2->size());
        le->setFocus();
    } else {
        le->blockSignals(true);
        // Determine size based on layout coordinates
        int c1 = anchor->pos().rx();
        int c2 = le->pos().rx();
        int z  = le->size().rwidth();
        int x = (c2 - c1)+ z;
        c1 = anchor->pos().ry();
        c2 = ui->acftLineEdit_2->pos().ry();
        z  = ui->acftLineEdit_2->size().height();
        int y = (c2 - c1) + z;
        //Re-size calendar and parent label accordingly
        anchor->resize(x, y);
        cw->setParent(ui->placeLabel1_2);
        cw->setGeometry(QRect(0, 0, x, y));
        cw->show();
        cw->setFocus();
    }
}

void ExpNewFlightDialog::on_calendarCheckBox_stateChanged(int arg1)
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

void ExpNewFlightDialog::on_deptLocLineEdit_2_editingFinished()
{
    emit locationEditingFinished(ui->deptLocLineEdit_2, ui->deptNameLabel_2);
}

void ExpNewFlightDialog::on_destLocLineEdit_2_editingFinished()
{
    emit locationEditingFinished(ui->destLocLineEdit_2, ui->destNameLabel_2);
}

void ExpNewFlightDialog::onLocLineEdit_editingFinished(QLineEdit *line_edit, QLabel *name_label)
{
    const auto &text = line_edit->text();
    DEB(line_edit->objectName() << " Editing finished. " << text);
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

void ExpNewFlightDialog::onTimeLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Editing Finished -" << line_edit->text());

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

void ExpNewFlightDialog::on_acftLineEdit_2_editingFinished()
{
    auto line_edit = ui->acftLineEdit_2;
    DEB(line_edit->objectName() << "Editing Finished!" << line_edit->text());

    if (tailsIdMap.value(line_edit->text()) != 0) {
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
    ui->acftTypeLabel_2->setText("Unknown Registration.");


}

/*
 * Pilot Line Edits
 */

void ExpNewFlightDialog::onPilotLineEdit_editingFinished()
{
    auto sender_object = sender();
    auto line_edit = this->findChild<QLineEdit*>(sender_object->objectName());
    DEB(line_edit->objectName() << "Editing Finished -" << line_edit->text());

    if(line_edit->text() == "self" || line_edit->text() == "SELF") {
        emit goodInputReceived(line_edit);
        return;
    }

    if (!line_edit->completer()->currentCompletion().isEmpty()) {
        DEB("Trying to fix input...");
        line_edit->setText(line_edit->completer()->currentCompletion());
    }
    if(pilotsIdMap.value(line_edit->text()) == 0) {
        emit badInputReceived(line_edit);
        // to do: prompt user to add new
    } else {
        emit goodInputReceived(line_edit);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                               Auto Logging Tab Slots                                        ///
///////////////////////////////////////////////////////////////////////////////////////////////////

void ExpNewFlightDialog::on_setAsDefaultButton_2_clicked()
{
    writeSettings();
}

void ExpNewFlightDialog::on_restoreDefaultButton_2_clicked()
{
    readSettings();
}

void ExpNewFlightDialog::on_PilotFlyingCheckBox_2_stateChanged(int)
{
    DEB("PF checkbox state changed.");
    if(ui->PilotFlyingCheckBox_2->isChecked()){
        ui->TakeoffSpinBox_2->setValue(1);
        ui->TakeoffCheckBox_2->setCheckState(Qt::Checked);
        ui->LandingSpinBox_2->setValue(1);
        ui->LandingCheckBox_2->setCheckState(Qt::Checked);

    }else if(!ui->PilotFlyingCheckBox_2->isChecked()){
        ui->TakeoffSpinBox_2->setValue(0);
        ui->TakeoffCheckBox_2->setCheckState(Qt::Unchecked);
        ui->LandingSpinBox_2->setValue(0);
        ui->LandingCheckBox_2->setCheckState(Qt::Unchecked);
    }
}

void ExpNewFlightDialog::on_IfrCheckBox_2_stateChanged(int)
{
    if (mandatoryLineEditsGood.count(true) == 7 && updateEnabled)
        emit mandatoryLineEditsFilled();
}

void ExpNewFlightDialog::on_manualEditingCheckBox_2_stateChanged(int arg1)
{
    if (!(mandatoryLineEditsGood.count(true) == 7) && ui->manualEditingCheckBox_2->isChecked()) {
        auto message_box = QMessageBox(this);
        message_box.setText("Before editing times manually, please fill out the required fields in the flight data tab,"
                            " so that total time can be calculated.");
        message_box.exec();
        ui->manualEditingCheckBox_2->setChecked(false);
        return;
    }
    QList<QLineEdit*>   LE = {ui->tSPSETimeLineEdit_2, ui->tSPMETimeLineEdit_2, ui->tMPTimeLineEdit_2,    ui->tIFRTimeLineEdit_2,
                              ui->tNIGHTTimeLineEdit_2,ui->tPICTimeLineEdit_2,  ui->tPICUSTimeLineEdit_2, ui->tSICTimeLineEdit_2,
                              ui->tDUALTimeLineEdit_2, ui->tFITimeLineEdit_2};
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

void ExpNewFlightDialog::on_ApproachComboBox_2_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ILS CAT III"){  //for a CAT III approach an Autoland is mandatory, so we can preselect it.
        ui->AutolandCheckBox_2->setCheckState(Qt::Checked);
        ui->AutolandSpinBox_2->setValue(1);
    }else{
        ui->AutolandCheckBox_2->setCheckState(Qt::Unchecked);
        ui->AutolandSpinBox_2->setValue(0);
    }
}
