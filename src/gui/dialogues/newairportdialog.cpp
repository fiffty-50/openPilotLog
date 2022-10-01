#include "newairportdialog.h"
#include "ui_newairportdialog.h"
#include <QValidator>
#include <QTimeZone>

#include "src/opl.h"
#include "src/database/database.h"
#include "src/database/row.h"

NewAirportDialog::NewAirportDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::NewAirportDialog)
{
    rowId = 0; // new entry
    ui->setupUi(this);
    setValidators();
    loadTimeZones();
}

NewAirportDialog::NewAirportDialog(int row_id, QWidget *parent)
    : QDialog(parent), ui(new Ui::NewAirportDialog), rowId(row_id)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Edit Airport"));
    setValidators();
    loadTimeZones();
    loadAirportData(row_id);
}

NewAirportDialog::~NewAirportDialog()
{
    delete ui;
}

void NewAirportDialog::setValidators()
{
    ui->icaoLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{4}"), this)); // 4 letter code
    ui->iataLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{3}"), this)); // 3 letter code
}

void NewAirportDialog::loadTimeZones()
{
    QStringList tz_list;
    for (const auto &tz : QTimeZone::availableTimeZoneIds())
        tz_list.append(tz);
    ui->timeZoneComboBox->addItems(tz_list);
}

void NewAirportDialog::loadAirportData(int row_id)
{
    const auto airport_data = DB->getAirportEntry(row_id).getData();
    //const auto airport_data = airport.getData();
    DEB << "Filling Airport Data: " << airport_data;

    ui->nameLineEdit->setText(airport_data.value(OPL::Db::AIRPORTS_NAME).toString());
    ui->icaoLineEdit->setText(airport_data.value(OPL::Db::AIRPORTS_ICAO).toString());
    ui->iataLineEdit->setText(airport_data.value(OPL::Db::AIRPORTS_IATA).toString());
    ui->latDoubleSpinBox->setValue(airport_data.value(OPL::Db::AIRPORTS_LAT).toDouble());
    ui->lonDoubleSpinBox->setValue(airport_data.value(OPL::Db::AIRPORTS_LON).toDouble());
    ui->countryLineEdit->setText(airport_data.value(OPL::Db::AIRPORTS_COUNTRY).toString());

    const QString timezone = airport_data.value(OPL::Db::AIRPORTS_TZ_OLSON).toString();
    DEB << "Timezone: " << timezone;
    if (timezone.isNull())
        WARN(tr("Unable to read timezone data for this airport. Please verify."));
    ui->timeZoneComboBox->setCurrentText(timezone);
}

bool NewAirportDialog::verifyInput()
{
    if (ui->nameLineEdit->text().isEmpty()) {
        WARN(tr("Please enter the airport name."));
        return false;
    }
    if (ui->icaoLineEdit->text().length() != 4) {
        WARN(tr("Invalid ICAO Code."));
        return false;
    }
    if (ui->latDoubleSpinBox->value() == 0 || ui->lonDoubleSpinBox->value() == 0) {
        WARN(tr("Please enter the latitude and longitude in decimal degrees.<br><br>"
                "This data is required for calculation of sunrise and sunset times."));
        return false;
    }

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
    if (!verifyInput())
        return;
    // create Entry object
    OPL::RowData_T airport_data = {
        {OPL::Db::AIRPORTS_NAME,     ui->nameLineEdit->text()},
        {OPL::Db::AIRPORTS_ICAO,     ui->icaoLineEdit->text()},
        {OPL::Db::AIRPORTS_IATA,     ui->iataLineEdit->text()},
        {OPL::Db::AIRPORTS_LAT,      ui->latDoubleSpinBox->value()},
        {OPL::Db::AIRPORTS_LON,      ui->lonDoubleSpinBox->value()},
        {OPL::Db::AIRPORTS_TZ_OLSON, ui->timeZoneComboBox->currentText()},
        {OPL::Db::AIRPORTS_COUNTRY,  ui->countryLineEdit->text()},
    };

    OPL::AirportEntry entry(rowId, airport_data);
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

void NewAirportDialog::on_buttonBox_rejected()
{
    QDialog::reject();
}

