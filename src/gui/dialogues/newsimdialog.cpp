#include "newsimdialog.h"
#include "ui_newsimdialog.h"
#include "src/opl.h"
#include "src/functions/atime.h"
#include "src/functions/adate.h"
#include <QCompleter>
/*!
 * \brief create a NewSimDialog to add a new Simulator Entry to the database
 */
NewSimDialog::NewSimDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimDialog)
{
    //entry = ASimulatorEntry();
    ui->setupUi(this);
    ui->dateLineEdit->setText(ADate::currentDate());
    init();
}
/*!
 * \brief create a NewSimDialog to edit an existing Simulator Entry
 * \param row_id of the entry to be edited
 */
NewSimDialog::NewSimDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimDialog)
{

    ui->setupUi(this);
    entry = DB->getSimEntry(row_id);
    init();
    fillEntryData();
}

/*!
 * \brief set up the UI with Combo Box entries and QCompleter
 */
void NewSimDialog::init()
{
    OPL::GLOBALS->loadSimulatorTypes(ui->deviceTypeComboBox);

    const QStringList aircraft_list = OPL::DbCompletionData::getCompletionList(OPL::CompleterTarget::AircraftTypes);
    auto completer = new QCompleter(aircraft_list, ui->aircraftTypeLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->aircraftTypeLineEdit->setCompleter(completer);
}

/*!
 * \brief fills the UI with data retreived from an existing entry.
 */
void NewSimDialog::fillEntryData()
{
    const auto& data = entry.getData();
    ui->dateLineEdit->setText(data.value(OPL::Db::SIMULATORS_DATE).toString());
    ui->totalTimeLineEdit->setText(ATime::toString(data.value(OPL::Db::SIMULATORS_TIME).toInt()));
    ui->deviceTypeComboBox->setCurrentIndex(data.value(OPL::Db::SIMULATORS_TYPE).toInt());
    ui->aircraftTypeLineEdit->setText(data.value(OPL::Db::SIMULATORS_ACFT).toString());
    ui->registrationLineEdit->setText(data.value(OPL::Db::SIMULATORS_REG).toString());
    ui->remarksLineEdit->setText(data.value(OPL::Db::SIMULATORS_REMARKS).toString());
}

NewSimDialog::~NewSimDialog()
{
    delete ui;
}

void NewSimDialog::on_dateLineEdit_editingFinished()
{
    auto text = ui->dateLineEdit->text();

    OPL::DateFormat date_format = OPL::DateFormat::ISODate;
    auto date = ADate::parseInput(text, date_format);
    if (date.isValid()) {
        ui->dateLineEdit->setText(ADate::toString(date, date_format));
        ui->dateLineEdit->setStyleSheet(QString());
        return;
    } else {
        ui->dateLineEdit->setText(QString());
        ui->dateLineEdit->setStyleSheet(OPL::Styles::RED_BORDER);
    }
}


void NewSimDialog::on_totalTimeLineEdit_editingFinished()
{
    const QString time_string = ATime::formatTimeInput(ui->totalTimeLineEdit->text());
    const QTime time = ATime::fromString(time_string);

    if (time.isValid()) {
        ui->totalTimeLineEdit->setText(time_string);
        ui->totalTimeLineEdit->setStyleSheet(QString());
    } else {
        ui->totalTimeLineEdit->setText(QString());
        ui->totalTimeLineEdit->setStyleSheet(OPL::Styles::RED_BORDER);
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
    auto text = ui->dateLineEdit->text();
    OPL::DateFormat date_format = OPL::DateFormat::ISODate;
    const auto date = ADate::parseInput(text, date_format);

    if (!date.isValid()) {
        ui->dateLineEdit->setStyleSheet(OPL::Styles::RED_BORDER);
        ui->dateLineEdit->setText(QString());
        error_msg = tr("Invalid Date");
        return false;
    }
    // Time
    const QString time_string = ATime::formatTimeInput(ui->totalTimeLineEdit->text());
    const QTime time = ATime::fromString(time_string);

    if (!time.isValid()) {
        ui->totalTimeLineEdit->setStyleSheet(OPL::Styles::RED_BORDER);
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
    new_entry.insert(OPL::Db::SIMULATORS_DATE, ui->dateLineEdit->text());
    // Time
    new_entry.insert(OPL::Db::SIMULATORS_TIME, ATime::toMinutes(ui->totalTimeLineEdit->text()));
    // Device Type
    new_entry.insert(OPL::Db::SIMULATORS_TYPE, ui->deviceTypeComboBox->currentText());
    // Aircraft Type
    new_entry.insert(OPL::Db::SIMULATORS_ACFT, ui->aircraftTypeLineEdit->text());
    // Registration
    if (!ui->registrationLineEdit->text().isEmpty())
        new_entry.insert(OPL::Db::SIMULATORS_REG, ui->registrationLineEdit->text());
    // Remarks
    if (!ui->remarksLineEdit->text().isEmpty())
        new_entry.insert(OPL::Db::FLIGHTS_REMARKS, ui->remarksLineEdit->text());

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
