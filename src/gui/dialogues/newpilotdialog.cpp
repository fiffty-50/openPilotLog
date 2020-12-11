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
#include "newpilotdialog.h"
#include "ui_newpilot.h"
#include "debug.h"

#include "src/experimental/DataBase.h"
#include "src/experimental/Entry.h"
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
// [F] I think we don't even need static here at all, as it should be implicitly static anyway?
// [G] Thats not how it works. Its such a leap of assumption to make them static.
// in this context static declare that the variables will only be used by this cpp file (translation unit)
// which is where they are used anyway so we should keep the static and be explicit.
// The same thing happens to static functions (outside of classes). They are "local" functions.
static const auto NAME_RX = QLatin1String("(\\p{L}+(\\s|'|\\-)?\\s?(\\p{L}+)?\\s?)");
static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    "picfirstnameLineEdit", QRegularExpression(NAME_RX + NAME_RX + NAME_RX)};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    "piclastnameLineEdit", QRegularExpression(NAME_RX + NAME_RX + NAME_RX)};
static const auto PHONE_VALID = QPair<QString, QRegularExpression> {
    "phoneLineEdit", QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
static const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
    "emailLineEdit", QRegularExpression("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                                        "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
static const auto COMPANY_VALID = QPair<QString, QRegularExpression> {
    "companyLineEdit", QRegularExpression("\\w+(\\s|\\-)?(\\w+(\\s|\\-)?)?(\\w+(\\s|\\-)?)?")};
static const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
    "employeeidLineEdit", QRegularExpression("\\w+")};

static const auto LINE_EDIT_VALIDATORS = QVector{
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
    DEB("New NewPilotDialog\n");
    setup();

    using namespace experimental;
    pilotEntry = PilotEntry();
    ui->piclastnameLineEdit->setFocus();
}

NewPilotDialog::NewPilotDialog(int rowId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    setup();

    using namespace experimental;
    pilotEntry = DB()->getPilotEntry(rowId);
    DEB("Pilot Entry position: " << pilotEntry.getPosition());
    formFiller();
    ui->piclastnameLineEdit->setFocus();
}

NewPilotDialog::~NewPilotDialog()
{
    DEB("Deleting New NewPilotDialog\n");
    delete ui;
}

void NewPilotDialog::setup()
{
    ui->setupUi(this);

    DEB("Setting up Validators...");
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        if(line_edit != nullptr){
            auto validator = new QRegularExpressionValidator(pair.second,line_edit);
            line_edit->setValidator(validator);
        }else{
            DEB("Error: Line Edit not found: "<< pair.first << " - skipping.");
        }
    }

    DEB("Setting up completer...");
    auto companies = new CompletionList(CompleterTarget::companies);
    auto completer = new QCompleter(companies->list, ui->companyLineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->companyLineEdit->setCompleter(completer);
}

void NewPilotDialog::on_buttonBox_accepted()
{
    if (ui->piclastnameLineEdit->text().isEmpty() || ui->picfirstnameLineEdit->text().isEmpty()) {
        auto mb = QMessageBox(this);
        mb.setText("Last Name and First Name are required.");
        mb.show();
    } else {
        submitForm();
    }
}

void NewPilotDialog::onCommitSuccessful()
{
    accept();
}

void NewPilotDialog::onCommitUnsuccessful(const QSqlError &sqlError, const QString &)
{
    auto mb = QMessageBox(this);
    mb.setText("The following error has ocurred. Your entry has not been saved./n/n"
               + sqlError.text());
}

void NewPilotDialog::formFiller()
{
    DEB("Filling Form...");
    auto line_edits = this->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        QString key = le->objectName().remove("LineEdit");
        QString value = pilotEntry.getData().value(key);
        le->setText(value);
    }
}

void NewPilotDialog::submitForm()
{
    DEB("Creating Database Object...");
    QMap<QString, QString> newData;

    auto line_edits = this->findChildren<QLineEdit *>();

    for(auto& le : line_edits) {
        auto key = le->objectName().remove("LineEdit");
        auto value = le->text();
        newData.insert(key, value);
    }

    // [G]: If this formating is Entry-Subclass specific
    // shouldnt PilotEntry know what to do with the database-centric pilot name?
    // [F]: That's one way of looking at it - I see it more as something derived
    // from a QLineEdit included in the 'package' of data the entry gets from the
    // Dialo. Where in the PilotEntry would you see it as more appropriate?
    // [G]: Not sure i get exactly what you mean but my point is that we have leak of 
    // logic again. I see two alternatives. encapsulate the displayname "creation" in the PilotEntry,
    // or remove it from the database all together. In my eyes displayname doesnt provide anything intersting
    // for the database. It might aswell just be the output of the function that takes the actual interesting
    // data that is the picfirst and piclast names. I suggest we go for the second. 
    // TL;DR displayname is useless, remove it even from the database. Any "display name" required
    // will be the output of a function/method that takes the actual data picfirst piclast name.
    QString display_name;
    display_name.append(ui->piclastnameLineEdit->text());
    display_name.append(QLatin1String(", "));
    display_name.append(ui->picfirstnameLineEdit->text().left(1));
    display_name.append(QLatin1Char('.'));
    newData.insert("displayname", display_name);

    using namespace experimental;

    pilotEntry.setData(newData);
    DEB("Pilot entry position: " << pilotEntry.getPosition());
    DEB("Pilot entry data: " << pilotEntry.getData());
    DB()->commit(pilotEntry);
}
