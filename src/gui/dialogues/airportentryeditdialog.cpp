/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "airportentryeditdialog.h"
#include <QTimeZone>
#include <QValidator>

#include "src/database/database.h"
#include "src/opl.h"

AirportEntryEditDialog::AirportEntryEditDialog(QWidget *parent) : EntryEditDialog(0, parent)
{
    init();
}

AirportEntryEditDialog::AirportEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(row_id, parent)
{
    init();
    loadAirportData(row_id);
}

void AirportEntryEditDialog::init()
{
    // Main Layout
    gridLayout                  = new QGridLayout(this);
    int row                     = 0;
    constexpr int firstCol      = 0;
    constexpr int secondCol     = 1;
    constexpr int thirdCol      = 3;
    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Add widgets to first and second column, advance to next row
    auto addTwoWidgets = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, secondCol, singleSpan, spanRemaining);
        row++;
    };

    // Add widgets to first, second and third column, advance to next row
    auto addThreeWidgets = [&](QWidget *left, QWidget *middle, QWidget *right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(middle, row, secondCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, thirdCol, singleSpan, singleSpan);
        row++;
    };

    // Row 0
    nameLabel    = new QLabel(this);
    nameLineEdit = new QLineEdit(this);
    addTwoWidgets(nameLabel, nameLineEdit);

    // Row 1
    latitudeLabel    = new QLabel(this);
    latDoubleSpinBox = new QDoubleSpinBox(this);
    latDoubleSpinBox->setObjectName("latitudeDoubleSpinBox");
    latDoubleSpinBox->setDecimals(10);
    latDoubleSpinBox->setMinimum(-90.000000000000000);
    latDoubleSpinBox->setMaximum(90.000000000000000);
    addTwoWidgets(latitudeLabel, latDoubleSpinBox);

    // Row 2
    longitudeLabel   = new QLabel(this);
    lonDoubleSpinBox = new QDoubleSpinBox(this);
    lonDoubleSpinBox->setObjectName("longitudeDoubleSpinBox");
    lonDoubleSpinBox->setDecimals(10);
    lonDoubleSpinBox->setMinimum(-180.000000000000000);
    lonDoubleSpinBox->setMaximum(180.000000000000000);
    addTwoWidgets(longitudeLabel, lonDoubleSpinBox);

    // Row 3
    timezoneLabel    = new QLabel(this);
    timeZoneComboBox = new QComboBox(this);
    addTwoWidgets(timezoneLabel, timeZoneComboBox);

    // ICAO Labels
    icaoLabel              = new QLabel(this);
    icaoDisplayLabel       = new QLabel(this);
    editIcaoCodePushButton = new QPushButton(this);
    addThreeWidgets(icaoLabel, icaoDisplayLabel, editIcaoCodePushButton);

    // IATA Labels
    iataLabel              = new QLabel(this);
    iataDisplayLabel       = new QLabel(this);
    editIataCodePushButton = new QPushButton(this);
    addThreeWidgets(iataLabel, iataDisplayLabel, editIataCodePushButton);

    // Other Labels
    otherCodeLabel          = new QLabel(this);
    otherCodeDisplayLabel   = new QLabel(this);
    editOtherCodePushButton = new QPushButton(this);
    addThreeWidgets(otherCodeLabel, otherCodeDisplayLabel, editOtherCodePushButton);

    // Row 4
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel |
                                  QDialogButtonBox::StandardButton::Ok);
    gridLayout->addWidget(buttonBox, row, firstCol, singleSpan, spanRemaining);

    // finish setup
    retranslateUi();
    setupSlots();
    loadTimeZones();
}

void AirportEntryEditDialog::retranslateUi()
{
    m_rowId == 0 ? this->setWindowTitle(tr("Add New Airport"))
                 : this->setWindowTitle(tr("Edit Airport"));

    longitudeLabel->setText(tr("Longitude"));
    latitudeLabel->setText(tr("Latitude"));
    timezoneLabel->setText(tr("Timezone"));
    nameLabel->setText(tr("Airport Name"));
    iataLabel->setText(tr("IATA Code"));
    icaoLabel->setText(tr("ICAO Code"));
    otherCodeLabel->setText(tr("Other Code"));

    const QString edit = tr("Edit");
    editIcaoCodePushButton->setText(edit);
    editIataCodePushButton->setText(edit);
    editOtherCodePushButton->setText(edit);
}

void AirportEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this,
                     &AirportEntryEditDialog::on_buttonBox_accepted);
    QObject::connect(editIcaoCodePushButton, &QPushButton::clicked, this,
                     &AirportEntryEditDialog::on_editIcaoCodePushButton_clicked);
    QObject::connect(editIataCodePushButton, &QPushButton::clicked, this,
                     &AirportEntryEditDialog::on_editIataCodePushButton_clicked);
    QObject::connect(editOtherCodePushButton, &QPushButton::clicked, this,
                     &AirportEntryEditDialog::on_editOtherCodePushButton_clicked);
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

    const auto entry = DB->getAirportEntry(row_id);
    DEB << "Filling Airport Data: " << entry;

    nameLineEdit->setText(entry.getAirportName());
    latDoubleSpinBox->setValue(entry.getLatitude());
    lonDoubleSpinBox->setValue(entry.getLongitude());

    const QString timezone = entry.getTimezone();
    if (timezone.isNull())
        WARN(tr("Unable to read timezone data for this airport. Please verify."));
    timeZoneComboBox->setCurrentText(timezone);

    icaoDisplayLabel->setText(airportData->icao(m_rowId));
    iataDisplayLabel->setText(airportData->iata(m_rowId));
    // TODO - include other codes in airportData
    otherCodeDisplayLabel->setText(
        OPL::AirportCodeEntry::getCurrentCode(m_rowId, OPL::AirportCodeEntry::CodeType::OTHER));
}

bool AirportEntryEditDialog::userWantsToEditCode()
{
    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Confirm"));

    const QString text =
        tr(""
           "Are you sure you want to Edit the Airport Code? "
           "This is normally only necessary when an airport code changes over time "
           "and requires adding a start date for when the new code will be current.<br><br>"
           "<b>Take caution when editing this value as it may break functionality if done "
           "incorrectly.</b><br><br>"
           "It is recommended to read the manual section about airport codes before "
           "proceeding.<br><br>"
           "<p align='right'><b>Do you want to proceed?</b></p>");
    confirm.setText(text);
    return confirm.exec() == QMessageBox::Yes;
}

void AirportEntryEditDialog::on_editIcaoCodePushButton_clicked()
{
    if (userWantsToEditCode()) {
        LOG << "ICAO Code Edit requested.";
        WARN("This functionality is not yet implemented"); // TODO
    }
}

void AirportEntryEditDialog::on_editIataCodePushButton_clicked()
{
    if (userWantsToEditCode()) {
        LOG << "IATA Code Edit requested.";
        WARN("This functionality is not yet implemented"); // TODO
    }
}

void AirportEntryEditDialog::on_editOtherCodePushButton_clicked()
{
    if (userWantsToEditCode()) {
        LOG << "Other Code Edit requested.";
        WARN("This functionality is not yet implemented"); // TODO
    }
}

void AirportEntryEditDialog::on_buttonBox_accepted()
{
    auto entry = OPL::AirportEntry();
    if (!entry.setAirportName(nameLineEdit->text())) {
        WARN(tr("Invalid Airport Name"));
        return;
    }
    if (!entry.setLatitude(latDoubleSpinBox->value())) {
        WARN(tr("Invalid Latitude Value."));
        return;
    }
    if (!entry.setLongitude(lonDoubleSpinBox->value())) {
        WARN(tr("Invalid Longitude Value."));
        return;
    }
    if (!entry.setTimezone(timeZoneComboBox->currentText())) {
        WARN(tr("Invalid Timezone: %1").arg(timeZoneComboBox->currentText()));
        return;
    }

    entry.setRowId(m_rowId);
    if (DB->commit(entry)) {
        m_rowId = DB->getLastEntry(OPL::DbTable::Airports);
        QDialog::accept();
    }
    else {
        WARN(tr("Unable to add Airport to the database. The following error has ocurred:<br><br>%1")
                 .arg(DB->lastError.text()));
        return;
    }
}

// EntryEditDialog interface
void AirportEntryEditDialog::loadEntry(int rowId)
{
    m_rowId = rowId;
    loadAirportData(rowId);
}

bool AirportEntryEditDialog::deleteEntry(int rowId)
{
    auto entry = DB->getAirportEntry(rowId);
    return DB->remove(entry);
}

void AirportEntryEditDialog::reset()
{
    m_rowId = OPL::NEW_ROW_ID;
    nameLineEdit->setText({});
    icaoDisplayLabel->setText({});
    iataDisplayLabel->setText({});
    otherCodeDisplayLabel->setText({});
    lonDoubleSpinBox->setValue(0.0);
    latDoubleSpinBox->setValue(0.0);
    timeZoneComboBox->setCurrentIndex(0);
}
