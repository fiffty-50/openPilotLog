#include "newsimdialog.h"
#include "src/database/databasecache.h"
#include "src/gui/verification/timeinput.h"
#include "ui_newsimdialog.h"
#include "src/opl.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/classes/settings.h"
#include <QCompleter>
/*!
 * \brief create a NewSimDialog to add a new Simulator Entry to the database
 */
NewSimDialog::NewSimDialog(QWidget *parent)
    : EntryEditDialog(parent),
    ui(new Ui::NewSimDialog)
{
    ui->setupUi(this);
    init();

    ui->dateLineEdit->setText(OPL::Date::today(m_format).toString());
}
/*!
 * \brief create a NewSimDialog to edit an existing Simulator Entry
 * \param row_id of the entry to be edited
 */
NewSimDialog::NewSimDialog(int row_id, QWidget *parent)
    : EntryEditDialog(parent),
    ui(new Ui::NewSimDialog)
{
    ui->setupUi(this);
    init();

    entry = DB->getSimEntry(row_id);
    fillEntryData();
}

/*!
 * \brief set up the UI with Combo Box entries and QCompleter
 */
void NewSimDialog::init()
{
    OPL::GLOBALS->loadSimulatorTypes(ui->deviceTypeComboBox);

    const QStringList aircraft_list = DBCache->getAircraftList();
    auto completer = new QCompleter(aircraft_list, ui->aircraftTypeLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->aircraftTypeLineEdit->setCompleter(completer);

    m_format = Settings::getDisplayFormat();
}

/*!
 * \brief fills the UI with data retreived from an existing entry.
 */
void NewSimDialog::fillEntryData()
{
    const auto& data = entry.getData();
    ui->dateLineEdit->setText(OPL::Date(data.value(OPL::SimulatorEntry::DATE).toInt(), m_format).toString());
    ui->totalTimeLineEdit->setText(OPL::Time(data.value(OPL::SimulatorEntry::TIME).toInt()).toString());
    ui->deviceTypeComboBox->setCurrentIndex(data.value(OPL::SimulatorEntry::TYPE).toInt());
    ui->aircraftTypeLineEdit->setText(data.value(OPL::SimulatorEntry::ACFT).toString());
    ui->registrationLineEdit->setText(data.value(OPL::SimulatorEntry::REG).toString());
    ui->remarksLineEdit->setText(data.value(OPL::SimulatorEntry::REMARKS).toString());
}

NewSimDialog::~NewSimDialog()
{
    delete ui;
}

void NewSimDialog::on_dateLineEdit_editingFinished()
{
    const auto date = OPL::Date(ui->dateLineEdit->text(), m_format);
    if(date.isValid()) {
        ui->dateLineEdit->setText(date.toString());
        ui->dateLineEdit->setStyleSheet(QString());
        return;
    } else {
        ui->dateLineEdit->setText(QString());
        ui->dateLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
    }
}


void NewSimDialog::on_totalTimeLineEdit_editingFinished()
{
    const auto input = TimeInput(ui->totalTimeLineEdit->text());
    if(input.isValid())
        return;
    else {
        QString fixed = input.fixup();
        if(fixed == QString()) {
            ui->totalTimeLineEdit->setText(QString());
            ui->totalTimeLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        } else {
            ui->totalTimeLineEdit->setText(fixed);
            ui->totalTimeLineEdit->setStyleSheet(QString());
        }
    }
}

void NewSimDialog::on_registrationLineEdit_textChanged(const QString &arg1)
{
    ui->registrationLineEdit->setText(arg1.toUpper());
}

void NewSimDialog::on_helpPushButton_clicked()
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

bool NewSimDialog::verifyInput(QString& error_msg)
{
    // Date
    const auto date = OPL::Date(ui->dateLineEdit->text(), m_format);

    if (!date.isValid()) {
        ui->dateLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        ui->dateLineEdit->setText(QString());
        error_msg = tr("Invalid Date");
        return false;
    }
    // Time
    if(!TimeInput(ui->totalTimeLineEdit->text()).isValid())
        return false;

    const OPL::Time time = OPL::Time::fromString(ui->totalTimeLineEdit->text());

    if (!time.isValidTimeOfDay()) {
        ui->totalTimeLineEdit->setStyleSheet(OPL::CssStyles::RED_BORDER);
        ui->totalTimeLineEdit->setText(QString());
        error_msg = tr("Invalid time");
        return false;
    }

    // Device Type - for FSTD, aircraft info is required
    if (ui->deviceTypeComboBox->currentIndex() == static_cast<int>(OPL::SimulatorType::FSTD)
            && ui->aircraftTypeLineEdit->text() == QString()) {
        error_msg = tr("For FSTD, please enter the aircraft type.");
        return false;
    }

    return true;
}

OPL::RowData_T NewSimDialog::collectInput()
{
    OPL::RowData_T new_entry;
    // Date
    const auto date = OPL::Date(ui->dateLineEdit->text(), m_format);
    new_entry.insert(OPL::SimulatorEntry::DATE, date.toJulianDay());
    // Time
    new_entry.insert(OPL::SimulatorEntry::TIME, OPL::Time::fromString(ui->totalTimeLineEdit->text()).toMinutes());
    // Device Type
    new_entry.insert(OPL::SimulatorEntry::TYPE, ui->deviceTypeComboBox->currentText());
    // Aircraft Type
    new_entry.insert(OPL::SimulatorEntry::ACFT, ui->aircraftTypeLineEdit->text());
    // Registration
    if (!ui->registrationLineEdit->text().isEmpty())
        new_entry.insert(OPL::SimulatorEntry::REG, ui->registrationLineEdit->text());
    // Remarks
    if (!ui->remarksLineEdit->text().isEmpty())
        new_entry.insert(OPL::SimulatorEntry::REMARKS, ui->remarksLineEdit->text());

    return new_entry;
}

void NewSimDialog::on_buttonBox_accepted()
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
void NewSimDialog::loadEntry(int rowID)
{
    entry = DB->getSimEntry(rowID);
    init();
    fillEntryData();
}

bool NewSimDialog::deleteEntry(int rowID)
{
    const auto entry = DB->getSimEntry(rowID);
    return DB->remove(entry);
}
