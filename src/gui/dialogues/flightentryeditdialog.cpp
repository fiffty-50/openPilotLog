#include "flightentryeditdialog.h"
#include "src/classes/date.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/gui/dialogues/airportentryeditdialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/verification/airportinput.h"
#include "src/gui/verification/completerprovider.h"
#include "src/gui/verification/pilotinput.h"
#include "src/gui/verification/tailinput.h"
#include "src/gui/verification/timeinput.h"
#include <src/classes/settings.h>
#include <QGridLayout>
#include <QDialogButtonBox>

FlightEntryEditDialog::FlightEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent)
{
    init();
}

FlightEntryEditDialog::FlightEntryEditDialog(int rowId, QWidget *parent)
    : EntryEditDialog(parent)
{
    init();
}

void FlightEntryEditDialog::loadEntry(int rowID)
{
    const OPL::FlightEntry entry = DB->getFlightEntry(rowID);
    loadEntry(entry);
}

void FlightEntryEditDialog::loadEntry(OPL::Row entry)
{
    DEB << "Loading Flight Entry";
    DEB << entry;
    using namespace OPL;
    const auto &entryData = entry.getData();

    // Fill the entry data into the form
    // Date of Flight
    const QDate date = QDate::fromJulianDay(entryData.value(FlightEntry::DOFT).toInt());
    calendarWidget->setSelectedDate(date);
    dateLineEdit.setText(Date(date, displayFormat).toString());
    // Location
    departureLineEdit.setText(entryData.value(OPL::FlightEntry::DEPT).toString());
    destinationLineEdit.setText(entryData.value(OPL::FlightEntry::DEST).toString());
    // Times
    timeOutLineEdit.setText(Time(entryData.value(OPL::FlightEntry::TOFB).toInt(), displayFormat).toString());
    timeInLineEdit.setText(Time(entryData.value(OPL::FlightEntry::TONB).toInt(), displayFormat).toString());
    // Registration
    registrationLineEdit.setText(DBCache->getTailsMap().value(entryData.value(OPL::FlightEntry::ACFT).toInt()));
    // Pilot Names
    firstPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(OPL::FlightEntry::PIC).toInt()));
    secondPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(OPL::FlightEntry::SECONDPILOT).toInt()));
    thirdPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(OPL::FlightEntry::THIRDPILOT).toInt()));

    //Function
    // TODO - this is not very pretty think of a better way to do this
    for (int i = 0; i < 5; i++) { // QHash::iterator is not guarenteed to be in order
        if(entryData.value(pilotFuncionsMap.value(i)).toInt() != 0)
            pilotFunctionComboBox.setCurrentIndex(i);
    }

    // Approach
    approachTypeComboBox.setCurrentText(entryData.value(FlightEntry::APPROACHTYPE).toString());

    // Flight Rules - check if IFR time > 0
    flightRulesComboBox.setCurrentIndex(entryData.value(FlightEntry::TIFR).toInt() > 0 ? 1 : 0);

    // Take Off and Landing Count
    int takeOffCount = entryData.value(OPL::FlightEntry::TODAY).toInt()
                       + entryData.value(OPL::FlightEntry::TONIGHT).toInt();
    int landingCount = entryData.value(OPL::FlightEntry::LDGDAY).toInt()
                       + entryData.value(OPL::FlightEntry::LDGNIGHT).toInt();
    takeOffCountSpinBox.setValue(takeOffCount);
    landingCountSpinBox.setValue(landingCount);
    // Pilot Flying or Pilot Monitoring
    pilotFlyingCheckBox.setChecked(entryData.value(OPL::FlightEntry::PILOTFLYING).toBool());
    // Remarks and Flight Number
    remarksLineEdit.setText(entryData.value(OPL::FlightEntry::REMARKS).toString());
    flightNumberLineEdit.setText(entryData.value(OPL::FlightEntry::FLIGHTNUMBER).toString());
}

bool FlightEntryEditDialog::deleteEntry(int rowID)
{
    return DB->remove(DB->getFlightEntry(rowID));
}

void FlightEntryEditDialog::init()
{
    setupUI();
    setupAutoCompletion();
    // set up the pop-up date selection
    calendarWidget = new QCalendarWidget(this);
    calendarWidget->setVisible(false);
    calendarWidget->setWindowFlag(Qt::Dialog);

    // set up combo and spin boxes
    OPL::GLOBALS->loadApproachTypes(&approachTypeComboBox);
    OPL::GLOBALS->loadPilotFunctios(&pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(&flightRulesComboBox);
    takeOffCountSpinBox.setRange(0, 99);
    landingCountSpinBox.setRange(0, 99);

    setupSignalsAndSlots();
    readSettings();

    // use the dateDisplayLabel as a spacer
    dateDisplayLabel.setMinimumWidth(200); // TODO make dynamic
    // set the current date
    OPL::Date today = OPL::Date::today(displayFormat);
    dateLineEdit.setText(today.toString());
    // set cursor to entry point
    dateLineEdit.setFocus();
}

void FlightEntryEditDialog::setupUI()
{
    // TODO implement OOIB logging, hide for now
    timeOffLabel.hide();
    timeOffLineEdit.hide();
    timeOnLabel.hide();
    timeOnLineEdit.hide();

    // setup the basic layout
    // const int column0 = 0;
    // const int column1 = 1;
    // const int column2 = 2;
    // const int column3 = 3;
    // const int allColSpan = 4;
    QGridLayout *layout = new QGridLayout(this);
    const int lastCol = 4;
    int column = 0;
    int row = 0;

    registrationLineEdit.setObjectName(QStringLiteral("registrationLineEdit"));
    layout->addWidget(&dateButton, row, column++);
    layout->addWidget(&dateLineEdit, row, column++);
    layout->addWidget(&dateDisplayLabel, row, column++);
    layout->addWidget(&registrationLabel, row, column++);
    layout->addWidget(&registrationLineEdit, row, column);

    column = 0;
    firstPilotLineEdit.setObjectName(QStringLiteral("firstPilotLineEdit"));
    layout->addWidget(&departureLabel, ++row, column++);
    layout->addWidget(&departureLineEdit, row, column++);
    layout->addWidget(&departureDisplayLabel, row, column++);
    layout->addWidget(&firstPilotLabel, row, column++);
    layout->addWidget(&firstPilotLineEdit, row, column++);

    column = 0;
    secondPilotLineEdit.setObjectName(QStringLiteral("secondPilotLineEdit"));
    layout->addWidget(&destinationLabel, ++row, column++);
    layout->addWidget(&destinationLineEdit, row, column++);
    layout->addWidget(&destinationDisplayLabel, row, column++);
    layout->addWidget(&secondPilotLabel, row, column++);
    layout->addWidget(&secondPilotLineEdit, row, column++);

    column = 0;
    thirdPilotLineEdit.setObjectName(QStringLiteral("thirdPilotLineEdit"));
    timeOffLineEdit.setObjectName(QStringLiteral("timeOutLineEdit"));
    layout->addWidget(&timeOutLabel, ++row, column++);
    layout->addWidget(&timeOutLineEdit, row, column++);
    column++;
    layout->addWidget(&thirdPilotLabel, row, column++);
    layout->addWidget(&thirdPilotLineEdit, row, column++);

    column = 0;
    timeInLineEdit.setObjectName(QStringLiteral("timeInLineEdit"));
    layout->addWidget(&timeInLabel, ++row, column++);
    layout->addWidget(&timeInLineEdit, row, column++);
    column++;
    layout->addWidget(&flightNumberLabel, row, column++);
    layout->addWidget(&flightNumberLineEdit, row, column++);

    column = 0;
    layout->addWidget(&totalTimeLabel, ++row, column++);
    layout->addWidget(&totalTimeDisplayLabel, row, column++);
    column++;
    layout->addWidget(&remarksLabel, row, column++);
    layout->addWidget(&remarksLineEdit, row, column++);


    column = 0;
    layout->addWidget(&pilotFunctionLabel, ++row, column++);
    layout->addWidget(&pilotFunctionComboBox, row, column++);
    column++;
    layout->addWidget(&pilotFlyingLabel, row, column++);
    layout->addWidget(&pilotFlyingCheckBox, row, column++);


    column = 0;
    layout->addWidget(&approachTypeLabel, ++row, column++);
    layout->addWidget(&approachTypeComboBox, row, column++);
    column++;
    layout->addWidget(&takeOffCountLabel, row, column++);
    layout->addWidget(&takeOffCountSpinBox, row, column++);

    column = 0;
    layout->addWidget(&flightRulesLabel, ++row, column++);
    layout->addWidget(&flightRulesComboBox, row, column++);
    column++;
    layout->addWidget(&landingCountLabel, row, column++);
    layout->addWidget(&landingCountSpinBox, row, column++);

    layout->addWidget(&acceptButtonBox, ++row, lastCol);

    // set order for tabbing through the line edits
    setTabOrder({
        &dateButton,
        &dateLineEdit,
        &departureLineEdit,
        &destinationLineEdit,
        &timeOutLineEdit,
        &timeOffLineEdit,
        &timeOnLineEdit,
        &timeInLineEdit,
        &pilotFunctionComboBox,
        &approachTypeComboBox,
        &flightRulesComboBox,
        &registrationLineEdit,
        &firstPilotLineEdit,
        &secondPilotLineEdit,
        &thirdPilotLineEdit,
        &flightNumberLineEdit,
        &remarksLineEdit,
        &pilotFlyingCheckBox,
        &takeOffCountSpinBox,
        &landingCountSpinBox,
        &acceptButtonBox
                });

    // hide the total time label, will be shown once times are entered
    totalTimeDisplayLabel.setEnabled(false);
}

void FlightEntryEditDialog::setupAutoCompletion()
{
    // Time Line Edits
    for (const auto& line_edit : timeLineEdits) {
        const auto validator = new QRegularExpressionValidator(OPL::RegEx::RX_TIME_ENTRY, line_edit);
        line_edit->setValidator(validator);
    }
    // Location Line Edits
    for (const auto& line_edit : locationLineEdits) {
        const auto validator = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, line_edit);
        line_edit->setValidator(validator);
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
    }
    // Name Line Edits
    for (const auto& line_edit : pilotNameLineEdits) {
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Pilots));
    }
    // Acft Line Edit
    registrationLineEdit.setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Tails));
}

void FlightEntryEditDialog::setupSignalsAndSlots()
{
    // acceptButtonBox - standard discard, for accept redirect to run some checks first
    QObject::connect(&acceptButtonBox, &QDialogButtonBox::accepted,
                     this, &FlightEntryEditDialog::onDialogAccepted);
    QObject::connect(&acceptButtonBox, &QDialogButtonBox::rejected,
                     this, &FlightEntryEditDialog::reject);

    // calendarWidget - enable single-click selection
    QObject::connect(&dateButton, &QPushButton::clicked,
                     this, &FlightEntryEditDialog::onCalendarRequested);
    QObject::connect(calendarWidget, &QCalendarWidget::selectionChanged,
                     this, &FlightEntryEditDialog::onCalendarDateSelected);
    QObject::connect(calendarWidget, &QCalendarWidget::clicked,
                     this, &FlightEntryEditDialog::onCalendarDateSelected);
    // Date Line Edit
    QObject::connect(&dateLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onDateEditingFinished);
    // Registration Line Edit
    QObject::connect(&registrationLineEdit, &QLineEdit::editingFinished,
                     this, [this]() {
        FlightEntryEditDialog::onRegistrationEditingFinished(&registrationLineEdit);
    });

    /* Slots that are called by multiple line edits forward the caller object */
    // Time line edits
    for(const auto& lineEdit : timeLineEdits) {
        QObject::connect(lineEdit, &QLineEdit::editingFinished, this, [this, &lineEdit](){
            onTimeEditingFinished(lineEdit);
        });
    }

    // Location line edits
    for(const auto& lineEdit : locationLineEdits) {
        QObject::connect(lineEdit, &QLineEdit::editingFinished, this, [this, &lineEdit](){
            onLocationEditingFinished(lineEdit);
        });
    }

    // Pilot Name line edits
    for(const auto& lineEdit : pilotNameLineEdits) {
        QObject::connect(lineEdit, &QLineEdit::editingFinished, this, [this, &lineEdit](){
            onNameEditingFinished(lineEdit);
        });
    }
    // Change text to upper case for location and acft line edits
    QObject::connect(&registrationLineEdit, &QLineEdit::textChanged,
                     this, [this](){
        const QSignalBlocker b(registrationLineEdit);
        registrationLineEdit.setText(registrationLineEdit.text().toUpper());
    });

    for(const auto& lineEdit : locationLineEdits) {
        QObject::connect(lineEdit, &QLineEdit::textChanged,
                         this, [this, &lineEdit]() {
            const QSignalBlocker b(lineEdit);
            lineEdit->setText(lineEdit->text().toUpper());
        });
    }

    // Install Event Filter for focus in events on mandatory line edits
    for(auto it = mandatoryLineEdits.constBegin(); it != mandatoryLineEdits.constEnd(); ++it) {
        it.key()->installEventFilter(this);
    }
}

void FlightEntryEditDialog::readSettings()
{
    displayFormat = Settings::getDisplayFormat();

    pilotFunctionComboBox.setCurrentIndex(static_cast<int>(Settings::getPilotFunction()));
    approachTypeComboBox.setCurrentText(Settings::getApproachType());
    flightRulesComboBox.setCurrentIndex(Settings::getLogIfr());
    flightNumberLineEdit.setText(Settings::getFlightNumberPrefix());
    displayFormat = Settings::getDisplayFormat();
}

bool FlightEntryEditDialog::verifyUserInput(QLineEdit *lineEdit, const UserInput &input)
{
    DEB << "Verifying user input: " << lineEdit->text();

    if(!input.isValid()) {
        QString fixed = input.fixup();
        if(fixed == QString()) {
            return false;
        } else {
            lineEdit->setText(fixed);
            return true;
        }
    }
    return true;
}

/*
 * Slots
 */

void FlightEntryEditDialog::onBadInputReceived(QLineEdit *lineEdit) {
    LOG << "Bad Input Received: " + lineEdit->text();
    lineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    if(mandatoryLineEdits.contains(lineEdit)) {
        validationState.invalidate(mandatoryLineEdits.value(lineEdit));
    }
}

void FlightEntryEditDialog::onGoodInputReceived(QLineEdit *lineEdit) {
    LOG << "Good Input Received: " + lineEdit->text();
    lineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    lineEdit->setStyleSheet(QString());
    if(mandatoryLineEdits.contains(lineEdit)) {
        validationState.validate(mandatoryLineEdits.value(lineEdit));
    }
    if (validationState.timesValid()) {
        const OPL::Time tofb = OPL::Time::fromString(timeOutLineEdit.text(), displayFormat);
        const OPL::Time tonb = OPL::Time::fromString(timeInLineEdit.text(), displayFormat);
        const OPL::Time tblk = OPL::Time::blockTime(tofb, tonb);
        totalTimeDisplayLabel.setText(tblk.toString());
        LOG << "Block Time calculated: " + tblk.toString();
    }
    validationState.printValidationStatus();
}

void FlightEntryEditDialog::onDialogAccepted()
{
    LOG << "Dialog accepted";
    accept();
}

void FlightEntryEditDialog::onCalendarRequested()
{
    calendarWidget->setVisible(true);
}

void FlightEntryEditDialog::onCalendarDateSelected()
{
    calendarWidget->setVisible(false);
    dateLineEdit.setText(OPL::Date(calendarWidget->selectedDate(), dateTimeFormat).toString());
}

void FlightEntryEditDialog::onDateEditingFinished()
{
    DEB << "Date editing finished";
    OPL::Date date(dateLineEdit.text(), dateTimeFormat);

    if(date.isValid()) {
        onGoodInputReceived(&dateLineEdit);
        DEB << "Date is valid: " + date.toString();
        return;
    }
    DEB << "Date is invalid: " + dateLineEdit.text();
    onBadInputReceived(&dateLineEdit);
}

void FlightEntryEditDialog::onTimeEditingFinished(QLineEdit *lineEdit)
{
    DEB << "Time Editing finished: " + lineEdit->text();

    if(verifyUserInput(lineEdit, TimeInput(lineEdit->text(), dateTimeFormat))) {
        onGoodInputReceived(lineEdit);
        return;
    }

    onBadInputReceived(lineEdit);
}

void FlightEntryEditDialog::onNameEditingFinished(QLineEdit *lineEdit)
{
    // try to match user input with map
    if(verifyUserInput(lineEdit, PilotInput(lineEdit->text()))) {
        onGoodInputReceived(lineEdit);
        return;
    }

    // - on negative match, offer to add new pilot entry
    const auto table = OPL::DbTable::Pilots;
    if(addNewDatabaseElement(lineEdit, table)) {
        emit lineEdit->editingFinished(); //re-trigger verification
        return;
    }

    // entry was not recognized and no new entry has been added.
    onBadInputReceived(lineEdit);
}

void FlightEntryEditDialog::onRegistrationEditingFinished(QLineEdit *lineEdit)
{
    // try to match user input with map
    if(verifyUserInput(lineEdit, TailInput(lineEdit->text()))) {
        DEB << "Good input received - " + lineEdit->text();
        // The QCompleter contains both dashed as well as non-dashed registrations. Strip the second one
        {
            QSignalBlocker b(lineEdit);
            lineEdit->setText(lineEdit->text().split(' ')[0]);
        }
        onGoodInputReceived(lineEdit);
        return;
    }
    // if no match was found, offer to add new aircraft entry
    const auto table = OPL::DbTable::Tails;
    if(addNewDatabaseElement(lineEdit, table)) {
        emit lineEdit->editingFinished();
        return;
    }

    // entry was not recognized and no new entry has been added
    onBadInputReceived(lineEdit);
}

void FlightEntryEditDialog::onLocationEditingFinished(QLineEdit *lineEdit)
{
    QLabel *nameLabel;
    if(lineEdit == locationLineEdits.first()) {
        nameLabel = &departureDisplayLabel;
    } else {
        nameLabel = &destinationDisplayLabel;
    }

    if(verifyUserInput(lineEdit, AirportInput(lineEdit->text())) ) {
        // Match ICAO code with Airport Name and display on label
        nameLabel->setText(DBCache->getAirportsMapNames().value(
            DBCache->getAirportsMapICAO().key(
                lineEdit->text())));
        onGoodInputReceived(lineEdit);
        return;
    } else {
        nameLabel->setText("Unknown Airport");
        if(addNewDatabaseElement(lineEdit, OPL::DbTable::Airports)) {
            emit lineEdit->editingFinished(); //re-trigger verification
            return;
        }
    }

    // entry was not recognized and no new entry has been added
    onBadInputReceived(lineEdit);
}

bool FlightEntryEditDialog::addNewDatabaseElement(QLineEdit *caller, const OPL::DbTable table)
{
    // Ask the user if they want to add a new DB Element
    QMessageBox::StandardButton reply;
    switch (table) {
    case OPL::DbTable::Pilots:
        reply = QMessageBox::question(this, tr("No Pilot found"),
                                      tr("No pilot found.<br>Please enter the Name as"
                                         "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
                                         "If this is the first time you log a flight with this pilot, "
                                         "you have to add the pilot to the database first."
                                         "<br><br>Would you like to add a new pilot to the database?"),
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::Tails:
        reply = QMessageBox::question(this, tr("No Aircraft found"),
                                      tr("No aircraft with this registration found.<br>"
                                         "If this is the first time you log a flight with this aircraft, "
                                         "you have to add the registration to the database first."
                                         "<br><br>Would you like to add a new aircraft to the database?"),
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::Airports:
        reply = QMessageBox::question(this, tr("No Airport found"),
                                      tr("No Airport with this identifier found.<br>"
                                         "If this is the first time you log a flight to this airport, "
                                         "you have to add the airport to the database first."
                                         "<br><br>Would you like to add a new airport to the database?"),
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    default:
        reply = QMessageBox::No;
        break;
    }

    if(reply != QMessageBox::Yes) {
        return false;
    }

    // Create a New Dialog to add the new Element
    QDialog *newEntryDialog = nullptr;
    switch (table) {
    case OPL::DbTable::Pilots:
        newEntryDialog = new NewPilotDialog(caller->text(), this);
        break;
    case OPL::DbTable::Tails:
        newEntryDialog = new NewTailDialog(registrationLineEdit.text(), this);
        break;
    case OPL::DbTable::Airports:
        newEntryDialog = new NewAirportDialog(this);
        break;
    default:
        return false;
        break;
    }

    bool success = newEntryDialog->exec() == QDialog::Accepted;
    if(!success)
        return false;

    // set the line edit to the newly created entry
    const int lastRowId = DB->getLastEntry(table);
    switch(table) {
    case OPL::DbTable::Pilots:
        caller->setText(DBCache->getPilotNamesMap().value(lastRowId));
        break;
    case OPL::DbTable::Tails:
        caller->setText(DBCache->getTailsMap().value(lastRowId));
        break;
    case OPL::DbTable::Airports:
        caller->setText(DBCache->getAirportsMapICAO().value(lastRowId));
        break;
    default:
        break;
    }

    return true;
}

