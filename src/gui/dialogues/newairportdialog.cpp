#include "newairportdialog.h"
#include "ui_newairportdialog.h"
#include <QValidator>
#include <QTimeZone>

#include "src/opl.h"
#include "src/database/database.h"
#include "src/database/row.h"

NewAirportDialog::NewAirportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAirportDialog)
{
    ui->setupUi(this);
    setValidators();
    loadTimeZones();
}

NewAirportDialog::~NewAirportDialog()
{
    delete ui;
}

void NewAirportDialog::setValidators()
{
    ui->latitudeLineEdit ->setValidator(new QDoubleValidator(-90,90,10,this));   // -90  <= Latitude  <= 90
    ui->longitudeLineEdit->setValidator(new QDoubleValidator(-180,180,10,this)); // -180 <= Longitude <= 180
    ui->icaoLineEdit     ->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{4}"), this)); // 4 letter code
    ui->iataLineEdit     ->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{3}"), this)); // 3 letter code
}

void NewAirportDialog::loadTimeZones()
{
    QStringList tz_list;
    for (const auto &tz : QTimeZone::availableTimeZoneIds())
        tz_list.append(tz);
    ui->timeZoneComboBox->addItems(tz_list);
}

bool NewAirportDialog::confirmTimezone()
{
    if (ui->timeZoneComboBox->currentIndex() == 0) {

        QString airport_name = ui->nameLineEdit->text();
        QString timezone = ui->timeZoneComboBox->currentText();

        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle("Confirm Timezone");
        confirm.setText(tr("Is the following Timezone correct for the airport %1?<br><br><b><tt>"
                           "%2<br></b></tt>"
                           "Correct Timezone data is paramount for correctly converting between UTC and Local Time."
                           ).arg(airport_name, timezone));
        if (confirm.exec() == QMessageBox::Yes)
            return true;
        else
            return false;
    }
    return true;
}

void NewAirportDialog::on_buttonBox_accepted()
{
    // validate input
    if (!ui->latitudeLineEdit->hasAcceptableInput()) {
        WARN(tr("The entered latitude is invalid. Please enter the latitude as a decimal number between -90.0 and 90.0 degrees."));
        return;
    }
    if (!ui->longitudeLineEdit->hasAcceptableInput()) {
        WARN(tr("The entered longitude is invalid. Please enter the longitude as a decimal number between -180.0 and 180.0 degrees."));
        return;
    }
    if (!confirmTimezone())
        return;

    // create Entry object
    OPL::RowData_T airport_data = {
        {OPL::Db::AIRPORTS_ICAO,     ui->icaoLineEdit->text()},
        {OPL::Db::AIRPORTS_IATA,     ui->iataLineEdit->text()},
        {OPL::Db::AIRPORTS_LAT,      ui->latitudeLineEdit->text()},
        {OPL::Db::AIRPORTS_LON,      ui->longitudeLineEdit->text()},
        {OPL::Db::AIRPORTS_TZ_OLSON, ui->timeZoneComboBox->currentText()},
        {OPL::Db::AIRPORTS_COUNTRY,  ui->countryLineEdit->text()},
    };

    OPL::AirportEntry entry(airport_data);
    if(DB->commit(entry))
        QDialog::accept();
    else {
        WARN(tr("Unable to add Airport to the database. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));
        return;
    }
}

void NewAirportDialog::on_iataLineEdit_textChanged(const QString &arg1)
{
    ui->iataLineEdit->setText(arg1.toUpper());
}

void NewAirportDialog::on_icaoLineEdit_textChanged(const QString &arg1)
{
    ui->icaoLineEdit->setText(arg1.toUpper());
}

void NewAirportDialog::on_nameLineEdit_editingFinished()
{
    DEB << "Editing finished: ";
}

void NewAirportDialog::on_iataLineEdit_editingFinished()
{
    DEB << "Editing finished: ";
}

void NewAirportDialog::on_icaoLineEdit_editingFinished()
{
    DEB << "Editing finished: ";
}

void NewAirportDialog::on_latitudeLineEdit_editingFinished()
{
    DEB << "Editing finished: ";
}

void NewAirportDialog::on_longitudeLineEdit_editingFinished()
{
    DEB << "Editing finished: ";
}

void NewAirportDialog::on_latitudeLineEdit_inputRejected()
{
    DEB << "Input Rejected";
}

void NewAirportDialog::on_longitudeLineEdit_inputRejected()
{
    DEB << "Input Rejected";
}

void NewAirportDialog::on_icaoLineEdit_inputRejected()
{
    DEB << "Input Rejected";
}

void NewAirportDialog::on_iataLineEdit_inputRejected()
{
    DEB << "Input Rejected";
}

void NewAirportDialog::on_buttonBox_rejected()
{
    QDialog::reject();
}

