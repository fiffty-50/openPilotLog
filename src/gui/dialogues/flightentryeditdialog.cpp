#include "flightentryeditdialog.h"
#include "src/classes/date.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/functions/calc.h"
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

    FlightEntryEditDialog::loadEntry(rowId);
}

void FlightEntryEditDialog::loadEntry(int rowID)
{
    m_flightEntry = DB->getFlightEntry(rowID);
    FlightEntryEditDialog::loadEntry(m_flightEntry);
}

void FlightEntryEditDialog::loadEntry(OPL::Row entry)
{
    DEB << "Loading Flight Entry";
    DEB << entry;
    using namespace OPL;

    // copy the entry data into the member Flight Entry
    m_flightEntry.setData(entry.getData());
    m_flightEntry.setRowId(entry.getRowId());
    m_flightEntry.forceValid(); // since this value is read from the database, its fields are valid
    const auto &entryData = m_flightEntry.getData();

    // Fill the entry data into the form
    // Date of Flight
    const QDate date = QDate::fromJulianDay(entryData.value(FlightEntry::DOFT).toInt());
    calendarWidget->setSelectedDate(date);
    dateLineEdit.setText(Date(date, m_displayFormat).toString());
    // Location
    departureLineEdit.setText(entryData.value(FlightEntry::DEPT).toString());
    destinationLineEdit.setText(entryData.value(FlightEntry::DEST).toString());
    // Times
    timeOutLineEdit.setText(Time(entryData.value(FlightEntry::TOFB).toInt(), m_displayFormat).toString());
    timeInLineEdit.setText(Time(entryData.value(FlightEntry::TONB).toInt(), m_displayFormat).toString());
    totalTimeDisplayLabel.setText(Time(m_flightEntry.getBlockTime(), m_displayFormat).toString());
    // Registration
    registrationLineEdit.setText(DBCache->getTailsMap().value(entryData.value(FlightEntry::ACFT).toInt()));
    // Pilot Names
    firstPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(FlightEntry::PIC).toInt()));
    secondPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(FlightEntry::SECONDPILOT).toInt()));
    thirdPilotLineEdit.setText(DBCache->getPilotNamesMap().value(entryData.value(FlightEntry::THIRDPILOT).toInt()));

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
    int takeOffCount = entryData.value(FlightEntry::TODAY).toInt()
                       + entryData.value(FlightEntry::TONIGHT).toInt();
    int landingCount = entryData.value(FlightEntry::LDGDAY).toInt()
                       + entryData.value(FlightEntry::LDGNIGHT).toInt();
    takeOffCountSpinBox.setValue(takeOffCount);
    landingCountSpinBox.setValue(landingCount);
    // Pilot Flying or Pilot Monitoring
    pilotFlyingCheckBox.setChecked(entryData.value(FlightEntry::PILOTFLYING).toBool());
    // Remarks and Flight Number
    remarksLineEdit.setText(entryData.value(FlightEntry::REMARKS).toString());
    flightNumberLineEdit.setText(entryData.value(FlightEntry::FLIGHTNUMBER).toString());
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
    OPL::Date today = OPL::Date::today(m_displayFormat);
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
    // Time Line Edits
    QObject::connect(&timeOutLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onTimeOutEditingFinished);
    QObject::connect(&timeInLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onTimeInEditingFinished);

    // Secondary Line Edits
    QObject::connect(&remarksLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onRemarksEditingFinished);
    QObject::connect(&flightNumberLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onFlightNumberEditingFinished);

    /* Slots that are called by multiple line edits forward the caller object */
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
    // Add Take Off and Landing when Pilot Flying
    QObject::connect(&pilotFlyingCheckBox, &QCheckBox::checkStateChanged,
                     this, &FlightEntryEditDialog::onPilotFlyingCheckboxStateChanged);
}

void FlightEntryEditDialog::readSettings()
{
    m_displayFormat = Settings::getDisplayFormat();
    pilotFunctionComboBox.setCurrentIndex(static_cast<int>(Settings::getPilotFunction()));
    approachTypeComboBox.setCurrentText(Settings::getApproachType());
    flightRulesComboBox.setCurrentIndex(Settings::getLogIfr());
    flightNumberLineEdit.setText(Settings::getFlightNumberPrefix());
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
}

void FlightEntryEditDialog::onGoodInputReceived(QLineEdit *lineEdit) {
    LOG << "Good Input Received: " + lineEdit->text();
    lineEdit->setStyleSheet(QString());

    // update block time label
    int blockMinutes = m_flightEntry.getBlockTime();
    if(blockMinutes > 0) {
        totalTimeDisplayLabel.setText(OPL::Time(blockMinutes, m_displayFormat).toString());
    }
}

void FlightEntryEditDialog::updateAirportLabels()
{
    departureDisplayLabel.setText(DBCache->getAirportsMapNames().value(
        DBCache->getAirportsMapICAO().key(
            departureLineEdit.text())));

    destinationDisplayLabel.setText(DBCache->getAirportsMapNames().value(
        DBCache->getAirportsMapICAO().key(
            destinationLineEdit.text())));
}

void FlightEntryEditDialog::collectSecondaryFlightData()
{
    bool success = true;
    // Calculate Night Time and Set Take Off and Landing Count
    const int blockMinutes = m_flightEntry.getBlockTime();
    if(Settings::getNightLoggingEnabled()) {
        if(! m_flightEntry.getValidationState().nightDataValid()) {
            LOG << "Night Data Invalid - skipping night time calculation";
            success = false;
        } else {
            // determine departure date/time
            const QStringView departureDate = m_flightEntry.getDate();
            const int departureTime = m_flightEntry.getTimeOffBlocks();
            const QStringView departureTimeHHMM = OPL::Time(departureTime, OPL::DateTimeFormat()).toString();
            const QDateTime departureDateTime = QDateTime::fromString(departureDate, departureTimeHHMM);
            // get departure and destination location
            const QString dept = m_flightEntry.getDeparture();
            const QString dest = m_flightEntry.getDestination();

            // Calculate Night Time and save the result
            const auto nightTimeData = OPL::Calc::NightTimeValues(dept, dest,departureDateTime, blockMinutes, Settings::getNightAngle());
            m_flightEntry.setNightTime(nightTimeData.nightMinutes);
            m_flightEntry.setTakeOffCount(takeOffCountSpinBox.value(), !nightTimeData.takeOffNight);
            m_flightEntry.setLandingCount(landingCountSpinBox.value(), !nightTimeData.landingNight);
        }
    } else {
        m_flightEntry.setTakeOffCount(takeOffCountSpinBox.value(), true);
        m_flightEntry.setLandingCount(landingCountSpinBox.value(), true);
    }

    // SPSE/SPME/ME/MP
    m_flightEntry.setAircraftCategoryTimes();

    // IFR time
    flightRulesComboBox.currentIndex() == 0 ? m_flightEntry.setIfrTime(0) : m_flightEntry.setIfrTime(blockMinutes);

    // PilotFunction time
    auto function = static_cast<OPL::FlightEntry::PilotFunction>(pilotFunctionComboBox.currentIndex());
    success &= m_flightEntry.setFunctionTimes(function);

    // Pilot Flying / Pilot Monitoring
    m_flightEntry.setIsPilotFlying(pilotFlyingCheckBox.isChecked());

    // Additional data
    m_flightEntry.setFlightNumber(flightNumberLineEdit.text());
    m_flightEntry.setRemarks(remarksLineEdit.text());
    m_flightEntry.setApproachType(approachTypeComboBox.currentText());

    if(!success)
        LOG << "Error ocurred when collecting secondary data.";
}

void FlightEntryEditDialog::onDialogAccepted()
{
    LOG << "Dialog accepted";
    // check if mandatory inputs are correctly filled
    if(!m_flightEntry.getValidationState().allValid()) {
        DEB << "Mandatory entries missing";
        // Collect information about missing items and inform user
        const QStringList missingItems = m_flightEntry.getValidationState().invalidItemDisplayNames();
        if(missingItems.isEmpty()) {
            WARN(tr("Mandatory Entries invalid."));
            return;
        }
        QString missingItemsString;
        for(const auto &item : missingItems) {
            missingItemsString.append(item);
            missingItemsString.append(QStringLiteral("<br>"));
        }
        INFO(tr("Not all mandatory entries are valid.<br>"
                "The following item(s) are empty or invalid:"
                "<br><br><center><b>%1</b></center><br>"
                "Please go back and fill in the required data."
                ).arg(missingItemsString));
    }

    collectSecondaryFlightData();
    DEB << m_flightEntry;
    if (!DB->commit(m_flightEntry)) {
        WARN(tr("The following error has ocurred:"
                "<br><br>%1<br><br>"
                "The entry has not been saved."
                ).arg(DB->lastError.text()));
        return;
    } else {
        QDialog::accept();
    }
}

void FlightEntryEditDialog::onCalendarRequested()
{
    calendarWidget->setVisible(true);
}

void FlightEntryEditDialog::onCalendarDateSelected()
{
    calendarWidget->setVisible(false);
    dateLineEdit.setText(OPL::Date(calendarWidget->selectedDate(), m_displayFormat).toString());
}

void FlightEntryEditDialog::onPilotFlyingCheckboxStateChanged(int index)
{
    if (index == Qt::CheckState::Checked) {
        takeOffCountSpinBox.setValue(1);
        landingCountSpinBox.setValue(1);
    } else {
        takeOffCountSpinBox.setValue(0);
        landingCountSpinBox.setValue(0);
    }
}

void FlightEntryEditDialog::onDateEditingFinished()
{
    LOG << "Date editing finished: " << dateLineEdit.text();
    if(!m_flightEntry.setDate(dateLineEdit.text(), m_displayFormat)) {
        onBadInputReceived(&dateLineEdit);
    } else {
        onGoodInputReceived(&dateLineEdit);
    }
}

void FlightEntryEditDialog::onTimeOutEditingFinished()
{
    verifyUserInput(&timeOutLineEdit, TimeInput(timeOutLineEdit.text(), m_displayFormat));
    if(! m_flightEntry.setTimeOffBlocks(timeOutLineEdit.text(), m_displayFormat))
        onBadInputReceived(&timeOutLineEdit);
    else
        onGoodInputReceived(&timeOutLineEdit);
}

void FlightEntryEditDialog::onTimeInEditingFinished()
{
    verifyUserInput(&timeInLineEdit, TimeInput(timeInLineEdit.text(), m_displayFormat));
    if(! m_flightEntry.setTimeOnBlocks(timeInLineEdit.text(), m_displayFormat))
        onBadInputReceived(&timeInLineEdit);
    else
        onGoodInputReceived(&timeInLineEdit);
}

void FlightEntryEditDialog::onNameEditingFinished(QLineEdit *lineEdit)
{
    // try to match user input with map
    if(verifyUserInput(lineEdit, PilotInput(lineEdit->text()))) {
        bool success = false;
        int index = pilotNameLineEdits.indexOf(lineEdit);
        switch (index) {
        case 0:
            success = m_flightEntry.setFirstPilot(lineEdit->text());
            break;
        case 1:
            success = m_flightEntry.setSecondPilot(lineEdit->text());
            break;
        case 2:
            success = m_flightEntry.setThirdPilot(lineEdit->text());
            break;
        default:
            break;
        }
        success ? onGoodInputReceived(lineEdit) : onBadInputReceived(lineEdit);
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
            const QString cleanRegistration = lineEdit->text().split(' ')[0];
            lineEdit->setText(cleanRegistration);
                if(!m_flightEntry.setRegistration(cleanRegistration)) {
                onBadInputReceived(lineEdit);
                return;
            }
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
    if(verifyUserInput(lineEdit, AirportInput(lineEdit->text())) ) {
        updateAirportLabels();
        lineEdit == locationLineEdits.first() ?
            m_flightEntry.setDeparture(lineEdit->text()) :
            m_flightEntry.setDestination(lineEdit->text());
        onGoodInputReceived(lineEdit);
        return;
    } else {
        if(addNewDatabaseElement(lineEdit, OPL::DbTable::Airports)) {
            emit lineEdit->editingFinished(); //re-trigger verification
            return;
        }
    }

    // entry was not recognized and no new entry has been added
    onBadInputReceived(lineEdit);
}

void FlightEntryEditDialog::onRemarksEditingFinished()
{
    m_flightEntry.setRemarks(remarksLineEdit.text());
}

void FlightEntryEditDialog::onFlightNumberEditingFinished()
{
    m_flightEntry.setFlightNumber(flightNumberLineEdit.text());
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

    // check if the user added a new entry or the dialog was canceled
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

