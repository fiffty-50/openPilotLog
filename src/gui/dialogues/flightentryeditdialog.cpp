#include "flightentryeditdialog.h"
#include "src/classes/date.h"
#include "src/gui/verification/airportinput.h"
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

}

void FlightEntryEditDialog::loadEntry(OPL::Row entry)
{

}

bool FlightEntryEditDialog::deleteEntry(int rowID)
{

}

void FlightEntryEditDialog::init()
{
    setupUI();
    // initialise QCompleters
    // load ComboBoxes

    // set up the pop-up date selection
    calendarWidget = new QCalendarWidget(this);
    calendarWidget->setVisible(false);
    calendarWidget->setWindowFlag(Qt::Dialog);

    setupSignalsAndSlots();
}

void FlightEntryEditDialog::setupUI()
{
    // TODO implement OOIB logging, hide for now
    timeOffLabel.hide();
    timeOffLineEdit.hide();
    timeOnLabel.hide();
    timeOnLineEdit.hide();
    // setup layout
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

    // initialise auto-completion
    // finish up
    OPL::GLOBALS->loadApproachTypes(&approachTypeComboBox);
    OPL::GLOBALS->loadPilotFunctios(&pilotFunctionComboBox);
    OPL::GLOBALS->loadFlightRules(&flightRulesComboBox);
    takeOffCountSpinBox.setRange(0, 99);
    landingCountSpinBox.setRange(0, 99);
    totalTimeDisplayLabel.setEnabled(false);

    readSettings();

    // use the dateDisplayLabel as a spacer
    OPL::Date today = OPL::Date::today(displayFormat);
    dateLineEdit.setText(today.toString());
    dateDisplayLabel.setMinimumWidth(200); // TODO make dynamic
    dateDisplayLabel.setText(today.toString());

}

void FlightEntryEditDialog::setupSignalsAndSlots()
{
    // acceptButtonBox
    QObject::connect(&acceptButtonBox, &QDialogButtonBox::accepted,
                     this, &FlightEntryEditDialog::onDialogAccepted);
    QObject::connect(&acceptButtonBox, &QDialogButtonBox::rejected,
                     this, &FlightEntryEditDialog::reject);
    // calendarWidget
    QObject::connect(&dateButton, &QPushButton::clicked,
                     this, &FlightEntryEditDialog::onCalendarRequested);
    QObject::connect(calendarWidget, &QCalendarWidget::selectionChanged,
                     this, &FlightEntryEditDialog::onCalendarDateSelected);
    QObject::connect(calendarWidget, &QCalendarWidget::clicked,
                     this, &FlightEntryEditDialog::onCalendarDateSelected);
    // QLineEdits
    QObject::connect(&dateLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onDateEditingFinished);
    QObject::connect(&timeOutLineEdit, &QLineEdit::editingFinished,

                     this, &FlightEntryEditDialog::onTimeEditingFinished);
    QObject::connect(&timeOffLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onTimeEditingFinished);
    QObject::connect(&timeOnLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onTimeEditingFinished);
    QObject::connect(&timeInLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onTimeEditingFinished);

    QObject::connect(&firstPilotLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onNameEditingFinished);
    QObject::connect(&secondPilotLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onNameEditingFinished);
    QObject::connect(&thirdPilotLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onNameEditingFinished);

    QObject::connect(&registrationLineEdit, &QLineEdit::editingFinished,
                     this, &FlightEntryEditDialog::onRegistrationEditingFinished);

}

void FlightEntryEditDialog::readSettings()
{
    displayFormat = Settings::getDisplayFormat();

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
    OPL::Date date(dateLineEdit.text(), dateTimeFormat);

    if(date.isValid()) {
        clearBorder(dateLineEdit);
        return;
    }

    setRedBorder(dateLineEdit);
}

void FlightEntryEditDialog::onTimeEditingFinished()
{
    auto lineEdit = this->findChild<QLineEdit*>(sender()->objectName());

    if(verifyUserInput(lineEdit, TimeInput(lineEdit->text(), dateTimeFormat))) {
        clearBorder(*lineEdit);
        return;
    }

    setRedBorder(*lineEdit);
}

void FlightEntryEditDialog::onNameEditingFinished()
{
    // TODO
    // - match with map
    // - on negative match, offer to add new pilot entry
    auto lineEdit = this->findChild<QLineEdit*>(sender()->objectName());

    setRedBorder(*lineEdit);
}

void FlightEntryEditDialog::onRegistrationEditingFinished()
{
    // TODO
    // - match with map
    // - on negative match, offer to add new aircraft entry
    auto lineEdit = this->findChild<QLineEdit*>(sender()->objectName());

    setRedBorder(*lineEdit);
}

void FlightEntryEditDialog::onLocationEditingFinished()
{
    // determine which location line edit has been edited
    const QString& lineEditName = sender()->objectName();
    const auto lineEdit = this->findChild<QLineEdit*>(lineEditName);
    QLabel *name_label;
    if (lineEditName.contains(QLatin1String("departure")))
        name_label = &departureDisplayLabel;
    else
        name_label = &destinationDisplayLabel;

    // if(verifyUserInput(lineEdit, AirportInput(lineEdit->text())) ) {
    //     // Match ICAO code with Airport Name and display on label
    //     name_label->setText(DBCache->getAirportsMapNames().value(
    //         DBCache->getAirportsMapICAO().key(
    //             line_edit->text())));
    // } else {
    //     name_label->setText("Unknown Airport");
    //     addNewDatabaseElement(line_edit, OPL::DbTable::Airports);
    // }

}

