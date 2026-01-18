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
#include "pilotentryeditdialog.h"
#include "src/gui/verification/completerprovider.h"
#include "src/opl.h"

#include "src/database/database.h"

/*!
 * \brief PilotEntryEditDialog::PilotEntryEditDialog - creates a new pilot dialog which can be used to add a new entry to the database
 */
PilotEntryEditDialog::PilotEntryEditDialog(QString userInput, QWidget* parent)
    : EntryEditDialog{parent}
{
    init();
    if(userInput != QString()) {
        nameLineEdit->setText(userInput.replace(0, 1, userInput.first(1).toUpper()));
    }
    nameLineEdit->setFocus();
}

/*!
 * \brief PilotEntryEditDialog::PilotEntryEditDialog - creates a new pilot dialog which can be used to edit an existing entry in the database
 * \param rowId - the rowid of the entry to be edited in the database
 */
PilotEntryEditDialog::PilotEntryEditDialog(int rowId, QWidget *parent) :
    EntryEditDialog{rowId, parent}
{
    init();

    pilotEntry = DB->getPilotEntry(rowId);
    DEB << "Editing Pilot: " << pilotEntry;
    formFiller();
    nameLineEdit->setFocus();
}

void PilotEntryEditDialog::init()
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
    nameLineEdit->setMinimumWidth(140);
    nameLineEdit->setObjectName(QStringLiteral("pilot_nameLineEdit"));
    addWidgets(nameLabel, nameLineEdit);

    // Row 1
    companyLabel = new QLabel(this);
    companyLineEdit = new QLineEdit(this);
    companyLineEdit->setObjectName(QStringLiteral("companyLineEdit"));
    auto completer = QCompleterProvider.getCompleter(CompleterProvider::Companies);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    companyLineEdit->setCompleter(completer);
    addWidgets(companyLabel, companyLineEdit);

    // Row 2
    aliasLabel = new QLabel(this);
    aliasLineEdit = new QLineEdit(this);
    aliasLineEdit->setObjectName(QStringLiteral("aliasLineEdit"));
    addWidgets(aliasLabel, aliasLineEdit);

    // Row 3
    employeeidLabel = new QLabel(this);
    employeeidLineEdit = new QLineEdit(this);
    employeeidLineEdit->setObjectName(QStringLiteral("employee_idLineEdit"));
    addWidgets(employeeidLabel, employeeidLineEdit);

    // Row 4
    phoneLabel = new QLabel(this);
    phoneLineEdit = new QLineEdit(this);
    phoneLineEdit->setObjectName(QStringLiteral("phoneLineEdit"));
    addWidgets(phoneLabel, phoneLineEdit);

    // Row 5
    emailLabel = new QLabel(this);
    emailLineEdit = new QLineEdit(this);
    emailLineEdit->setObjectName(QStringLiteral("emailLineEdit"));
    addWidgets(emailLabel, emailLineEdit);

    // Row 6
    remarksLabel = new QLabel(this);
    remarksLineEdit = new QLineEdit(this);
    remarksLineEdit->setObjectName(QStringLiteral("remarksLineEdit"));
    addWidgets(remarksLabel, remarksLineEdit);

    // Row 7
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, row, firstCol, singleSpan, doubleSpan);

    QWidget::setTabOrder({nameLineEdit,
                          companyLineEdit,
                          aliasLineEdit,
                          employeeidLineEdit,
                          phoneLineEdit,
                          emailLineEdit,
                          remarksLineEdit,
                          buttonBox});

    retranslateUi();
    setupSlots();
}

void PilotEntryEditDialog::retranslateUi()
{
    setWindowTitle(tr("Add New Pilot"));
    nameLabel->setText(tr("Name"));
    companyLabel->setText(tr("Company"));
    aliasLabel->setText(tr("Alias"));
    employeeidLabel->setText(tr("Employee ID"));
    phoneLabel->setText(tr("Phone"));
    emailLabel->setText(tr("eMail"));
    remarksLabel->setText(tr("Remarks"));
}

void PilotEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &PilotEntryEditDialog::on_buttonBox_accepted);
}



void PilotEntryEditDialog::on_buttonBox_accepted()
{
    if (nameLineEdit->text().isEmpty()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Name is required."));
        message_box.exec();
    } else {
        submitForm();
    }
}


void PilotEntryEditDialog::formFiller()
{
    const auto line_edits = this->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        le->setText(pilotEntry.getData().value(key).toString());
    }
}

void PilotEntryEditDialog::submitForm()
{
    OPL::RowData_T new_data;
    const auto line_edits = this->findChildren<QLineEdit *>();
    for(auto& le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        auto value = le->text();
        new_data.insert(key, value);
    }

    pilotEntry.setData(new_data);
    DEB << "Submitting Pilot:";
    DEB << pilotEntry;
    if (!DB->commit(pilotEntry)) {
        QMessageBox message_box(this);
        message_box.setText(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(DB->lastError.text()));
        message_box.exec();
        return;
    } else {
        QDialog::accept();
    }
}

bool PilotEntryEditDialog::deleteEntry(int rowId)
{
    return DB->remove(OPL::DbTable::v2Pilots, rowId);
}

void PilotEntryEditDialog::loadEntry(int rowId)
{
    pilotEntry = DB->getPilotEntry(rowId);
    formFiller();
    nameLineEdit->setFocus();
}
