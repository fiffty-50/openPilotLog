/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#include "newtaildialog.h"
#include "ui_newtail.h"
#include "src/testing/adebug.h"

static const auto REG_VALID = QPair<QString, QRegularExpression> {
    "registrationLineEdit", QRegularExpression("\\w+-\\w+")};
static const auto MAKE_VALID = QPair<QString, QRegularExpression> {
    "makeLineEdit", QRegularExpression("[-a-zA-Z\\s]+")};
static const auto MODEL_VALID = QPair<QString, QRegularExpression> {
    "modelLineEdit", QRegularExpression("[\\s\\w-]+")};
static const auto VARIANT_VALID = QPair<QString, QRegularExpression> {
    "variantLineEdit", QRegularExpression("[\\s\\w-]+")};
static const auto LINE_EDIT_VALIDATORS = QVector({REG_VALID, MAKE_VALID, MODEL_VALID, VARIANT_VALID});


NewTailDialog::NewTailDialog(QString new_registration, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    DEB "new NewTailDialog";
    ui->setupUi(this);

    setupCompleter();
    setupValidators();

    ui->registrationLineEdit->setText(new_registration);
    ui->searchLineEdit->setStyleSheet("border: 1px solid blue");
    ui->searchLineEdit->setFocus();

    entry = ATailEntry();
}

NewTailDialog::NewTailDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    DEB "New New Pilot Dialog (edit existing)";
    ui->setupUi(this);

    ui->searchLabel->hide();
    ui->searchLineEdit->hide();
    ui->line->hide();

    setupValidators();
    entry = aDB()->getTailEntry(row_id);
    fillForm(entry, false);
}

NewTailDialog::~NewTailDialog()
{
    DEB "Deleting NewTailDialog\n";
    delete ui;
}
/// Functions

/*!
 * \brief NewTail::setupCompleter obtains a QMap<QString searchstring, int aircaft_id> for auto completion
 * and obtains a QStringList for QCompleter. This function then sets up the search line edit where
 * the user can select a template from the aircraft database to pre-fill the form with the details
 * for the selected type.
 */
void NewTailDialog::setupCompleter()
{
    idMap = aDB()->getIdMap(ADatabaseTarget::aircraft);
    aircraftList = aDB()->getCompletionList(ADatabaseTarget::aircraft);

    QCompleter *completer = new QCompleter(aircraftList, ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);

    QObject::connect(completer, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),
                     this, &NewTailDialog::onSearchCompleterActivated);
    QObject::connect(completer, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::highlighted),
                     this, &NewTailDialog::onSearchCompleterActivated);


}

void NewTailDialog::setupValidators()
{
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = this->findChild<QLineEdit*>(pair.first);
        auto validator = new QRegularExpressionValidator(pair.second, line_edit);
        line_edit->setValidator(validator);
    }
}

/*!
 * \brief NewTailDialog::fillForm populates the Dialog with the
 * information contained in an entry object. This can be either
 * a template (AAircraft, used when creating a new entry) or
 * a tail (ATail, used when editing an existing entry)
 * \param entry
 */
void NewTailDialog::fillForm(AEntry entry, bool is_template)
{
    DEB "Filling Form for a/c" << entry.getPosition().tableName << entry.getPosition().rowId;
    //fill Line Edits
    auto line_edits = this->findChildren<QLineEdit *>();

    if (is_template)
        line_edits.removeOne(ui->registrationLineEdit);

    auto data = entry.getData();

    for (const auto &le : line_edits) {
        auto key = le->objectName().remove("LineEdit");
        le->setText(data.value(key).toString());
    }

    ui->operationComboBox->setCurrentIndex(data.value(DB_TAILS_MULTIPILOT).toInt() + 1);
    ui->ppNumberComboBox ->setCurrentIndex(data.value(DB_TAILS_MULTIENGINE).toInt() + 1);
    ui->ppTypeComboBox->setCurrentIndex(data.value(DB_TAILS_ENGINETYPE).toInt() + 1);
    ui->weightComboBox->setCurrentIndex(data.value(DB_TAILS_WEIGHTCLASS).toInt() + 1);
}

/*!
 * \brief NewTail::verify A simple check for empty recommended fields in the form
 * \return true if all reconmmended fields are populated
 */
bool NewTailDialog::verify()
{
    auto recommended_line_edits = this->findChildren<QLineEdit *>("registrationLineEdit");
    recommended_line_edits.append(this->findChild<QLineEdit *>("makeLineEdit"));
    recommended_line_edits.append(this->findChild<QLineEdit *>("modelLineEdit"));

    auto recommended_combo_boxes = this->findChildren<QComboBox *>("operationComboBox");
    recommended_combo_boxes.append(this->findChild<QComboBox *>("ppNumberComboBox"));
    recommended_combo_boxes.append(this->findChild<QComboBox *>("ppTypeComboBox"));

    for (const auto &le : recommended_line_edits) {
        if (le->text() != "") {
            DEB "Good: " << le;
            recommended_line_edits.removeOne(le);
            le->setStyleSheet("");
        } else {
            le->setStyleSheet("border: 1px solid red");
            DEB "Not Good: " << le;
        }
    }
    for (const auto &cb : recommended_combo_boxes) {
        if (cb->currentIndex() != 0) {

            recommended_combo_boxes.removeOne(cb);
            cb->setStyleSheet("");
        } else {
            cb->setStyleSheet("background: orange");
            DEB "Not Good: " << cb;
        }
    }

    if (recommended_line_edits.isEmpty() && recommended_combo_boxes.isEmpty()) {
        return true;
    } else {
        return false;
    }
}
/*!
 * \brief NewTail::submitForm collects input from Line Edits and creates
 * or updates a database entry and commits or updates the database
 * \param edRole editExisting or createNew
 */
void NewTailDialog::submitForm()
{
    DEB "Creating Database Object...";
    RowData new_data;
    //retreive Line Edits
    auto line_edits = this->findChildren<QLineEdit *>();
    line_edits.removeOne(this->findChild<QLineEdit *>("searchLineEdit"));

    for (const auto &le : line_edits) {
        auto key = le->objectName().remove("LineEdit");
        new_data.insert(key, le->text());
    }

    if (ui->operationComboBox->currentIndex() != 0) { // bool Multipilot
        new_data.insert(DB_TAILS_MULTIPILOT, ui->operationComboBox->currentIndex() - 1);
    }
    if (ui->ppNumberComboBox->currentIndex() != 0) { // bool MultiEngine
        new_data.insert(DB_TAILS_MULTIENGINE, ui->ppNumberComboBox->currentIndex() - 1);
    }
    if (ui->ppTypeComboBox->currentIndex() != 0) { // int 0=unpowered,....4=jet
        new_data.insert(DB_TAILS_ENGINETYPE, ui->ppTypeComboBox->currentIndex() - 1);
    }
    if (ui->weightComboBox->currentIndex() != 0) { // int 0=light...3=super
        new_data.insert(DB_TAILS_WEIGHTCLASS, ui->weightComboBox->currentIndex() - 1);
    }

    //create db object

    entry.setData(new_data);
    if (!aDB()->commit(entry)) {
        auto message_box = QMessageBox(this);
        message_box.setText("The following error has ocurred:\n\n"
                            + aDB()->lastError.text()
                            + "\n\nThe entry has not been saved.");
        message_box.exec();
        return;
    } else {
        if (entry.getPosition().rowId != 0)
            ACalc::updateAutoTimes(entry.getPosition().rowId);
        QDialog::accept();
    }
}

/// Slots

void NewTailDialog::on_operationComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->operationComboBox->setStyleSheet("");
    }
}

void NewTailDialog::on_ppTypeComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->ppTypeComboBox->setStyleSheet("");
    }
}

void NewTailDialog::on_ppNumberComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->ppNumberComboBox->setStyleSheet("");
    }
}

void NewTailDialog::on_weightComboBox_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->weightComboBox->setStyleSheet("");
    }
}

void NewTailDialog::on_buttonBox_accepted()
{
    DEB "Button Box Accepted.";
    if (ui->registrationLineEdit->text().isEmpty()) {
        auto nope = QMessageBox(this);
        nope.setText("Registration cannot be empty.");
        nope.exec();
        return;
    }

    if (!verify()) {
        if (!ASettings::read("userdata/acAllowIncomplete").toInt()) {
            auto nope = QMessageBox(this);
            nope.setIcon(QMessageBox::Warning);
            nope.setText("Some or all recommended fields are empty.\nPlease go back and "
                         "complete the form.\n\nYou can allow logging incomplete tail entries on the settings page.");
            nope.exec();
            return;
        } else {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Warning",
                                          "Some recommended fields are empty.\n\n"
                                          "If you do not fill out the aircraft details, "
                                          "it will be impossible to automatically determine Single/Multi Pilot Times or Single/Multi Engine Time."
                                          "This will also impact statistics and auto-logging capabilites.\n\n"
                                          "It is highly recommended to fill in all the details.\n\n"
                                          "Are you sure you want to proceed?",
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                submitForm();
            }
        }
    }
    DEB "Form verified";
    submitForm();
}

void NewTailDialog::onSearchCompleterActivated()
{
    DEB "Search completer activated!";
    const auto &text = ui->searchLineEdit->text();
    if (aircraftList.contains(text)) {

            DEB "Template Selected. aircraft_id is: " << idMap.value(text);
            //call autofiller for dialog
            fillForm(aDB()->getAircraftEntry(idMap.value(text)), true);
            ui->searchLineEdit->setStyleSheet("border: 1px solid green");
            ui->searchLabel->setText(text);
        } else {
            //for example, editing finished without selecting a result from Qcompleter
            ui->searchLineEdit->setStyleSheet("border: 1px solid orange");
        }
}

void NewTailDialog::on_registrationLineEdit_textChanged(const QString &arg1)
{
    ui->registrationLineEdit->setText(arg1.toUpper());
}
