#include "newsimdialog.h"
#include "ui_newsimdialog.h"
#include "src/opl.h"
#include "src/functions/atime.h"
#include "src/functions/adate.h"
#include <QCompleter>

NewSimDialog::NewSimDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimDialog)
{
    ui->setupUi(this);
    ui->dateLineEdit->setText(ADate::currentDate());
    Opl::GLOBALS->loadSimulatorTypes(ui->typeComboBox);

    const QStringList aircraft_list = aDB->getCompletionList(ADatabaseTarget::aircraft);
    auto completer = new QCompleter(aircraft_list, ui->acftLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->acftLineEdit->setCompleter(completer);
}

NewSimDialog::~NewSimDialog()
{
    delete ui;
}

void NewSimDialog::on_dateLineEdit_editingFinished()
{
    auto text = ui->dateLineEdit->text();

    Opl::Date::ADateFormat date_format = Opl::Date::ADateFormat::ISODate;
    auto date = ADate::parseInput(text, date_format);
    if (date.isValid()) {
        ui->dateLineEdit->setText(ADate::toString(date, date_format));
        ui->dateLineEdit->setStyleSheet(QString());
        return;
    } else {
        ui->dateLineEdit->setText(QString());
        ui->dateLineEdit->setStyleSheet(Opl::Styles::RED_BORDER);
    }
}


void NewSimDialog::on_timeLineEdit_editingFinished()
{
    const QString time_string = ATime::formatTimeInput(ui->timeLineEdit->text());
    const QTime time = ATime::fromString(time_string);

    if (time.isValid()) {
        ui->timeLineEdit->setText(time_string);
        ui->timeLineEdit->setStyleSheet(QString());
    } else {
        ui->timeLineEdit->setText(QString());
        ui->timeLineEdit->setStyleSheet(Opl::Styles::RED_BORDER);
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
    Opl::Date::ADateFormat date_format = Opl::Date::ADateFormat::ISODate;
    const auto date = ADate::parseInput(text, date_format);

    if (!date.isValid()) {
        ui->dateLineEdit->setStyleSheet(Opl::Styles::RED_BORDER);
        ui->dateLineEdit->setText(QString());
        error_msg = tr("Invalid Date");
        return false;
    }
    // Time
    const QString time_string = ATime::formatTimeInput(ui->timeLineEdit->text());
    const QTime time = ATime::fromString(time_string);

    if (!time.isValid()) {
        ui->timeLineEdit->setStyleSheet(Opl::Styles::RED_BORDER);
        ui->timeLineEdit->setText(QString());
        error_msg = tr("Invalid time");
        return false;
    }

    // Device Type - for FSTD, aircraft info is required
    if (ui->typeComboBox->currentIndex() == Opl::SimulatorTypes::FSTD
            && ui->acftLineEdit->text() == QString()) {
        error_msg = tr("For FSTD, please enter the aircraft type.");
        return false;
    }

    return true;
}

RowData_T NewSimDialog::collectInput()
{
    RowData_T new_entry;
    // Date
    new_entry.insert(Opl::Db::SIMULATORS_DATE, ui->dateLineEdit->text());
    // Time
    new_entry.insert(Opl::Db::SIMULATORS_TIME, ATime::toMinutes(ui->timeLineEdit->text()));
    // Device Type
    new_entry.insert(Opl::Db::SIMULATORS_TYPE, ui->typeComboBox->currentText());
    // Aircraft Type
    new_entry.insert(Opl::Db::SIMULATORS_ACFT, ui->acftLineEdit->text());
    // Registration
    if (!ui->registrationLineEdit->text().isEmpty())
        new_entry.insert(Opl::Db::SIMULATORS_REG, ui->registrationLineEdit->text());
    // Remarks
    if (!ui->remarksLineEdit->text().isEmpty())
        new_entry.insert(Opl::Db::FLIGHTS_REMARKS, ui->remarksLineEdit->text());

    return new_entry;
}

void NewSimDialog::on_buttonBox_accepted()
{
    QString error_msg;
    if (!verifyInput(error_msg)) {
        INFO(error_msg);
        return;
    }

    auto sim_entry = ASimulatorEntry(collectInput());
    DEB << sim_entry;
    aDB->commit(sim_entry);
}
