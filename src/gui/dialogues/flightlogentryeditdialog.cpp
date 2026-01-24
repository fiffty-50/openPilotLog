#include "flightlogentryeditdialog.h"
#include "src/classes/date.h"
#include "src/classes/settings.h"
#include "src/classes/style.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/database/flightlogentry.h"
#include "src/gui/dialogues/airportentryeditdialog.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/gui/dialogues/tailentryeditdialog.h"
#include "src/gui/verification/completerprovider.h"
#include "src/opl.h"
#include <QCalendarWidget>
#include <QDateEdit>
#include <QTimeEdit>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qregularexpression.h>
#include <qspinbox.h>
#include <qtextformat.h>
#include <qvalidator.h>

FlightLogEntryEditDialog::FlightLogEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent), m_dateFormatString(Settings::getDateFormatString()),
      m_timeFormatString(Settings::getTimeFormatString())
{
    init();
    retranslateUi();
    setupSlots();
    readSettings();
}

void FlightLogEntryEditDialog::loadEntry(int event_row_id)
{
    // Try to load the flight data
    if (auto flightOpt = OPL::FlightData::getFlightData(event_row_id); flightOpt) {
        m_eventId = event_row_id;

        const auto &flight_data  = *flightOpt;
        const auto &log_entry    = *flight_data.logEntry();
        const auto &flight_entry = *flight_data.flightEntry();

        DEB << "Log Entry:" << log_entry;
        DEB << "Flight Entry:" << flight_entry;
        DEB << "Movements:" << *flight_data.movementEntries();

        // Populate UI fields
        dateEdit->setDate(log_entry.getDate());
        deptLineEdit->setText(
            DBCache->getMap(MapType::AirportCodesIcao).value(flight_entry.getDepartureId()));
        destLineEdit->setText(
            DBCache->getMap(MapType::AirportCodesIcao).value(flight_entry.getDestinationId()));
        timeOffEdit->setTime(QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOffBlocksMs()));
        timeOnEdit->setTime(QTime::fromMSecsSinceStartOfDay(flight_entry.getTimeOnBlocksMs()));
        registrationLineEdit->setText(
            DBCache->getMap(MapType::TailRegistrations).value(flight_entry.getTailId()));
        picLineEdit->setText(DBCache->getMap(MapType::PilotNames).value(flight_entry.getPicId()));
        sicLineEdit->setText(
            DBCache->getMap(MapType::PilotNames).value(flight_entry.getSecondPilotId()));
        flightNumberLineEdit->setText(flight_entry.getFlightNumber());
        remarksTextEdit->setPlainText(log_entry.getRemarks());

        // Movements
        takeOffCountSpinBox->setValue(flight_data.getTakeOffCount());
        landingCountSpinBox->setValue(flight_data.getLandingCount());

        // Segment Data
        {
            const QSignalBlocker b(pilotFlyingCheckBox);
            pilotFlyingCheckBox->setChecked(flight_data.isPilotFlying());
        }
        flightRulesComboBox->setCurrentIndex(flight_data.isIfr());
        pilotFunctionComboBox->setCurrentText(flight_data.pilotFunction());
    }
    else {
        // Flight not found
        WARN("Unable to load Flight with Log Event Id: " + QString::number(event_row_id));
    }
}

bool FlightLogEntryEditDialog::deleteEntry(int row_id) { return false; }

void FlightLogEntryEditDialog::init()
{
    // Main Layout
    // 5 columns with the middle column used as a spacer or in some cases for display Labels
    gridLayout         = new QGridLayout(this);
    int row            = 0;
    constexpr int col0 = 0;
    constexpr int col1 = 1;
    constexpr int col2 = 2; // middle separator
    constexpr int col3 = 3;
    constexpr int col4 = 4;

    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Left side (cols 0–1), optionally fills col 2
    auto addLeft = [&](QWidget *left, QWidget *right, QWidget *middle = nullptr) {
        gridLayout->addWidget(left, row, col0, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col1, singleSpan, singleSpan);

        if (middle)
            gridLayout->addWidget(middle, row, col2, singleSpan, singleSpan);
        else
            gridLayout->addWidget(new QLabel(this), row, col2, singleSpan, singleSpan);
    };

    // Right side (cols 3–4), advances row
    auto addRight = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, col3, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, col4, singleSpan, singleSpan);
        row++;
    };

    // Row
    // Left
    datePushButton   = new QPushButton(this);
    dateEdit         = new QDateEdit(this);
    dateDisplayLabel = new QLabel(this);
    addLeft(datePushButton, dateEdit, dateDisplayLabel);

    // Right
    registrationLabel    = new QLabel(this);
    registrationLineEdit = new QLineEdit(this);
    addRight(registrationLabel, registrationLineEdit);

    // Row
    // Left
    deptLabel        = new QLabel(this);
    deptLineEdit     = new QLineEdit(this);
    deptDisplayLabel = new QLabel(this);
    addLeft(deptLabel, deptLineEdit, deptDisplayLabel);

    // Right
    picLabel    = new QLabel(this);
    picLineEdit = new QLineEdit(this);
    addRight(picLabel, picLineEdit);

    // Row
    // Left
    destLabel        = new QLabel(this);
    destLineEdit     = new QLineEdit(this);
    destDisplayLabel = new QLabel(this);
    addLeft(destLabel, destLineEdit, destDisplayLabel);

    // Right
    sicLabel    = new QLabel(this);
    sicLineEdit = new QLineEdit(this);
    addRight(sicLabel, sicLineEdit);

    // Row
    // Left
    timeOffLabel = new QLabel(this);
    timeOffEdit  = new QTimeEdit(this);
    addLeft(timeOffLabel, timeOffEdit);

    // Right
    flightNumberLabel    = new QLabel(this);
    flightNumberLineEdit = new QLineEdit(this);
    addRight(flightNumberLabel, flightNumberLineEdit);

    // Row
    // Left
    timeOnLabel = new QLabel(this);
    timeOnEdit  = new QTimeEdit(this);
    addLeft(timeOnLabel, timeOnEdit);

    // Right
    pilotFlyingCheckBox = new QCheckBox(this);
    gridLayout->addWidget(pilotFlyingCheckBox, row, col3, singleSpan, spanRemaining);
    row++;

    // Row
    // Left
    pilotFunctionLabel    = new QLabel(this);
    pilotFunctionComboBox = new QComboBox(this);
    addLeft(pilotFunctionLabel, pilotFunctionComboBox);

    // Right
    takeOffCountLabel   = new QLabel(this);
    takeOffCountSpinBox = new QSpinBox(this);
    addRight(takeOffCountLabel, takeOffCountSpinBox);

    // Row
    // Left
    flightRulesLabel    = new QLabel(this);
    flightRulesComboBox = new QComboBox(this);
    addLeft(flightRulesLabel, flightRulesComboBox);

    // Right
    landingCountLabel   = new QLabel(this);
    landingCountSpinBox = new QSpinBox(this);
    addRight(landingCountLabel, landingCountSpinBox);

    // Row
    // Left
    remarksLabel    = new QLabel(this);
    remarksTextEdit = new QPlainTextEdit(this);
    remarksTextEdit->setMaximumHeight(registrationLineEdit->sizeHint().height() * 2);
    addLeft(remarksLabel, remarksTextEdit);

    // Right
    totalTimeLabel        = new QLabel(this);
    totalTimeDisplayLabel = new QLabel(this);
    addRight(totalTimeLabel, totalTimeDisplayLabel);

    // Row
    buttonBox = new QDialogButtonBox(this);
    // buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, row, col4, singleSpan, singleSpan);

    // Save some line edits for later use
    m_locationLineEdits.resize(2);
    m_locationLineEdits[0] = deptLineEdit;
    m_locationLineEdits[1] = destLineEdit;
    m_nameLineEdits.resize(2);
    m_nameLineEdits[0] = picLineEdit;
    m_nameLineEdits[1] = sicLineEdit;

    m_line_edit_table_map = {
        {deptLineEdit,         OPL::DbTable::v2Airports     },
        {destLineEdit,         OPL::DbTable::v2Airports     },
        {picLineEdit,          OPL::DbTable::v2Pilots       },
        {sicLineEdit,          OPL::DbTable::v2Pilots       },
        {registrationLineEdit, OPL::DbTable::v2AircraftTails},
    };

    // Create a map of validation functions for line edits that take user input
    m_line_edit_validators = {
        {deptLineEdit,
         [this](auto text) {
             return DBCache->getMultiMap(MapType::AirportCodesAll).contains(text);
         }                                                                                   },
        {destLineEdit,
         [this](auto text) {
             return DBCache->getMultiMap(MapType::AirportCodesAll).contains(text);
         }                                                                                   },
        {picLineEdit,
         [this](auto text) { return DBCache->getKeyMap(MapType::PilotNames).contains(text); }},
        {sicLineEdit,
         [this](auto text) { return DBCache->getKeyMap(MapType::PilotNames).contains(text); }},
        {registrationLineEdit, [this](auto text) {
             return DBCache->getKeyMap(MapType::TailRegistrations).contains(text);
         }                                                     }
    };

    setTabOrder({datePushButton, dateEdit, deptLineEdit, destLineEdit, timeOffEdit, timeOnEdit,
                 pilotFunctionComboBox, flightRulesComboBox, registrationLineEdit, picLineEdit,
                 sicLineEdit, flightNumberLineEdit, pilotFlyingCheckBox, takeOffCountSpinBox,
                 landingCountSpinBox, buttonBox});

    retranslateUi();
    setupValidationAndCompletion();
    setupSlots();
    readSettings();
    dateEdit->setFocus();
}

void FlightLogEntryEditDialog::retranslateUi()
{
    datePushButton->setText(tr("Date"));
    dateDisplayLabel->setText(dateEdit->date().toString(QStringLiteral("ddd d MMM, yyyy")));
    deptLabel->setText(tr("Departure"));
    destLabel->setText(tr("Destination"));
    timeOffLabel->setText(tr("Off Blocks"));
    timeOnLabel->setText(tr("On Blocks"));
    totalTimeLabel->setText(tr("Total"));
    pilotFunctionLabel->setText(tr("Function"));
    flightRulesLabel->setText(tr("Flight Rules"));
    registrationLabel->setText(tr("Registration"));
    picLabel->setText(tr("Pilot in Command"));
    sicLabel->setText(tr("Second Pilot"));
    flightNumberLabel->setText(tr("Flight Number"));
    pilotFlyingCheckBox->setText(tr("Pilot Flying"));
    takeOffCountLabel->setText(tr("Take Off"));
    landingCountLabel->setText(tr("Landing"));
    remarksLabel->setText(tr("Remarks"));
    totalTimeDisplayLabel->setText(QStringLiteral("00:00"));
}

void FlightLogEntryEditDialog::setupValidationAndCompletion()
{
    // Setup Widegts
    dateEdit->setDisplayFormat(m_dateFormatString);
    dateEdit->setCalendarPopup(true);
    dateEdit->setTimeZone(QTimeZone::UTC);
    dateEdit->setMinimumDate(OPL::Date::minimumDate());
    dateEdit->setMaximumDate(OPL::Date::maximumDate());
    dateEdit->setDate(QDate::currentDate());
    dateDisplayLabel->setMinimumWidth(200);
    dateDisplayLabel->setMaximumWidth(200);
    QFont f = dateDisplayLabel->font();
    f.setItalic(true);
    dateDisplayLabel->setFont(f);
    deptDisplayLabel->setFont(f);
    destDisplayLabel->setFont(f);

    takeOffCountSpinBox->setMinimum(0);
    landingCountSpinBox->setMinimum(0);
    timeOnEdit->setDisplayFormat(m_timeFormatString);
    timeOffEdit->setDisplayFormat(m_timeFormatString);

    OPL::GLOBALS->loadPilotFunctions(pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(flightRulesComboBox);

    // Setup autocompletion and Basic Input Validation
    for (const auto &line_edit : std::as_const(m_locationLineEdits)) {
        const auto val = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, line_edit);
        line_edit->setValidator(val);
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
    }

    for (const auto &line_edit : std::as_const(m_nameLineEdits)) {
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Pilots));
    }

    registrationLineEdit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Tails));
}

void FlightLogEntryEditDialog::setupSlots()
{
    // Button Box
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FlightLogEntryEditDialog::on_accepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // User input line edits that contain mandatory inputs need to be verified
    // Registration Line Edit
    connect(registrationLineEdit, &QLineEdit::editingFinished, this,
            [this]() { on_table_line_edit_editingFinished(registrationLineEdit); });
    // Pilot Name Line Edits
    connect(picLineEdit, &QLineEdit::editingFinished,
            [this]() { on_table_line_edit_editingFinished(picLineEdit); });
    connect(sicLineEdit, &QLineEdit::editingFinished,
            [this]() { on_table_line_edit_editingFinished(sicLineEdit); });
    // Location Line Edits - also change text toUpper
    for (const auto &line_edit : std::as_const(m_locationLineEdits)) {
        connect(line_edit, &QLineEdit::textChanged, this, [this, &line_edit]() {
            const QSignalBlocker b(line_edit);
            line_edit->setText(line_edit->text().toUpper());
        });
        connect(line_edit, &QLineEdit::editingFinished,
                [this, &line_edit]() { on_table_line_edit_editingFinished(line_edit); });
    }
    // Change text to upper case for acft line edit
    connect(registrationLineEdit, &QLineEdit::textChanged, this, [this]() {
        const QSignalBlocker b(registrationLineEdit);
        registrationLineEdit->setText(registrationLineEdit->text().toUpper());
    });

    // Calculate Block Time when time edit is changed
    connect(timeOffEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(timeOffEdit->time(), timeOnEdit->time()));
        totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });
    connect(timeOnEdit, &QTimeEdit::timeChanged, this, [this]() {
        const QTime blockTime = QTime::fromMSecsSinceStartOfDay(
            OPL::Time::blockTimeMs(timeOffEdit->time(), timeOnEdit->time()));
        totalTimeDisplayLabel->setText(blockTime.toString(QStringLiteral("hh:mm")));
    });

    // Add Take Off and Landing when Pilot Flying
    connect(pilotFlyingCheckBox, &QCheckBox::checkStateChanged, this,
            &FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged);
}

void FlightLogEntryEditDialog::readSettings()
{
    pilotFunctionComboBox->setCurrentIndex(static_cast<int>(Settings::getPilotFunction()));
    flightRulesComboBox->setCurrentIndex(Settings::getLogIfr());
    flightNumberLineEdit->setText(Settings::getFlightNumberPrefix());
}

// Data Collection and Submission

bool FlightLogEntryEditDialog::runSanityChecks()
{
    // make sure the pilot function and pilot names make sense
    int pic_id = DBCache->getKeyMap(MapType::PilotNames).value(picLineEdit->text());
    int sic_id = DBCache->getKeyMap(MapType::PilotNames).value(sicLineEdit->text());

    if (pic_id == sic_id) {
        INFO(tr("PIC and SIC names are the same."));
        return false;
    }

    QVariant v = pilotFunctionComboBox->currentData();
    if (!v.canConvert<OPL::PilotFunction>()) {
        INFO(tr("Invalid pilot function."));
        return false;
    }
    auto function = v.value<OPL::PilotFunction>();

    // if the logbook owner is entered as PIC, he must be the first pilot and vice versa
    bool ownerIsPic = (pic_id == OPL::LOGBOOK_OWNER_ID);
    bool functionIsPicCompatible =
        (function == OPL::PilotFunction::PIC || function == OPL::PilotFunction::FI);

    if (ownerIsPic != functionIsPicCompatible) {
        INFO(tr("PIC and Pilot Function are inconsistent."));
        return false;
    }
    return true;
}

bool FlightLogEntryEditDialog::userWantsToAddNewDatabaseElement(QLineEdit *caller)
{
    DEB << "Add new Database entry?";
    assert(m_line_edit_table_map.contains(caller));
    auto table = m_line_edit_table_map[caller];

    QMessageBox::StandardButton reply;
    switch (table) {
    case OPL::DbTable::v2Pilots:
        reply = QMessageBox::question(
            this, tr("No Pilot found"),
            tr("No pilot found.<br>Please enter the Name as"
               "<br><br><center><b>Lastname, Firstname</b></center><br><br>"
               "If this is the first time you log a flight with this pilot, "
               "you have to add the pilot to the database first."
               "<br><br>Would you like to add a new pilot to the database?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::v2AircraftTails:
        reply = QMessageBox::question(
            this, tr("No Aircraft found"),
            tr("No aircraft with this registration found.<br><br>"
               "If this is the first time you log a flight with this aircraft, "
               "you have to add the registration to the database first."
               "<br><br>Would you like to add a new aircraft to the database?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    case OPL::DbTable::v2Airports:
        reply = QMessageBox::question(
            this, tr("No Airport found"),
            tr("No Airport with this identifier found.<br><br>"
               "<b>Please verify the input. Airports can be entered with their ICAO or IATA "
               "code.<br><br></b>"
               "If this is the first time you log a flight to this airport, "
               "and it is not yet in the database, "
               "you have to add the airport to the database first."
               "<br><br>Would you like to add a new airport to the database?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton::Yes);
        break;
    default:
        reply = QMessageBox::No;
        break;
    }

    DEB << reply;

    return reply == QMessageBox::Yes;
}

bool FlightLogEntryEditDialog::addNewDatabaseElement(QLineEdit *caller)
{
    if (!m_line_edit_table_map.contains(caller)) {
        return false;
    }
    OPL::DbTable table = m_line_edit_table_map.value(caller);

    // Create a Factory for entry edit dialogues associated with the line edits
    static const QHash<OPL::DbTable, std::function<std::unique_ptr<EntryEditDialog>(QLineEdit *)>>
        entry_edit_dialog_factory = {
            {OPL::DbTable::v2Pilots,
             [](QLineEdit *le) {
                 return std::make_unique<PilotEntryEditDialog>(le->text(), le->parentWidget());
             }},
            {OPL::DbTable::v2AircraftTails,
             [](QLineEdit *le) {
                 return std::make_unique<TailEntryEditDialog>(le->text(), le->parentWidget());
             }},
            {OPL::DbTable::v2Airports,
             [](QLineEdit *le) {
                 return std::make_unique<AirportEntryEditDialog>(le->parentWidget());
             }},
    };

    if (!entry_edit_dialog_factory.contains(table)) return false;
    std::unique_ptr<EntryEditDialog> dlg(entry_edit_dialog_factory[table](caller));
    bool success = dlg->exec() == QDialog::Accepted;

    // Set the line edits text to the newly created entry
    QHash<OPL::DbTable, const OPL::IdMap *> id_map = {
        {OPL::DbTable::v2Pilots,        &DBCache->getMap(MapType::PilotNames)       },
        {OPL::DbTable::v2AircraftTails, &DBCache->getMap(MapType::TailRegistrations)},
        {OPL::DbTable::v2Airports,      &DBCache->getMap(MapType::AirportCodesIcao) },
    };
    int row_id      = dlg->getRowId();
    const auto &map = id_map[table];

    if (!map->contains(row_id)) return false;
    caller->setText(map->value(row_id));
    return success;
}

bool FlightLogEntryEditDialog::validateUserInput(QLineEdit *line_edit)
{

    bool contains = m_line_edit_validators[line_edit](line_edit->text());
    if (contains) {
        on_GoodInputReceived(line_edit);
    }
    else {
        on_badInputReceived(line_edit);
    }

    return contains;
}

// Slots

void FlightLogEntryEditDialog::on_accepted()
{
    DEB << "Dialog accepted";
    /*
    if (flight_data_opt) {
        const auto &data = *flight_data_opt;
        if (runSanityChecks()) {
            if (false) {
            //if (data.submit();) {
                QDialog::accept();
            }
        }
    }*/
}

void FlightLogEntryEditDialog::on_table_line_edit_editingFinished(QLineEdit *caller)
{
    if (caller->text().isEmpty()) {
        return;
    }

    if (validateUserInput(caller)) {
        DEB << "Valid input received: " << caller->text();
        return;
    }

    // Try to get a completion
    auto *completer    = caller->completer();
    QString completion = completer ? completer->currentCompletion() : QString();

    if (completion.isEmpty()) {
        DEB << "No completion available for: " << caller->text();
        if (!userWantsToAddNewDatabaseElement(caller)) {
            return;
        }
        if (!addNewDatabaseElement(caller)) {
            return;
        }
    }
    else {
        caller->setText(completion);
    }

    // re-validate
    validateUserInput(caller);
}

void FlightLogEntryEditDialog::on_pilotFlyingCheckBoxStateChanged(Qt::CheckState state)
{
    switch (state) {
    case Qt::Checked:
        takeOffCountSpinBox->setValue(1);
        landingCountSpinBox->setValue(1);
        break;
    case Qt::Unchecked:
        takeOffCountSpinBox->setValue(0);
        landingCountSpinBox->setValue(0);
        break;
    default:
        break;
    }
}
