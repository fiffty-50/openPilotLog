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
#include "newpilotdialog.h"
#include "ui_newpilot.h"

#include "src/database/adatabase.h"
#include "src/classes/aentry.h"
#include "src/functions/alog.h"

/* Examples for names around the world:
 * José Eduardo Santos Tavares Melo Silva
 * María José Carreño Quiñones
 * 毛泽东先生 (Mao Ze Dong xiān shēng)
 * Борис Николаевич Ельцин (Boris Nikolayevich Yeltsin)
 * John Q. Public
 * Abu Karim Muhammad al-Jamil ibn Nidal ibn Abdulaziz al-Filistini
 * Nguyễn Tấn Dũng
 * 東海林賢蔵
 * Chris van de Hoopen
 * Karl-Gustav von Meiershausen
 * Mathias d'Arras
 * Martin Luther King, Jr.
 */

static const auto NAME_RX = QLatin1String("(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)");
static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("firstnameLineEdit"), QRegularExpression(NAME_RX + NAME_RX + NAME_RX)};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("lastnameLineEdit"), QRegularExpression(NAME_RX + NAME_RX + NAME_RX)};
static const auto PHONE_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("phoneLineEdit"), QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
static const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("emailLineEdit"), QRegularExpression(
                "\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
static const auto COMPANY_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("companyLineEdit"), QRegularExpression("\\w+(\\s|\\-)?(\\w+(\\s|\\-)?)?(\\w+(\\s|\\-)?)?")};
static const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
     QStringLiteral("employeeidLineEdit"), QRegularExpression("\\w+")};

static const auto LINE_EDIT_VALIDATORS = QVector<QPair<QString, QRegularExpression>> {
        FIRSTNAME_VALID,
        LASTNAME_VALID,
        PHONE_VALID,
        EMAIL_VALID,
        COMPANY_VALID,
        EMPLOYEENR_VALID
};

// For creating a new entry
NewPilotDialog::NewPilotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    DEB << "New NewPilotDialog (newEntry)";
    setup();

    pilotEntry = APilotEntry();
    ui->lastnameLineEdit->setFocus();
}

NewPilotDialog::NewPilotDialog(int rowId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    DEB << "New NewPilotDialog (editEntry)";
    setup();

    pilotEntry = aDB->getPilotEntry(rowId);
    DEB << "Pilot Entry position: " << pilotEntry.getPosition().tableName << pilotEntry.getPosition().rowId;
    formFiller();
    ui->lastnameLineEdit->setFocus();
}

NewPilotDialog::~NewPilotDialog()
{
    DEB << "Deleting New NewPilotDialog";
    delete ui;
}

void NewPilotDialog::setup()
{
    ui->setupUi(this);

    DEB << "Setting up Validators...";
    for (const auto& pair : LINE_EDIT_VALIDATORS) {
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        if (line_edit != nullptr) {
            auto validator = new QRegularExpressionValidator(pair.second,line_edit);
            line_edit->setValidator(validator);
        } else {
            DEB << "Error: Line Edit not found: "<< pair.first << " - skipping.";
        }
    }

    DEB << "Setting up completer...";
    auto completer = new QCompleter(aDB->getCompletionList(ADatabaseTarget::companies), ui->companyLineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->companyLineEdit->setCompleter(completer);
}

void NewPilotDialog::on_buttonBox_accepted()
{
    if (ui->lastnameLineEdit->text().isEmpty() || ui->firstnameLineEdit->text().isEmpty()) {
        QMessageBox message_box(this);
        message_box.setText(tr("Last Name and First Name are required."));
        message_box.exec();
    } else {
        submitForm();
    }
}

void NewPilotDialog::formFiller()
{
    DEB << "Filling Form...";
    auto line_edits = this->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        le->setText(pilotEntry.getData().value(key).toString());
    }
}

void NewPilotDialog::submitForm()
{
    DEB << "Collecting User Input...";

    RowData_T new_data;
    auto line_edits = this->findChildren<QLineEdit *>();
    for(auto& le : line_edits) {
        auto key = le->objectName().remove(QStringLiteral("LineEdit"));
        auto value = le->text();
        new_data.insert(key, value);
    }

    pilotEntry.setData(new_data);
    DEB << "Pilot entry position: " << pilotEntry.getPosition().tableName << pilotEntry.getPosition().rowId;
    DEB << "Pilot entry data: " << pilotEntry.getData();
    if (!aDB->commit(pilotEntry)) {
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
