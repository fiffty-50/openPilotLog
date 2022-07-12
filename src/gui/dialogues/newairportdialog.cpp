#include "newairportdialog.h"
#include "ui_newairportdialog.h"
#include <QValidator>

#include "src/opl.h"
#include "src/database/database.h"
#include "src/database/row.h"

NewAirportDialog::NewAirportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAirportDialog)
{
    ui->setupUi(this);
    setValidators();
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
    // create Entry object
    RowData_T airport_data = {
        {OPL::Db::AIRPORTS_ICAO, ui->icaoLineEdit->text()},
        {OPL::Db::AIRPORTS_IATA, ui->iataLineEdit->text()},
        {OPL::Db::AIRPORTS_LAT,  ui->latitudeLineEdit->text()},
        {OPL::Db::AIRPORTS_LON,  ui->longitudeLineEdit->text()},
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
