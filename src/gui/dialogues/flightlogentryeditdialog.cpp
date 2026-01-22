#include "flightlogentryeditdialog.h"
#include "src/classes/date.h"
#include "src/classes/settings.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/database/flightlogentry.h"
#include "src/gui/verification/completerprovider.h"
#include "src/opl.h"
#include <QCalendarWidget>
#include <QDateEdit>
#include <QTimeEdit>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qregularexpression.h>
#include <qspinbox.h>
#include <qtextformat.h>
#include <qvalidator.h>

FlightLogEntryEditDialog::FlightLogEntryEditDialog(QWidget *parent) : EntryEditDialog(parent)
{
    init();
    retranslateUi();
    setupSlots();
}

void FlightLogEntryEditDialog::loadEntry(int event_row_id)
{
    const auto flight_data = DB->getFlightData(event_row_id);
    const auto logEntry    = flight_data.logEntry();
    const auto flightEntry = flight_data.flightEntry();

    DEB << "Log Entry:" << *logEntry;
    DEB << "Flight Entry: " << *flightEntry;
    DEB << "Movements: " << *flight_data.movementEntries();

    m_eventId = event_row_id;

    dateEdit->setDate(logEntry->getDate());
    deptLineEdit->setText(DBCache->getMap(OPL::DatabaseCache::MapType::AirportCodesIcao).value(flightEntry->getDepartureId()));
    destLineEdit->setText(DBCache->getMap(OPL::DatabaseCache::MapType::AirportCodesIcao).value(flightEntry->getDestinationId()));
    timeOffEdit->setTime(QTime::fromMSecsSinceStartOfDay(flightEntry->getTimeOffBlocksMs()));
    timeOnEdit->setTime(QTime::fromMSecsSinceStartOfDay(flightEntry->getTimeOnBlocksMs()));
    registrationLineEdit->setText(DBCache->getMap(OPL::DatabaseCache::MapType::TailRegistrations).value(flightEntry->getTailId()));
    picLineEdit->setText(DBCache->getMap(OPL::DatabaseCache::MapType::PilotNames).value(flightEntry->getPicId()));
    sicLineEdit->setText(DBCache->getMap(OPL::DatabaseCache::MapType::PilotNames).value(flightEntry->getSecondPilotId()));
    flightNumberLineEdit->setText(flightEntry->getFlightNumber());
    remarksTextEdit->setPlainText(logEntry->getRemarks());

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
    dateEdit->setDisplayFormat(Settings::getDateFormatString());
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
    timeOnEdit->setDisplayFormat(Settings::getTimeFormatString());
    timeOffEdit->setDisplayFormat(Settings::getTimeFormatString());

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

    // Registration Line Edit
    connect(registrationLineEdit, &QLineEdit::editingFinished, this,
            &FlightLogEntryEditDialog::on_registrationLineEdit_editingFinished);

    // Pilot Name Line Edits
    connect(picLineEdit, &QLineEdit::editingFinished, this,
            [this]() { on_pilotNameLineEdit_editingFinished(picLineEdit); });
    connect(sicLineEdit, &QLineEdit::editingFinished, this,
            [this]() { on_pilotNameLineEdit_editingFinished(sicLineEdit); });

    // Location Line Edits
    connect(deptLineEdit, &QLineEdit::editingFinished, this,
            [this]() { on_locationLineEdit_editingFinished(deptLineEdit, deptDisplayLabel); });
    connect(destLineEdit, &QLineEdit::editingFinished, this,
            [this]() { on_locationLineEdit_editingFinished(destLineEdit, destDisplayLabel); });

    // Change text to upper case for location and acft line edits
    connect(registrationLineEdit, &QLineEdit::textChanged, this, [this]() {
        const QSignalBlocker b(registrationLineEdit);
        registrationLineEdit->setText(registrationLineEdit->text().toUpper());
    });
    for (const auto &lineEdit : std::as_const(m_locationLineEdits)) {
        QObject::connect(lineEdit, &QLineEdit::textChanged, this, [this, &lineEdit]() {
            const QSignalBlocker b(lineEdit);
            lineEdit->setText(lineEdit->text().toUpper());
        });
    }
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

// Slots
void FlightLogEntryEditDialog::on_accepted()
{
    DEB << "Dialog accepted";
    QDialog::accept();
}

void FlightLogEntryEditDialog::on_locationLineEdit_editingFinished(QLineEdit *caller,
                                                                   QLabel *displayLabel)
{

    const auto &text = caller->text();
    const auto &airportCodesMap =
        DBCache->getMultiMap(OPL::DatabaseCache::MapType::AirportCodesAll);
    const auto &airportNamesIdMap = DBCache->getMap(OPL::DatabaseCache::MapType::AirportNames);

    if (airportCodesMap.contains(text)) {
        displayLabel->setText(airportNamesIdMap.value(airportCodesMap.value(text)));
        on_GoodInputReceived(caller);
        return;
    }

    on_badInputReceived(caller);
}
void FlightLogEntryEditDialog::on_registrationLineEdit_editingFinished()
{
    if (registrationLineEdit->text().isEmpty()) {
        return;
    }

    if (DBCache->getKeyMap(OPL::DatabaseCache::MapType::TailRegistrations)
            .contains(registrationLineEdit->text())) {
        on_GoodInputReceived(registrationLineEdit);
        return;
    }
    else {
        QString completion = registrationLineEdit->completer()->currentCompletion();
        if (completion == QString()) {
            on_badInputReceived(registrationLineEdit);
            DEB << "Add new Tail entry...";
            // todo - prompt to add new
        }
        registrationLineEdit->setText(completion);
        return;
    }

    on_badInputReceived(registrationLineEdit);
}
void FlightLogEntryEditDialog::on_pilotNameLineEdit_editingFinished(QLineEdit *caller)
{
    if (DBCache->getKeyMap(OPL::DatabaseCache::MapType::PilotNames).contains(caller->text())) {
        on_GoodInputReceived(caller);
        return;
    }
    else {
        QString completion = caller->completer()->currentCompletion();
        if (completion == QString()) {
            on_badInputReceived(caller);
            DEB << "Add new Pilot entry...";
            // todo - prompt to add new
        }
        caller->setText(completion);
        return;
    }

    on_badInputReceived(caller);
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
