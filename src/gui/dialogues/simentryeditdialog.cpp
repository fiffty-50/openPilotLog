#include "simentryeditdialog.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/timeinput.h"
#include "src/opl.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/classes/settings.h"
#include <QCompleter>
/*!
 * \brief create a SimEntryEditDialog to add a new Simulator Entry to the database
 */
SimEntryEditDialog::SimEntryEditDialog(QWidget *parent)
    : EntryEditDialog(parent)
{
    init();
    dateLineEdit->setText(OPL::Date::today(m_format).toString());
}
/*!
 * \brief create a SimEntryEditDialog to edit an existing Simulator Entry
 * \param row_id of the entry to be edited
 */
SimEntryEditDialog::SimEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(parent)
{
    init();

    entry = DB->getSimEntry(row_id);
    fillEntryData();
}

/*!
 * \brief set up the UI with Combo Box entries and QCompleter
 */
void SimEntryEditDialog::init()
{
    // Pop-up calendar
    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setWindowFlag(Qt::Dialog);

    // Main Layout
    gridLayout = new QGridLayout(this);
    int row = 0;
    int firstCol = 0;
    int secondCol = 1;
    int singleSpan = 1;

    // Add widgets to left and right side, advance to next row
    auto addWidgets = [&](QWidget* left, QWidget* right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, secondCol, singleSpan, singleSpan);
        row++;
    };

    // Row 0
    dateButton = new QPushButton(this);
    dateLineEdit = new QLineEdit(this);
    dateLineEdit->setMinimumWidth(160);
    addWidgets(dateButton, dateLineEdit);

    // Row 1
    timeLabel = new QLabel(this);
    timeLineEdit = new QLineEdit(this);
    addWidgets(timeLabel, timeLineEdit);

    // Row 2
    simTypeComboBox = new QComboBox(this);
    simTypeLabel = new QLabel(this);
    addWidgets(simTypeLabel, simTypeComboBox);

    // Row 3
    acftTypeLabel = new QLabel(this);
    acftTypeLineEdit = new QLineEdit(this);
    addWidgets(acftTypeLabel, acftTypeLineEdit);

    // Row 4
    registrationLabel = new QLabel(this);
    registrationLineEdit = new QLineEdit(this);
    addWidgets(registrationLabel, registrationLineEdit);

    // Row 5
    remarksLabel = new QLabel(this);
    remarksLineEdit = new QLineEdit(this);
    addWidgets(remarksLabel, remarksLineEdit);

    // Row 6
    helpPushButton = new QPushButton(this);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setLayoutDirection(Qt::LeftToRight);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    addWidgets(helpPushButton, buttonBox);

    QWidget::setTabOrder({
                          dateLineEdit,
                          timeLineEdit,
                          simTypeComboBox,
                          acftTypeLineEdit,
                          registrationLineEdit,
                          remarksLineEdit
    });


    OPL::GLOBALS->loadSimulatorTypes(simTypeComboBox);

    const QStringList aircraft_list = DBCache->getList(OPL::DatabaseCache::ListType::AircraftTypes);
    auto completer = new QCompleter(aircraft_list, acftTypeLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    acftTypeLineEdit->setCompleter(completer);

    m_format = Settings::getDisplayFormat();

    retranslateUi();
    setupSlots();
}

void SimEntryEditDialog::retranslateUi()
{
    setWindowTitle(tr("Add New Simulator Session"));
    dateButton->setText(tr("Date"));
    timeLabel->setText(tr("Total Time of Session"));
    timeLineEdit->setPlaceholderText("00:00");
    simTypeLabel->setText(tr("Simulator Type"));
    acftTypeLabel->setText(tr("Aircraft Type"));
    registrationLabel->setText(tr("Registration"));
    remarksLabel->setText(tr("Remarks"));

    helpPushButton->setText(QStringLiteral("?"));
}

void SimEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     this, &SimEntryEditDialog::on_buttonBox_accepted);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     this, &QDialog::reject);
    QObject::connect(helpPushButton, &QPushButton::clicked,
                     this, &SimEntryEditDialog::on_helpPushButton_clicked);
    QObject::connect(registrationLineEdit, &QLineEdit::editingFinished,
                     this, &SimEntryEditDialog::on_registrationLineEdit_editingFinished);
    QObject::connect(timeLineEdit, &QLineEdit::editingFinished,
                     this, &SimEntryEditDialog::on_timeLineEdit_editingFinished);
    QObject::connect(dateLineEdit, &QLineEdit::editingFinished,
                     this, &SimEntryEditDialog::on_dateLineEdit_editingFinished);

    // Calendar
    QObject::connect(dateButton, &QPushButton::clicked,
                     this, &SimEntryEditDialog::on_datePushButton_clicked);
    QObject::connect(calendar, &QCalendarWidget::selectionChanged,
                     this, &SimEntryEditDialog::on_calendarDateSelected);
    QObject::connect(calendar, &QCalendarWidget::clicked,
                     this, &SimEntryEditDialog::on_calendarDateSelected);
}

/*!
 * \brief fills the UI with data retreived from an existing entry.
 */
void SimEntryEditDialog::fillEntryData()
{
    const auto& data = entry.getData();
    dateLineEdit->setText(OPL::Date(data.value(OPL::SimulatorEntry::DATE).toInt(), m_format).toString());
    timeLineEdit->setText(OPL::Time(data.value(OPL::SimulatorEntry::TIME).toInt(), m_format).toString());
    simTypeComboBox->setCurrentText(data.value(OPL::SimulatorEntry::TYPE).toString());
    acftTypeLineEdit->setText(data.value(OPL::SimulatorEntry::ACFT).toString());
    registrationLineEdit->setText(data.value(OPL::SimulatorEntry::REG).toString());
    remarksLineEdit->setText(data.value(OPL::SimulatorEntry::REMARKS).toString());
}


void SimEntryEditDialog::on_dateLineEdit_editingFinished()
{
    const auto date = OPL::Date(dateLineEdit->text(), m_format);
    if(date.isValid()) {
        dateLineEdit->setText(date.toString());
        dateLineEdit->setStyleSheet(QString());
        return;
    } else {
        dateLineEdit->setText(QString());
        dateLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
}


void SimEntryEditDialog::on_timeLineEdit_editingFinished()
{
    const auto input = TimeInput(timeLineEdit->text(), m_format);
    if(input.isValid())
        return;
    else {
        QString fixed = input.fixup();
        if(fixed == QString()) {
            timeLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        } else {
            timeLineEdit->setText(fixed);
            timeLineEdit->setStyleSheet(QString());
        }
    }
}

void SimEntryEditDialog::on_registrationLineEdit_editingFinished()
{
    registrationLineEdit->setText(registrationLineEdit->text().toUpper());
}

void SimEntryEditDialog::on_datePushButton_clicked()
{
    calendar->setVisible(true);
}

void SimEntryEditDialog::on_calendarDateSelected()
{
    calendar->setVisible(false);
    dateLineEdit->setText(OPL::Date(calendar->selectedDate(), m_format).toString());
}

void SimEntryEditDialog::on_helpPushButton_clicked()
{
    INFO(tr("<br>"
         "For  any  FSTD  enter  the  type  of  aircraft  and  qualification "
         "number  of  the  device.  For  other  flight  training  devices  enter "
         "either FNPT I or FNPT II as appropriate<br><br>"
         "Total time of session includes all exercises carried out in the "
         "device, including pre- and after-flight checks<br><br>"
         "Enter the type of exercise performed in the ‘remarks’ field "
            "for example operator proficiency check, revalidation."));
}

bool SimEntryEditDialog::verifyInput(QString& error_msg)
{
    // Date
    const auto date = OPL::Date(dateLineEdit->text(), m_format);

    if (!date.isValid()) {
        dateLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        dateLineEdit->setText(QString());
        error_msg = tr("Invalid Date");
        return false;
    }
    // Time
    const OPL::Time time = OPL::Time::fromString(timeLineEdit->text(), m_format);
    if (!time.isValidTimeOfDay()) {
        timeLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        timeLineEdit->setText(QString());
        error_msg = tr("Invalid time");
        return false;
    }

    // Device Type - for FSTD, aircraft info is required
    if (simTypeComboBox->currentIndex() == static_cast<int>(OPL::SimulatorType::FSTD)
            && acftTypeLineEdit->text() == QString()) {
        error_msg = tr("For FSTD, please enter the aircraft type.");
        return false;
    }

    return true;
}

OPL::RowData_T SimEntryEditDialog::collectInput()
{
    OPL::RowData_T new_entry;
    // Date
    const auto date = OPL::Date(dateLineEdit->text(), m_format);
    new_entry.insert(OPL::SimulatorEntry::DATE, date.toJulianDay());
    // Time
    new_entry.insert(OPL::SimulatorEntry::TIME, OPL::Time::fromString(timeLineEdit->text(), m_format).toMinutes());
    // Device Type
    new_entry.insert(OPL::SimulatorEntry::TYPE, simTypeComboBox->currentText());
    // Aircraft Type
    new_entry.insert(OPL::SimulatorEntry::ACFT, acftTypeLineEdit->text());
    // Registration
    if (!registrationLineEdit->text().isEmpty())
        new_entry.insert(OPL::SimulatorEntry::REG, registrationLineEdit->text());
    // Remarks
    if (!remarksLineEdit->text().isEmpty())
        new_entry.insert(OPL::SimulatorEntry::REMARKS, remarksLineEdit->text());

    return new_entry;
}

void SimEntryEditDialog::on_buttonBox_accepted()
{
    QString error_msg;
    if (!verifyInput(error_msg)) {
        INFO(error_msg);
        return;
    }

    entry.setData(collectInput());

    DEB << entry;

    if(DB->commit(entry))
        QDialog::accept();
    else
        WARN(tr("Unable to commit entry to database. The following error has ocurred <br><br>%1").arg(DB->lastError.text()));
}

// EntryEdit interface
void SimEntryEditDialog::loadEntry(int rowID)
{
    entry = DB->getSimEntry(rowID);
    init();
    fillEntryData();
}

bool SimEntryEditDialog::deleteEntry(int rowID)
{
    const auto entry = DB->getSimEntry(rowID);
    return DB->remove(entry);
}
