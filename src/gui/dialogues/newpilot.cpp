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

const auto FIRSTNAME_VALID = QPair<QString, QRegularExpression> {
    "picfirstnameLineEdit", QRegularExpression("[a-zA-Z]+")};
const auto LASTNAME_VALID = QPair<QString, QRegularExpression> {
    "piclastnameLineEdit", QRegularExpression("\\w+")};
const auto PHONE_VALID = QPair<QString, QRegularExpression> {
    "phoneLineEdit", QRegularExpression("^[+]{0,1}[0-9\\-\\s]+")};
const auto EMAIL_VALID = QPair<QString, QRegularExpression> {
    "emailLineEdit", QRegularExpression("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"
                                        "(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\z")};
const auto ALIAS_VALID = QPair<QString, QRegularExpression> {
    "aliasLineEdit", QRegularExpression("\\w+")};
const auto EMPLOYEENR_VALID = QPair<QString, QRegularExpression> {
    "employeeidLineEdit", QRegularExpression("\\w+")};



const auto LINE_EDIT_VALIDATORS = QVector({FIRSTNAME_VALID, LASTNAME_VALID,
                                           PHONE_VALID,     EMAIL_VALID,
                                           ALIAS_VALID,     EMPLOYEENR_VALID});

NewPilot::NewPilot(Db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    role = edRole;
    ui->setupUi(this);
    setupValidators();
}

NewPilot::NewPilot(Pilot existingEntry, Db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    oldEntry = existingEntry;
    role = edRole;
    ui->setupUi(this);
    setupValidators();
    formFiller();
    ui->piclastnameLineEdit->setFocus();
}

NewPilot::~NewPilot()
{
    delete ui;
}

void NewPilot::on_buttonBox_accepted()
{
    DEB("accepted.");
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
    for(const auto& pair : LINE_EDIT_VALIDATORS){
        auto line_edit = parent()->findChild<QLineEdit*>(pair.first);
        auto validator = new QRegularExpressionValidator(pair.second,line_edit);
        line_edit->setValidator(validator);
    }
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
    DEB("New Data: " << newData);
    DEB("Role: " << role);
    //create db object
    switch (role) {
    case Db::createNew: {
        auto newEntry = Pilot("pilots", newData);;
        DEB("New Object: ");
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
