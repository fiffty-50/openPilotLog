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
    latitudeLabel = new QLabel(this);
    latDoubleSpinBox = new QDoubleSpinBox(this);
    latDoubleSpinBox->setObjectName("latitideDoubleSpinBox");
    latDoubleSpinBox->setDecimals(10);
    latDoubleSpinBox->setMinimum(-90.000000000000000);
    latDoubleSpinBox->setMaximum(90.000000000000000);
    addWidgets(latitudeLabel, latDoubleSpinBox);

    // Row 2
    longitudeLabel = new QLabel(this);
    lonDoubleSpinBox = new QDoubleSpinBox(this);
    lonDoubleSpinBox->setObjectName("longitudeDoubleSpinBox");
    lonDoubleSpinBox->setDecimals(10);
    lonDoubleSpinBox->setMinimum(-180.000000000000000);
    lonDoubleSpinBox->setMaximum(180.000000000000000);
    addWidgets(longitudeLabel, lonDoubleSpinBox);

    // Row 3
    timezoneLabel = new QLabel(this);
    timeZoneComboBox = new QComboBox(this);
    addWidgets(timezoneLabel, timeZoneComboBox);

    // ICAO Labels
    icaoLabel = new QLabel(this);
    icaoDisplayLabel = new QLabel(this);
    addWidgets(icaoLabel, icaoDisplayLabel);

    // IATA Labels
    iataLabel = new QLabel(this);
    iataDisplayLabel = new QLabel(this);
    addWidgets(iataLabel, iataDisplayLabel);

    // Row 4
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);
    gridLayout->addWidget(buttonBox, row, firstCol, singleSpan, doubleSpan);

    // finish setup
    retranslateUi();
    setupSlots();
    loadTimeZones();
}

void AirportEntryEditDialog::retranslateUi()
{
    m_rowId == 0 ?
        this->setWindowTitle(tr("Add New Airport")) :
        this->setWindowTitle(tr("Edit Airport"));

    longitudeLabel->setText(tr("Longitude"));
    latitudeLabel->setText(tr("Latitude"));
    timezoneLabel->setText(tr("Timezone"));
    nameLabel->setText(tr("Airport Name"));
    iataLabel->setText(tr("IATA Code"));
    icaoLabel->setText(tr("ICAO Code"));
}

void AirportEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &AirportEntryEditDialog::on_buttonBox_accepted);
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
    latDoubleSpinBox->setValue(airport_data.value(OPL::AirportEntry::LATITUDE).toDouble());
    lonDoubleSpinBox->setValue(airport_data.value(OPL::AirportEntry::LONGITUDE).toDouble());

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
        {OPL::AirportEntry::NAME,     	 nameLineEdit->text()},
        {OPL::AirportEntry::LATITUDE,    latDoubleSpinBox->value()},
        {OPL::AirportEntry::LONGITUDE,   lonDoubleSpinBox->value()},
        {OPL::AirportEntry::TZ_OLSON,	 timeZoneComboBox->currentText()},
    };

    OPL::AirportEntry entry(m_rowId, airport_data);
    if(DB->commit(entry))
        QDialog::accept();
    else {
        WARN(tr("Unable to add Airport to the database. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));
        return;
    }
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

