#include "airportentryeditdialog.h"
#include <QValidator>
#include <QTimeZone>

#include "src/opl.h"
#include "src/database/database.h"

AirportEntryEditDialog::AirportEntryEditDialog(QWidget *parent) :
    EntryEditDialog(parent)
{
    init();
    m_rowId = 0; // new entry
}

AirportEntryEditDialog::AirportEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(parent), m_rowId(row_id)
{
    init();
    loadAirportData(row_id);
}

void AirportEntryEditDialog::init()
{
    // Main Layout
    gridLayout = new QGridLayout(this);
    int row = 0;
    const int firstCol = 0;
    const int secondCol = 1;
    const int singleSpan = 1;
    const int doubleSpan = 2;

    // Add widgets to left and right side, advance to next row
    auto addWidgets = [&](QWidget* left, QWidget* right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, secondCol, singleSpan, singleSpan);
        row++;
    };

    // Row 0
    nameLabel = new QLabel(this);
    nameLineEdit = new QLineEdit(this);
    addWidgets(nameLabel, nameLineEdit);

    // Row 1
    iataLineEdit = new QLineEdit(this);
    iataLineEdit->setMaxLength(3);
    iataLabel = new QLabel(this);
    addWidgets(iataLabel, iataLineEdit);

    // Row 2
    icaoLabel = new QLabel(this);
    icaoLineEdit = new QLineEdit(this);
    icaoLineEdit->setMaxLength(4);
    addWidgets(icaoLabel, icaoLineEdit);

    // Row 3
    countryLabel = new QLabel(this);
    countryLineEdit = new QLineEdit(this);
    addWidgets(countryLabel, countryLineEdit);


    // Row 4
    latitudeLabel = new QLabel(this);
    latDoubleSpinBox = new QDoubleSpinBox(this);
    latDoubleSpinBox->setObjectName("latDoubleSpinBox");
    latDoubleSpinBox->setDecimals(10);
    latDoubleSpinBox->setMinimum(-90.000000000000000);
    latDoubleSpinBox->setMaximum(90.000000000000000);
    addWidgets(latitudeLabel, latDoubleSpinBox);

    // Row 5
    longitudeLabel = new QLabel(this);
    lonDoubleSpinBox = new QDoubleSpinBox(this);
    lonDoubleSpinBox->setDecimals(10);
    lonDoubleSpinBox->setMinimum(-180.000000000000000);
    lonDoubleSpinBox->setMaximum(180.000000000000000);
    addWidgets(longitudeLabel, lonDoubleSpinBox);

    // Row 6
    timezoneLabel = new QLabel(this);
    timeZoneComboBox = new QComboBox(this);
    addWidgets(timezoneLabel, timeZoneComboBox);

    // Row 7
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);
    gridLayout->addWidget(buttonBox, row, firstCol, singleSpan, doubleSpan);

    // finish setup
    retranslateUi();
    setupSlots();
    setValidators();
    loadTimeZones();
}

void AirportEntryEditDialog::retranslateUi()
{
    m_rowId == 0 ?
        this->setWindowTitle(tr("Add New Airport")) :
        this->setWindowTitle(tr("Edit Airport"));

    countryLabel->setText(tr("Country"));
    longitudeLabel->setText(tr("Longitude"));
    latitudeLabel->setText(tr("Latitude"));
    timezoneLabel->setText(tr("Timezone"));
    icaoLabel->setText(tr("ICAO Code"));
    iataLabel->setText(tr("IATA Code"));
    nameLabel->setText(tr("Airport Name"));
}

void AirportEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &AirportEntryEditDialog::on_buttonBox_accepted);
    QObject::connect(icaoLineEdit, &QLineEdit::textChanged, this, &AirportEntryEditDialog::on_icaoLineEdit_textChanged);
    QObject::connect(iataLineEdit, &QLineEdit::textChanged, this, &AirportEntryEditDialog::on_iataLineEdit_textChanged);
}

void AirportEntryEditDialog::setValidators()
{
    icaoLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{4}"), this)); // 4 letter code
    iataLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\w{3}"), this)); // 3 letter code
}

void AirportEntryEditDialog::loadTimeZones()
{
    QStringList tz_list;
    for (const auto &tz : QTimeZone::availableTimeZoneIds())
        tz_list.append(tz);
    timeZoneComboBox->addItems(tz_list);
}

void AirportEntryEditDialog::loadAirportData(int row_id)
{
    this->setWindowTitle(tr("Edit Airport"));
    const auto airport_data = DB->getAirportEntry(row_id).getData();
    DEB << "Filling Airport Data: " << airport_data;

    nameLineEdit->setText(airport_data.value(OPL::AirportEntry::NAME).toString());
    icaoLineEdit->setText(airport_data.value(OPL::AirportEntry::ICAO).toString());
    iataLineEdit->setText(airport_data.value(OPL::AirportEntry::IATA).toString());
    latDoubleSpinBox->setValue(airport_data.value(OPL::AirportEntry::LAT).toDouble());
    lonDoubleSpinBox->setValue(airport_data.value(OPL::AirportEntry::LON).toDouble());
    countryLineEdit->setText(airport_data.value(OPL::AirportEntry::COUNTRY).toString());

    const QString timezone = airport_data.value(OPL::AirportEntry::TZ_OLSON).toString();
    DEB << "Timezone: " << timezone;
    if (timezone.isNull())
        WARN(tr("Unable to read timezone data for this airport. Please verify."));
    timeZoneComboBox->setCurrentText(timezone);
}

bool AirportEntryEditDialog::verifyInput()
{
    if (nameLineEdit->text().isEmpty()) {
        WARN(tr("Please enter the airport name."));
        return false;
    }
    if (icaoLineEdit->text().length() != 4) {
        WARN(tr("Invalid ICAO Code."));
        return false;
    }
    if (latDoubleSpinBox->value() == 0 || lonDoubleSpinBox->value() == 0) {
        WARN(tr("Please enter the latitude and longitude in decimal degrees.<br><br>"
                "This data is required for calculation of sunrise and sunset times."));
        return false;
    }

    if (timeZoneComboBox->currentIndex() == 0) {

        QString airport_name = nameLineEdit->text();
        QString timezone = timeZoneComboBox->currentText();

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

void AirportEntryEditDialog::on_buttonBox_accepted()
{
    if (!verifyInput())
        return;
    // create Entry object
    OPL::RowData_T airport_data = {
        {OPL::AirportEntry::NAME,     nameLineEdit->text()},
        {OPL::AirportEntry::ICAO,     icaoLineEdit->text()},
        {OPL::AirportEntry::IATA,     iataLineEdit->text()},
        {OPL::AirportEntry::LAT,      latDoubleSpinBox->value()},
        {OPL::AirportEntry::LON,      lonDoubleSpinBox->value()},
        {OPL::AirportEntry::TZ_OLSON, timeZoneComboBox->currentText()},
        {OPL::AirportEntry::COUNTRY,  countryLineEdit->text()},
    };

    OPL::AirportEntry entry(m_rowId, airport_data);
    if(DB->commit(entry))
        QDialog::accept();
    else {
        WARN(tr("Unable to add Airport to the database. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));
        return;
    }
}

void AirportEntryEditDialog::on_iataLineEdit_textChanged(const QString &arg1)
{
    iataLineEdit->setText(arg1.toUpper());
}

void AirportEntryEditDialog::on_icaoLineEdit_textChanged(const QString &arg1)
{
    icaoLineEdit->setText(arg1.toUpper());
}

// EntryEditDialog interface
void AirportEntryEditDialog::loadEntry(int rowId)
{
    m_rowId = rowId;
    loadAirportData(rowId);
}

void AirportEntryEditDialog::loadEntry(const OPL::Row &entry)
{
    m_rowId = entry.getRowId();
    loadAirportData(m_rowId);
}


bool AirportEntryEditDialog::deleteEntry(int rowId)
{
    auto entry = DB->getAirportEntry(rowId);
    return DB->remove(entry);
}

