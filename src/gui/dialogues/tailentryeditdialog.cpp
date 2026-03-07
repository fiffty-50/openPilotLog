/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "tailentryeditdialog.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/gui/comboboxes/aircrafttypeselectionbox.h"
#include "src/opl.h"
#include "src/classes/date.h"

TailEntryEditDialog::TailEntryEditDialog(const QString &new_registration, QWidget *parent)
    : EntryEditDialog(0, parent)
{
    //LOG << "Editing New Tail Entry: " << new_registration;
    init();

    registrationLineEdit->setText(new_registration);
    aircraftTypeComboBox->lineEdit()->setFocus();
    aircraftTypeComboBox->lineEdit()->selectAll();
}

TailEntryEditDialog::TailEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(row_id, parent)
{
    init();

    aircraftTypeComboBox->hide();
    aircraftTypeLabel->hide();
    seperator->hide();

    // retreive the entry to be edited
    const auto entry = DB->getTailEntry(m_rowId);
    //LOG << "Editing Tail Entry: " << entry;
    fillForm(entry);
}

void TailEntryEditDialog::init()
{
    // Main Layout
    gridLayout                  = new QGridLayout(this);
    int row                     = 0;
    constexpr int firstCol      = 0;
    constexpr int secondCol     = 1;
    constexpr int thirdCol      = 2;
    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Add widgets to first and second column, advance to next row
    auto addTwoWidgets = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, secondCol, singleSpan, spanRemaining);
        row++;
    };

    // Header Row
    aircraftTypeLabel    = new QLabel(this);
    aircraftTypeComboBox = new AircraftTypeSelectionBox(this);
    setDefaultPushButton = new QPushButton(this);
    gridLayout->addWidget(aircraftTypeLabel, row, firstCol, singleSpan, singleSpan);
    gridLayout->addWidget(aircraftTypeComboBox, row, secondCol, singleSpan, singleSpan);
    gridLayout->addWidget(setDefaultPushButton, row, thirdCol, singleSpan, singleSpan);
    row++;

    seperator = new QFrame(this);
    seperator->setFrameShape(QFrame::Shape::HLine);
    seperator->setFrameShadow(QFrame::Shadow::Sunken);
    gridLayout->addWidget(seperator, row, firstCol, singleSpan, spanRemaining);
    row++;

    // Registration

    registrationLabel = new QLabel(this);
    // registrationLabel->setMinimumWidth(160);
    registrationLineEdit = new QLineEdit(this);
    addTwoWidgets(registrationLabel, registrationLineEdit);

    // Company
    companyLabel    = new QLabel(this);
    companyLineEdit = new QLineEdit(this);
    addTwoWidgets(companyLabel, companyLineEdit);

    // Remarks
    remarksLabel    = new QLabel(this);
    remarksLineEdit = new QLineEdit(this);
    addTwoWidgets(remarksLabel, remarksLineEdit);

    // Check Box
    editServiceDatesCheckBox = new QCheckBox(this);
    editServiceDatesCheckBox->setLayoutDirection(Qt::RightToLeft);
    gridLayout->addWidget(editServiceDatesCheckBox, row, firstCol, singleSpan, spanRemaining);
    row++;

    // Seperator
    seperator2 = new QFrame(this);
    seperator2->setFrameShape(QFrame::Shape::HLine);
    seperator2->setFrameShadow(QFrame::Shadow::Sunken);
    gridLayout->addWidget(seperator2, row, firstCol, singleSpan, spanRemaining);
    row++;

    // In Service
    inServiceLabel        = new QLabel(this);
    const auto dateFormat = OPL::DateTimeFormat().dateFormatString();
    inServiceDateEdit     = new QDateEdit(this);
    inServiceDateEdit->setDisplayFormat(dateFormat);
    inServiceDateEdit->setMinimumDate(OPL::Date::minimumDate());
    inServiceDateEdit->setMaximumDate(OPL::Date::maximumDate());
    inServiceDateEdit->setDate(OPL::Date::minimumDate());
    inServiceDateEdit->setEnabled(false);
    addTwoWidgets(inServiceLabel, inServiceDateEdit);

    // Out of Service
    outOfServiceLabel    = new QLabel(this);
    outOfServiceDateEdit = new QDateEdit(this);
    outOfServiceDateEdit->setDisplayFormat(dateFormat);
    outOfServiceDateEdit->setMinimumDate(OPL::Date::minimumDate());
    outOfServiceDateEdit->setMaximumDate(OPL::Date::maximumDate());
    outOfServiceDateEdit->setDate(OPL::Date::maximumDate());
    outOfServiceDateEdit->setEnabled(false);
    addTwoWidgets(outOfServiceLabel, outOfServiceDateEdit);

    // Button Box
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, row, secondCol, singleSpan, singleSpan);

    QWidget::setTabOrder(
        {aircraftTypeComboBox, registrationLineEdit, companyLineEdit, remarksLineEdit, buttonBox});

    retranslateUi();
    hideServiceDateEdits();

    // Read the default value from settings
    int default_type_id = Settings::getDefaultAircraftType();
    int idx = aircraftTypeComboBox->findData(default_type_id);
    if (idx > -1) {
        aircraftTypeComboBox->setCurrentIndex(idx);
    }

    // Connect Slots
    QObject::connect(registrationLineEdit, &QLineEdit::editingFinished, this,
                     &TailEntryEditDialog::on_registrationLineEdit_editingFinished);
    QObject::connect(setDefaultPushButton, &QPushButton::clicked, this,
                     &TailEntryEditDialog::on_setDefaultPushButton_clicked);

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this,
                     &TailEntryEditDialog::on_buttonBox_accepted);

    QObject::connect(editServiceDatesCheckBox, &QCheckBox::checkStateChanged, this,
                     &TailEntryEditDialog::on_dateEditCheckBox_changed);
}

void TailEntryEditDialog::retranslateUi()
{
    // Title and Labels
    setWindowTitle(tr("Tail Editor"));
    registrationLineEdit->setPlaceholderText(tr("mandatory"));
    aircraftTypeLabel->setText(tr("<b>Select Aircraft Type<b>"));
    setDefaultPushButton->setText(tr("Set as Default"));

    registrationLabel->setText(tr("Registration"));
    companyLabel->setText(tr("Company"));
    companyLineEdit->setPlaceholderText(tr("optional"));
    remarksLabel->setText(tr("Remarks"));
    remarksLineEdit->setPlaceholderText(tr("optional"));

    editServiceDatesCheckBox->setText("Edit Service Interval");
    inServiceLabel->setText(tr("In service date"));
    outOfServiceLabel->setText(tr("Out of service date"));
}

void TailEntryEditDialog::fillForm(const OPL::TailEntry &entry)
{
    DEB << "Filling Form for a/c" << entry;
    const auto data = entry.getData();
    registrationLineEdit->setText(entry.getRegistration());
    companyLineEdit->setText(entry.getCompany());
    remarksLineEdit->setText(entry.getRemarks());

    const QDate inService = entry.getInServiceDate();
    if (inService != OPL::Date::minimumDate()) {
        inServiceDateEdit->setDate(inService);
    }
    const QDate outOfService = entry.getOutOfServiceDate();
    if (outOfService <= OPL::Date::minimumDate()) {
        outOfServiceDateEdit->setDate(OPL::Date::maximumDate());
    }
    else {
        outOfServiceDateEdit->setDate(outOfService);
    }

    int idx = aircraftTypeComboBox->findData(entry.getTypeId());
    if (idx > -1) {
        aircraftTypeComboBox->setCurrentIndex(idx);
    }
}

void TailEntryEditDialog::showServiceDateEdits()
{
    seperator2->show();
    inServiceLabel->show();
    inServiceDateEdit->show();
    outOfServiceLabel->show();
    outOfServiceDateEdit->show();

    inServiceDateEdit->setEnabled(true);
    outOfServiceDateEdit->setEnabled(true);
}

void TailEntryEditDialog::hideServiceDateEdits()
{
    seperator2->hide();
    inServiceLabel->hide();
    inServiceDateEdit->hide();
    outOfServiceLabel->hide();
    outOfServiceDateEdit->hide();

    inServiceDateEdit->setEnabled(false);
    outOfServiceDateEdit->setEnabled(false);
}

/// Slots

void TailEntryEditDialog::on_registrationLineEdit_editingFinished()
{
    registrationLineEdit->setText(registrationLineEdit->text().toUpper());
}

void TailEntryEditDialog::on_buttonBox_accepted()
{
    // Create the entry Object
    auto entry = OPL::TailEntry();
    entry.setRowId(m_rowId);
    DEB << entry.getData();

    // Add mandatory data
    if (!entry.setRegistration(registrationLineEdit->text())) {
        WARN(tr("The registration is invalid."));
        return;
    }

    const int type_id = aircraftTypeComboBox->currentData().toInt();
    DEB << "Type ID: " << type_id;

    if (!entry.setTypeId(type_id)) {
        WARN(tr("Unknown aircraft type: <b>%1</b><br>"
                "If this is the first time you are adding a tail with this "
                "aircraft type and it is not yet present in the database, you must "
                "first add it")
                 .arg(aircraftTypeComboBox->currentText()));
        return;
    }
    if (!entry.setInServiceDate(inServiceDateEdit->date())) {
        WARN(tr("Invalid in service date."));
        return;
    }

    // Optional Data
    entry.setCompany(companyLineEdit->text());
    entry.setRemarks(remarksLineEdit->text());
    // set out of service date (only if not default)
    if (outOfServiceDateEdit->date() == OPL::Date::maximumDate()) {
        entry.setOutOfServiceDate(QDate());
    }
    else {
        entry.setOutOfServiceDate(outOfServiceDateEdit->date());
    }

    // Submit to the Database
    LOG << "Commiting: " << entry;
    if (!DB->commit(entry)) {
        WARN(tr("The following error has ocurred:"
                "<br><br>%1<br><br>"
                "The entry has not been saved.")
                 .arg(DB->lastErrorText()));
        return;
    }
    else {
        m_rowId = DB->getLastEntry(OPL::DbTable::AircraftTails);
        emit tailDataChanged();
        QDialog::accept();
    }
}

void TailEntryEditDialog::on_searchCompleter_activated(const QModelIndex &index)
{
    aircraftTypeComboBox->setCurrentText(index.data().toString());
}

void TailEntryEditDialog::on_dateEditCheckBox_changed(Qt::CheckState state)
{
    switch (state) {
    case Qt::CheckState::Checked: {
        QMessageBox question;
        question.setIcon(QMessageBox::Question);
        question.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        question.setDefaultButton(QMessageBox::No);
        question.setText(
            (tr("If the same registration is re-used among several different aircraft "
                "over time, you can set a manual interval for when a certain registration "
                "is active on a given aircraft type<br><br>This is not normally required "
                "and should <b>only</b> be used if the same registration appears on different "
                "aircraft types in your logbook.<br><br>Do you want to continue?")));
        if (question.exec() == QMessageBox::Yes)
            showServiceDateEdits();
        else
            editServiceDatesCheckBox->setChecked(false);
        break;
    }
    case Qt::CheckState::Unchecked:
        hideServiceDateEdits();
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

void TailEntryEditDialog::on_setDefaultPushButton_clicked()
{
    int type_id = aircraftTypeComboBox->currentData().toInt();
    Settings::setDefaultAircraftType(type_id);
    INFO(tr("The default aircraft type for new aircraft has been set to<br><br><b>%1</b>")
             .arg(aircraftTypeComboBox->currentText()));
}

// EntryEditDialog Interface Implementation

bool TailEntryEditDialog::deleteEntry(int rowID)
{
    auto entry = DB->getTailEntry(rowID);
    return DB->remove(entry);
}

void TailEntryEditDialog::loadEntry(int rowId)
{
    m_rowId          = rowId;
    const auto entry = DB->getTailEntry(rowId);
    fillForm(entry);
}

void TailEntryEditDialog::reset()
{
    m_rowId = OPL::NEW_ROW_ID;
    aircraftTypeComboBox->setCurrentIndex(Settings::getDefaultAircraftType());
    registrationLineEdit->setText({});
    companyLineEdit->setText({});
    remarksLineEdit->setText({});
    editServiceDatesCheckBox->setCheckState(Qt::Unchecked);
    inServiceDateEdit->setDate(OPL::Date::minimumDate());
    outOfServiceDateEdit->setDate(OPL::Date::maximumDate());
}
