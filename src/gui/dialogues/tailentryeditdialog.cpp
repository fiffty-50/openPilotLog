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
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/opl.h"

TailEntryEditDialog::TailEntryEditDialog(const QString &new_registration, QWidget *parent) :
    EntryEditDialog(parent)
{
    LOG << "Editing New Tail Entry: " << new_registration;
    init();
    setupCompleter();

    registrationLineEdit.setText(new_registration);
    searchLineEdit.setStyleSheet(QStringLiteral("border: 1px solid blue"));
    searchLineEdit.setFocus();

    // Create a new Entry
    m_rowId = 0;
}

TailEntryEditDialog::TailEntryEditDialog(int row_id, QWidget *parent) :
    EntryEditDialog(parent), m_rowId(row_id)
{
    init();

    searchLabel.hide();
    searchLineEdit.hide();
    seperator.hide();

    // retreive the entry to be edited
    const auto entry = DB->getTailEntry(m_rowId);
    fillForm(entry, false);
    LOG << "Editing Tail Entry: " << entry;
}

void TailEntryEditDialog::init()
{
    DEB << "Setting up Grid Layout";
    gridLayout = new QGridLayout(this);

    gridLayout->addWidget(&searchLabel, 0, 0, 1, 1);
    gridLayout->addWidget(&searchLineEdit, 0, 1, 1, 1);

    seperator.setFrameShape(QFrame::Shape::HLine);
    seperator.setFrameShadow(QFrame::Shadow::Sunken);
    gridLayout->addWidget(&seperator, 1, 0, 1, 2);

    registrationLabel.setMinimumWidth(160);
    registrationLineEdit.setMaxLength(20);
    gridLayout->addWidget(&registrationLabel, 2, 0, 1, 1);
    gridLayout->addWidget(&registrationLineEdit, 2, 1, 1, 1);

    gridLayout->addWidget(&companyLabel, 3, 0, 1, 1);
    gridLayout->addWidget(&companyLineEdit, 3, 1, 1, 1);

    gridLayout->addWidget(&makeLabel, 4, 0, 1, 1);
    gridLayout->addWidget(&makeLineEdit, 4, 1, 1, 1);

    gridLayout->addWidget(&modelLabel, 5, 0, 1, 1);
    gridLayout->addWidget(&modelLineEdit, 5, 1, 1, 1);

    gridLayout->addWidget(&variantLabel, 6, 0, 1, 1);
    gridLayout->addWidget(&variantLineEdit, 6, 1, 1, 1);

    gridLayout->addWidget(&operationLabel, 7, 0, 1, 1);
    gridLayout->addWidget(&operationComboBox, 7, 1, 1, 1);

    gridLayout->addWidget(&powerPlantLabel, 8, 0, 1, 1);
    gridLayout->addWidget(&ppTypeComboBox, 8, 1, 1, 1);

    gridLayout->addWidget(&ppNumberComboBox, 9, 1, 1, 1);

    gridLayout->addWidget(&weightLabel, 10, 0, 1, 1);
    gridLayout->addWidget(&weightComboBox, 10, 1, 1, 1);


    buttonBox.setOrientation(Qt::Horizontal);
    buttonBox.setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    gridLayout->addWidget(&buttonBox, 11, 1, 1, 1);

    QWidget::setTabOrder({
                          &searchLineEdit, &registrationLineEdit,
                          &registrationLineEdit, &companyLineEdit,
                          &companyLineEdit, &makeLineEdit,
                          &makeLineEdit, &modelLineEdit,
                          &modelLineEdit, &variantLineEdit,
                          &variantLineEdit, &operationComboBox,
                          &operationComboBox, &ppTypeComboBox,
                          &ppTypeComboBox, &ppNumberComboBox,
                          &ppNumberComboBox, &weightComboBox});

    retranslateUi();
    setupSignalsAndSlots();
}

void TailEntryEditDialog::retranslateUi()
{
    // Title and Labels
    setWindowTitle(tr("Add New Tail"));
    registrationLineEdit.setPlaceholderText(tr("mandatory"));
    makeLineEdit.setPlaceholderText(tr("e.g. Boeing"));
    variantLabel.setText(tr("Variant"));
    modelLineEdit.setPlaceholderText(tr("e.g. 737"));
    searchLineEdit.setPlaceholderText(tr("Start typing to search for aircraft types"));
    QFont font;
    font.setBold(true);
    searchLabel.setFont(font);
    searchLabel.setFrameShape(QFrame::Panel);
    searchLabel.setText(tr("Search"));

    registrationLabel.setText(tr("Registration"));
    makeLabel.setText(tr("Make"));
    modelLabel.setText(tr("Model"));
    variantLabel.setText(tr("Variant"));
    companyLabel.setText(tr("Company"));
    companyLineEdit.setPlaceholderText(tr("optional"));

    // Combo Boxes
    powerPlantLabel.setText(tr("Power Plant"));
    weightLabel.setText(tr("Weight Class"));
    operationLabel.setText(tr("Operation"));

    auto populateComboBox = [](QComboBox *comboBox, const QStringList &items) {
        comboBox->clear();
        comboBox->addItems(items);
        comboBox->setCurrentIndex(0);
    };

    const QString SELECT = tr("<SELECT>");
    populateComboBox(&ppTypeComboBox, {
                                          SELECT,
                                          tr("Unpowered"),
                                          tr("Piston"),
                                          tr("Turboprop"),
                                          tr("Jet")
                                      });
    populateComboBox(&ppNumberComboBox, {
                                            SELECT,
                                            tr("Single Engine"),
                                            tr("Multi Engine")
                                        });
    populateComboBox(&weightComboBox, {
                                          SELECT,
                                          tr("Light"),
                                          tr("Medium"),
                                          tr("Heavy"),
                                          tr("Super")
                                      });
    populateComboBox(&operationComboBox, {
                                             SELECT,
                                             tr("Single Pilot"),
                                             tr("Multi Pilot")
                                         });

}

/*!
 * \brief NewTail::setupCompleter obtains a QHash<QString searchstring, int aircaft_id> for auto completion
 * and obtains a QStringList for QCompleter. This function then sets up the search line edit where
 * the user can select a template from the aircraft database to pre-fill the form with the details
 * for the selected type.
 */
void TailEntryEditDialog::setupCompleter()
{
    QCompleter *completer = new QCompleter(DBCache->getAircraftList(), &searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    searchLineEdit.setCompleter(completer);

    // auto fill on activation and highlighting in case tab is pressed during completion
    QObject::connect(completer, qOverload<const QModelIndex&>(&QCompleter::activated), this, &TailEntryEditDialog::on_searchCompleter_activated);
    QObject::connect(completer, qOverload<const QModelIndex&>(&QCompleter::highlighted), this, &TailEntryEditDialog::on_searchCompleter_activated);

}

void TailEntryEditDialog::setupSignalsAndSlots()
{
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, this, &TailEntryEditDialog::on_buttonBox_accepted);

    QObject::connect(&registrationLineEdit, &QLineEdit::editingFinished,
                     this, &TailEntryEditDialog::on_registrationLineEdit_editingFinished);
    QObject::connect(&ppTypeComboBox, &QComboBox::currentIndexChanged, this,
                     [this](){
                         on_mandatoryComboBox_currentIndexChanged(&ppTypeComboBox);
                     });
    QObject::connect(&ppNumberComboBox, &QComboBox::currentIndexChanged, this,
                     [this](){
                         on_mandatoryComboBox_currentIndexChanged(&ppNumberComboBox);
                     });
    QObject::connect(&operationComboBox, &QComboBox::currentIndexChanged, this,
                     [this](){
                         on_mandatoryComboBox_currentIndexChanged(&operationComboBox);
                     });
    QObject::connect(&weightComboBox, &QComboBox::currentIndexChanged, this,
                     [this](){
                         on_mandatoryComboBox_currentIndexChanged(&weightComboBox);
                     });
}

/*!
 * \brief TailEntryEditDialog::fillForm populates the Dialog with the
 * information contained in an entry object. This can be either
 * a template (AircraftEntry, used when creating a new entry) or
 * a tail (TailEntry, used when editing an existing entry)
 * \param is_template - determines whether we are adding a new entry
 * or editing an existing one.
 */
void TailEntryEditDialog::fillForm(const OPL::Row &entry, bool isTemplate)
{
    DEB << "Filling Form for a/c" << entry;
    const auto data = entry.getData();
    if (!isTemplate) {
        registrationLineEdit.setText(data.value(OPL::TailEntry::REGISTRATION).toString());
    }
    companyLineEdit.setText(data.value(OPL::TailEntry::COMPANY).toString());

    makeLineEdit.setText(data.value(OPL::TailEntry::MAKE).toString());
    modelLineEdit.setText(data.value(OPL::TailEntry::MODEL).toString());
    variantLineEdit.setText(data.value(OPL::TailEntry::VARIANT).toString());

    const int offset = 1; // First item of the Combo Box is "<SELECT>"
    operationComboBox.setCurrentIndex(data.value(OPL::TailEntry::MULTI_PILOT).toInt() + offset);
    ppNumberComboBox.setCurrentIndex(data.value(OPL::TailEntry::MULTI_ENGINE).toInt() + offset);
    ppTypeComboBox.setCurrentIndex(data.value(OPL::TailEntry::ENGINE_TYPE).toInt() + offset);
    weightComboBox.setCurrentIndex(data.value(OPL::TailEntry::WEIGHT_CLASS).toInt() + offset);
}

/*!
 * \brief checks for empty required fields in the form
 * \return true if all required fields are populated
 */
bool TailEntryEditDialog::verify()
{
    bool formValid = true;
    const std::array<QLineEdit *, 3> mandatoryLineEdits = {&registrationLineEdit, &makeLineEdit, &modelLineEdit };
    const std::array<QComboBox *, 3> mandatoryComboBoxes = {&operationComboBox, &ppNumberComboBox, &ppTypeComboBox };

    // Line Edits must be non empty
    for( const auto & lineEdit : mandatoryLineEdits ) {
        if (lineEdit->text().isEmpty()) {
            onBadInputReceived(lineEdit);
            formValid = false;
        } else {
            onGoodInputReceived(lineEdit);
        }
    }

    // Combo Boxes must have a valid selection
    for( const auto & comboBox : mandatoryComboBoxes ) {
        if (comboBox->currentIndex() == 0) {
            onBadInputReceived(comboBox);
            formValid = false;
        } else {
            onGoodInputReceived(comboBox);
        }
    }

    return formValid;
}
/*!
 * \brief NewTail::submitForm collects input from Line Edits and creates
 * or updates a database entry and commits or updates the database
 * \param edRole editExisting or createNew
 */
void TailEntryEditDialog::submitForm()
{
    OPL::RowData_T data;
    // Line Edits
    data.insert(OPL::TailEntry::REGISTRATION, registrationLineEdit.text());
    data.insert(OPL::TailEntry::MAKE, makeLineEdit.text());
    data.insert(OPL::TailEntry::MODEL, modelLineEdit.text());
    data.insert(OPL::TailEntry::VARIANT, variantLineEdit.text());
    data.insert(OPL::TailEntry::COMPANY, companyLineEdit.text());

    // Combo Boxes
    const int offset =  1; // First item in combo boxes is "<SELECT>"
    if (operationComboBox.currentIndex() != 0) { // bool Multipilot
        data.insert(OPL::TailEntry::MULTI_PILOT, operationComboBox.currentIndex() - offset);
    }
    if (ppNumberComboBox.currentIndex() != 0) { // bool MultiEngine
        data.insert(OPL::TailEntry::MULTI_ENGINE, ppNumberComboBox.currentIndex() - offset);
    }
    if (ppTypeComboBox.currentIndex() != 0) { // int 0=unpowered,....4=jet
        data.insert(OPL::TailEntry::ENGINE_TYPE, ppTypeComboBox.currentIndex() - offset);
    }
    if (weightComboBox.currentIndex() != 0) { // int 0=light...3=super
        data.insert(OPL::TailEntry::WEIGHT_CLASS, weightComboBox.currentIndex() - offset);
    }

    //create db object
    auto entry = OPL::TailEntry(m_rowId, data);

    // add type string
    entry.setTypeString();

    LOG << "Commiting: " << entry;
    if (!DB->commit(entry)) {
        QMessageBox message_box(this);
        message_box.setText(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(DB->lastError.text()));
        message_box.exec();
        return;
    } else {
        emit tailDataChanged();
        QDialog::accept();
    }
}

/// Slots

void TailEntryEditDialog::on_mandatoryComboBox_currentIndexChanged(QComboBox *comboBox)
{
    // disable "<SELECT>" item and clear style sheet
    comboBox->setItemData(0, 0, Qt::UserRole - 1);
    onGoodInputReceived(comboBox);
}

void TailEntryEditDialog::on_registrationLineEdit_editingFinished()
{
    registrationLineEdit.setText(registrationLineEdit.text().toUpper());
}

void TailEntryEditDialog::on_buttonBox_accepted()
{
    if (registrationLineEdit.text().isEmpty()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Registration cannot be empty."));
        message_box.exec();
        return;
    }

    if (!verify()) {
        QMessageBox message_box(this);
        message_box.setIcon(QMessageBox::Warning);
        message_box.setText(tr("Some or all required fields are empty.<br>"
                               "Please fill out the mandatory fields. You can use "
                               "the search function to automatically fill out all "
                               "the required fields for a known aircraft type."));
        message_box.exec();
        return;
    }
    submitForm();
}

void TailEntryEditDialog::on_searchCompleter_activated(const QModelIndex &index)
{
    const auto &text = searchLineEdit.text();
    if (DBCache->getAircraftList().contains(text)) {
        //call autofiller for dialog
        fillForm(DB->getAircraftEntry(DBCache->getAircraftMap().key(text)), true);
        searchLineEdit.setStyleSheet(QStringLiteral("border: 1px solid green"));
        searchLabel.setText(text);
    } else {
        //for example, editing finished without selecting a result from Qcompleter
        searchLineEdit.setStyleSheet(QStringLiteral("border: 1px solid orange"));
    }
}

bool TailEntryEditDialog::deleteEntry(int rowID)
{
    auto entry = DB->getTailEntry(rowID);
    return DB->remove(entry);
}

void TailEntryEditDialog::loadEntry(int rowId)
{
    searchLabel.hide();
    searchLineEdit.hide();
    seperator.hide();

    //setupValidators();
    const auto entry = DB->getTailEntry(rowId);
    fillForm(entry, false);
}
