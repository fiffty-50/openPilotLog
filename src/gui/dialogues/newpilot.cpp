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

NewPilot::NewPilot(db::editRole edRole, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    role = edRole;
    ui->setupUi(this);
}

NewPilot::NewPilot(db::editRole edRole, pilot existingEntry, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPilot)
{
    oldEntry = existingEntry;
    role = edRole;
    ui->setupUi(this);
    formFiller();
    ui->piclastnameLineEdit->setFocus();
}

NewPilot::~NewPilot()
{
    delete ui;
}

void NewPilot::on_buttonBox_accepted()
{
    DEB("aseontuh");
    if(ui->piclastnameLineEdit->text().isEmpty()){
        auto mb = new QMessageBox(this);
        mb->setText("Last Name is required.");
    }else{
        submitForm();
    }
}

void NewPilot::formFiller()
{
    DEB("Filling Form...");
    DEB(oldEntry);
    auto line_edits = parent()->findChildren<QLineEdit*>();

    for (const auto& le : line_edits) {
        QString key = le->objectName();
        key.chop(8);//remove "LineEdit"
        QString value = oldEntry.data.value(key);
        if(!value.isEmpty()){
            le->setText(value);
        }
    }
}

void NewPilot::submitForm()
{
    DEB("Creating Database Object...");
    QMap<QString,QString> newData;

    auto line_edits = parent()->findChildren<QLineEdit*>();

    for (const auto& le : line_edits) {
        QString key = le->objectName();
        key.chop(8);//remove "LineEdit"
        QString value = le->text();
        if(!key.isEmpty()){
            newData.insert(key,value);
        }
    }
    DEB("New Data: "<<newData);
    DEB("Role: "<<role);
    //create db object
    switch (role) {
    case db::createNew:{
        auto newEntry = pilot("pilots",newData);;
        DEB("New Object: ");
        newEntry.commit();
        break;}
    case db::editExisting:
        oldEntry.setData(newData);
        DEB("updated entry: "<< oldEntry);
        oldEntry.commit();
        break;
    }
}
