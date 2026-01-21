#include "flightlogentryeditdialog.h"
#include "src/classes/date.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/database/flightlogentry.h"
#include "src/gui/verification/completerprovider.h"
#include "src/opl.h"
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

void FlightLogEntryEditDialog::loadEntry(int rowID) {}

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
    QFont f          = dateDisplayLabel->font();
    f.setItalic(true);
    dateDisplayLabel->setFont(f);
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

    setTabOrder({datePushButton, dateEdit, deptLineEdit, destLineEdit, timeOffEdit, timeOnEdit,
                 pilotFunctionComboBox, flightRulesComboBox, registrationLineEdit, picLineEdit,
                 sicLineEdit, flightNumberLineEdit, pilotFlyingCheckBox, takeOffCountSpinBox,
                 landingCountSpinBox,  buttonBox});

    retranslateUi();
    setupValidationAndCompletion();
    setupSlots();
    dateEdit->setFocus();
}

void FlightLogEntryEditDialog::retranslateUi()
{
    datePushButton->setText(tr("Date"));
    dateDisplayLabel->setText(dateEdit->date().toString(QStringLiteral("ddd d MMM")));
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

    takeOffCountSpinBox->setMinimum(0);
    landingCountSpinBox->setMinimum(0);
    timeOnEdit->setDisplayFormat(Settings::getTimeFormatString());
    timeOffEdit->setDisplayFormat(Settings::getTimeFormatString());

    OPL::GLOBALS->loadPilotFunctions(pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(flightRulesComboBox);

    // Setup autocompletion and Basic Input Validation
    m_locationLineEdits.resize(2);
    m_locationLineEdits[0] = deptLineEdit;
    m_locationLineEdits[1] = destLineEdit;
    m_nameLineEdits.resize(2);
    m_nameLineEdits[0] = picLineEdit;
    m_nameLineEdits[1] = sicLineEdit;

    LOG << "2";
    for (const auto &line_edit : std::as_const(m_locationLineEdits)) {
        const auto val = new QRegularExpressionValidator(OPL::RegEx::RX_AIRPORT_CODE, line_edit);
        line_edit->setValidator(val);
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Airports));
    }

    LOG << "33";
    for (const auto &line_edit : std::as_const(m_nameLineEdits)) {
        line_edit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Pilots));
    }

    registrationLineEdit->setCompleter(QCompleterProvider.getCompleter(CompleterProvider::Tails));
}

void FlightLogEntryEditDialog::setupSlots()
{
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FlightLogEntryEditDialog::on_accepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

// Slots
void FlightLogEntryEditDialog::on_accepted()
{
    DEB << "Dialog accepted";
    QDialog::accept();
}
