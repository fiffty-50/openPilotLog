/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "src/functions/alog.h"
#include "src/opl.h"

NewTailDialog::NewTailDialog(const QString &new_registration, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);

    setupCompleter();
    setupValidators();

    ui->registrationLineEdit->setText(new_registration);
    ui->searchLineEdit->setStyleSheet(QStringLiteral("border: 1px solid blue"));
    ui->searchLineEdit->setFocus();

    //entry = OPL::TailEntry();
}

NewTailDialog::NewTailDialog(int row_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);

    ui->searchLabel->hide();
    ui->searchLineEdit->hide();
    ui->line->hide();

    setupValidators();
    entry = aDB->getTailEntry(row_id);
    LOG << "Editing: " << entry;
    fillForm(entry, false);
}

NewTailDialog::~NewTailDialog()
{
    delete ui;
}

/*!
 * \brief NewTail::setupCompleter obtains a QHash<QString searchstring, int aircaft_id> for auto completion
 * and obtains a QStringList for QCompleter. This function then sets up the search line edit where
 * the user can select a template from the aircraft database to pre-fill the form with the details
 * for the selected type.
 */
void NewTailDialog::setupCompleter()
{
    idMap = aDB->getIdMap(ADatabaseTarget::aircraft);
    aircraftList = aDB->getCompletionList(ADatabaseTarget::aircraft);

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
    const QHash<QLatin1String, QRegularExpression> line_edit_validators = {
        {QLatin1String("registrationLineEdit"), QRegularExpression(QLatin1String("\\w+-\\w+"))},
        {QLatin1String("makeLineEdit"),         QRegularExpression(QLatin1String("[-a-zA-Z\\s]+"))},
        {QLatin1String("modelLineEdit"),        QRegularExpression(QLatin1String("[\\s\\w-]+"))},
        {QLatin1String("variantLineEdit"),      QRegularExpression(QLatin1String("[\\s\\w-]+"))},
    };

    QHash<QLatin1String, QRegularExpression>::const_iterator i;
    for (i = line_edit_validators.constBegin(); i != line_edit_validators.constEnd(); ++i) {
        const auto line_edit = this->findChild<QLineEdit*>(i.key());
        auto validator = new QRegularExpressionValidator(i.value(), line_edit);
        line_edit->setValidator(validator);
    }
}

/*!
 * \brief NewTailDialog::fillForm populates the Dialog with the
 * information contained in an entry object. This can be either
 * a template (AircraftEntry, used when creating a new entry) or
 * a tail (TailEntry, used when editing an existing entry)
 * \param is_template - determines whether we are adding a new entry
 * or editing an existing one.
 */
void NewTailDialog::fillForm(OPL::Row entry, bool is_template)
{
    DEB << "Filling Form for a/c" << entry;
    //fill Line Edits
    auto line_edits = this->findChildren<QLineEdit *>();

    if (is_template)
        line_edits.removeOne(ui->registrationLineEdit);

    auto data = entry.getRowData();

    for (const auto &le : qAsConst(line_edits)) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        le->setText(data.value(key).toString());
    }

    ui->operationComboBox->setCurrentIndex(data.value(OPL::Db::TAILS_MULTIPILOT).toInt() + 1);
    ui->ppNumberComboBox ->setCurrentIndex(data.value(OPL::Db::TAILS_MULTIENGINE).toInt() + 1);
    ui->ppTypeComboBox->setCurrentIndex(data.value(OPL::Db::TAILS_ENGINETYPE).toInt() + 1);
    ui->weightComboBox->setCurrentIndex(data.value(OPL::Db::TAILS_WEIGHTCLASS).toInt() + 1);
}

/*!
 * \brief NewTail::verify A simple check for empty recommended fields in the form
 * \return true if all reconmmended fields are populated
 */
bool NewTailDialog::verify()
{
    auto recommended_line_edits = this->findChildren<QLineEdit *>(QStringLiteral("registrationLineEdit"));
    recommended_line_edits.append(this->findChild<QLineEdit *>(QStringLiteral("makeLineEdit")));
    recommended_line_edits.append(this->findChild<QLineEdit *>(QStringLiteral("modelLineEdit")));

    auto recommended_combo_boxes = this->findChildren<QComboBox *>(QStringLiteral("operationComboBox"));
    recommended_combo_boxes.append(this->findChild<QComboBox *>(QStringLiteral("ppNumberComboBox")));
    recommended_combo_boxes.append(this->findChild<QComboBox *>(QStringLiteral("ppTypeComboBox")));

    for (const auto &le : qAsConst(recommended_line_edits)) {
        if (le->text() != "") {
            DEB << "Good: " << le;
            recommended_line_edits.removeOne(le);
            le->setStyleSheet("");
        } else {
            le->setStyleSheet(QStringLiteral("border: 1px solid red"));
            DEB << "Not Good: " << le;
        }
    }
    for (const auto &cb : qAsConst(recommended_combo_boxes)) {
        if (cb->currentIndex() != 0) {

            recommended_combo_boxes.removeOne(cb);
            cb->setStyleSheet(QString());
        } else {
            cb->setStyleSheet(QStringLiteral("background: orange"));
            DEB << "Not Good: " << cb;
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
    RowData_T new_data;
    //retreive Line Edits
    auto line_edits = this->findChildren<QLineEdit *>();
    line_edits.removeOne(this->findChild<QLineEdit *>(QStringLiteral("searchLineEdit")));

    for (const auto &le : qAsConst(line_edits)) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        new_data.insert(key, le->text());
    }

    if (ui->operationComboBox->currentIndex() != 0) { // bool Multipilot
        new_data.insert(OPL::Db::TAILS_MULTIPILOT, ui->operationComboBox->currentIndex() - 1);
    }
    if (ui->ppNumberComboBox->currentIndex() != 0) { // bool MultiEngine
        new_data.insert(OPL::Db::TAILS_MULTIENGINE, ui->ppNumberComboBox->currentIndex() - 1);
    }
    if (ui->ppTypeComboBox->currentIndex() != 0) { // int 0=unpowered,....4=jet
        new_data.insert(OPL::Db::TAILS_ENGINETYPE, ui->ppTypeComboBox->currentIndex() - 1);
    }
    if (ui->weightComboBox->currentIndex() != 0) { // int 0=light...3=super
        new_data.insert(OPL::Db::TAILS_WEIGHTCLASS, ui->weightComboBox->currentIndex() - 1);
    }

    //create db object

    entry.setRowData(new_data);
    LOG << "Commiting: " << entry;
    if (!aDB->commit(entry)) {
        QMessageBox message_box(this);
        message_box.setText(tr("The following error has ocurred:"
                               "<br><br>%1<br><br>"
                               "The entry has not been saved."
                               ).arg(aDB->lastError.text()));
        message_box.exec();
        return;
    } else {
        QDialog::accept();
    }
}

/// Slots

void NewTailDialog::on_operationComboBox_currentIndexChanged(int index)
{
    if (index != 0)
        ui->operationComboBox->setStyleSheet(QString());
}

void NewTailDialog::on_ppTypeComboBox_currentIndexChanged(int index)
{
    if (index != 0)
        ui->ppTypeComboBox->setStyleSheet(QString());
}

void NewTailDialog::on_ppNumberComboBox_currentIndexChanged(int index)
{
    if (index != 0)
        ui->ppNumberComboBox->setStyleSheet(QString());
}

void NewTailDialog::on_weightComboBox_currentIndexChanged(int index)
{
    if (index != 0)
        ui->weightComboBox->setStyleSheet(QString());
}

void NewTailDialog::on_buttonBox_accepted()
{
    DEB << "Button Box Accepted.";
    if (ui->registrationLineEdit->text().isEmpty()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Registration cannot be empty."));
        message_box.exec();
        return;
    }

    if (!verify()) {
        QMessageBox message_box(this);
        message_box.setIcon(QMessageBox::Warning);
        message_box.setText(tr("Some or all recommended fields are empty.<br>"
                               "Please fill out the mandatory fields. You can use "
                               "the search function to automatically fill out all "
                               "the required fields for a known aircraft type."));
        message_box.exec();
        return;
    }
    submitForm();
}

void NewTailDialog::onSearchCompleterActivated()
{
    const auto &text = ui->searchLineEdit->text();
    if (aircraftList.contains(text)) {

            DEB << "Template Selected. aircraft_id is: " << idMap.key(text);
            //call autofiller for dialog
            fillForm(aDB->getAircraftEntry(idMap.key(text)), true);
            ui->searchLineEdit->setStyleSheet(QStringLiteral("border: 1px solid green"));
            ui->searchLabel->setText(text);
        } else {
            //for example, editing finished without selecting a result from Qcompleter
            ui->searchLineEdit->setStyleSheet(QStringLiteral("border: 1px solid orange"));
        }
}

void NewTailDialog::on_registrationLineEdit_textChanged(const QString &arg1)
{
    ui->registrationLineEdit->setText(arg1.toUpper());
}
