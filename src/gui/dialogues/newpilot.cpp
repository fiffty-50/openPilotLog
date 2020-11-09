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
#include "newpilot.h"
#include "ui_newpilot.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

static const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    "picfirstnameLineEdit", QRegularExpression("[a-zA-Z]+")};
static const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    "piclastnameLineEdit", QRegularExpression("\\w+")};
static const auto PHONE_VALID = QPair<QString, QRegularExpression> {
    "phoneLineEdit", QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
static const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
    "emailLineEdit", QRegularExpression("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                                        "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
static const auto COMPANY_VALID = QPair<QString, QRegularExpression> {
    "companyLineEdit", QRegularExpression("\\w+")};
static const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
    "employeeidLineEdit", QRegularExpression("\\w+")};


static const auto LINE_EDIT_VALIDATORS = QVector({FIRSTNAME_VALID, LASTNAME_VALID,
                                           PHONE_VALID,     EMAIL_VALID,
                                           COMPANY_VALID,     EMPLOYEENR_VALID});
// For creating a new entry
NewPilot::NewPilot(Db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    role = edRole;
    ui->setupUi(this);
    setupValidators();
    setupCompleter();
}
// For editing an existing entry
NewPilot::NewPilot(Pilot existingEntry, Db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    oldEntry = existingEntry;
    role = edRole;
    ui->setupUi(this);
    setupValidators();
    setupCompleter();

    formFiller();
    ui->piclastnameLineEdit->setFocus();
}

NewPilot::~NewPilot()
{
    delete ui;
}

void NewPilot::on_buttonBox_accepted()
{
    if (ui->piclastnameLineEdit->text().isEmpty() || ui->picfirstnameLineEdit->text().isEmpty()) {
        auto mb = new QMessageBox(this);
        mb->setText("Last Name and First Name are required.");
        mb->show();
    } else {
        submitForm();
        accept();
    }
}

void NewPilot::setupValidators()
{
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
}

void NewPilot::setupCompleter()
{
    DEB("Setting up completer...");

    auto companies = new CompletionList(CompleterTarget::companies);
    auto completer = new QCompleter(companies->list, ui->companyLineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    completer->setCaseSensitivity(Qt::CaseSensitive);

    ui->companyLineEdit->setCompleter(completer);
}

void NewPilot::formFiller()
{
    DEB("Filling Form...");
    DEB(oldEntry);
    auto line_edits = parent()->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        QString key = le->objectName();
        key.chop(8);//remove "LineEdit"
        QString value = oldEntry.data.value(key);
        if (!value.isEmpty()) {
            le->setText(value);
        }
    }
}

void NewPilot::submitForm()
{
    DEB("Creating Database Object...");
    QMap<QString, QString> newData;

    auto line_edits = parent()->findChildren<QLineEdit *>();

    for (const auto &le : line_edits) {
        QString key = le->objectName();
        key.chop(8);//remove "LineEdit"
        QString value = le->text();
        if (!key.isEmpty()) {
            newData.insert(key, value);
        }
    }
    QString displayName;
    displayName.append(ui->piclastnameLineEdit->text());
    displayName.append(QLatin1String(", "));
    displayName.append(ui->picfirstnameLineEdit->text().left(1));
    displayName.append(QLatin1Char('.'));
    newData.insert("displayname",displayName);
    //create db object
    switch (role) {
    case Db::createNew: {
        auto newEntry = Pilot(newData);;
        DEB("New Object: " << newEntry);
        newEntry.commit();
        break;
    }
    case Db::editExisting:
        oldEntry.setData(newData);
        DEB("updated entry: " << oldEntry);
        oldEntry.commit();
        break;
    }
}
