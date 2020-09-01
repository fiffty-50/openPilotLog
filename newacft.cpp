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
#include "newacft.h"
#include "ui_newacft.h"
#include "dbman.cpp"
#include "showaircraftlist.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel> // test

QString registration = "invalid";
QString make = "invalid";
QString model = "invalid";
QString variant = "invalid";
QString aircraft_id = "0";


NewAcft::NewAcft(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAcft)
{
    ui->setupUi(this);
}

NewAcft::~NewAcft()
{
    delete ui;
}





void NewAcft::on_MakeLineEdit_textEdited(const QString &arg1)
{
    QStringList makeList = dbAircraft::retreiveAircraftMake(arg1);
    makeList.removeDuplicates();
    QCompleter *makeCompleter = new QCompleter(makeList, this);
    makeCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    makeCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->MakeLineEdit->setCompleter(makeCompleter);

}
void NewAcft::on_MakeLineEdit_editingFinished()
{
    QStringList makeList = dbAircraft::retreiveAircraftMake(ui->MakeLineEdit->text());
    if(makeList.length() != 0)
    {
        make = makeList.first();
        ui->MakeLineEdit->setText(make);
    }else
    {
        QMessageBox msgBox;
        msgBox.setText("No Airplane Maker with that name found.");
        msgBox.exec();
        ui->MakeLineEdit->setText("");
    }
}


void NewAcft::on_ModelLineEdit_textEdited(const QString &arg1)
{
    QStringList modelList = dbAircraft::retreiveAircraftModel(make, arg1);
    modelList.removeDuplicates();
    QCompleter *modelCompleter = new QCompleter(modelList, this);
    modelCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    modelCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->ModelLineEdit->setCompleter(modelCompleter);
}
void NewAcft::on_ModelLineEdit_editingFinished()
{
    QStringList modelList = dbAircraft::retreiveAircraftModel(make, ui->ModelLineEdit->text());
    if(modelList.length() != 0)
    {
        model = modelList.first();
        ui->ModelLineEdit->setText(model);
    }else
    {
        QMessageBox msgBox;
        msgBox.setText("No Model (Type) with that name found.");
        msgBox.exec();
        ui->ModelLineEdit->setText("");
    }
}

void NewAcft::on_VariantLineEdit_textEdited(const QString &arg1)
{
    QStringList variantList = dbAircraft::retreiveAircraftVariant(make, model, arg1);
    variantList.removeDuplicates();
    QCompleter *variantCompleter = new QCompleter(variantList, this);
    variantCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    variantCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->VariantLineEdit->setCompleter(variantCompleter);
}

void NewAcft::on_VariantLineEdit_editingFinished()
{
    QStringList VariantList = dbAircraft::retreiveAircraftVariant(make, model, ui->VariantLineEdit->text());
    if(VariantList.length() != 0)
    {
        variant = VariantList.first();
        ui->VariantLineEdit->setText(variant);
    }else
    {
        QMessageBox msgBox;
        msgBox.setText("No Variant found. Are you sure you want to proceed?");
        msgBox.exec();
        ui->VariantLineEdit->setText("");
    }
}

void NewAcft::on_buttonBox_accepted()
{
    qDebug() << "Accepted Button pressed";
    aircraft_id = dbAircraft::retreiveAircraftIdFromMakeModelVariant(make, model, variant);
    if(aircraft_id.contains("0") && aircraft_id.length() < 2)
    {
        QMessageBox nope;
        nope.setText("EASA FCL.050 requires Pilots to record details of:\n\n"
                     "Make\t e.g. Boeing\nModel\t e.g. 737\nVariant\t e.g. 800\n\nRegistration\n\n"
                     "Please check or edit the aircraft database.");
        nope.exec();
    }else
    {
        dbAircraft::commitTailToDb(registration, aircraft_id, "");
        NewAcft::reject();
    }

}

void NewAcft::on_buttonBox_rejected()
{
    make = "xxx";
    model = "xxx";
    variant = "xxx";
    aircraft_id = "0";
    NewAcft::reject();
}


void NewAcft::on_VerifyButton_clicked()
{
    if(ui->EasaEnabledCheckBox->isChecked())
    {
        QString checkstring = "[ " + registration + " ]    " + make + " " + model + "-" + variant;
        ui->VerifyLineEdit->setText(checkstring);
    }else
    {
        ui->VerifyLineEdit->setText("EASA FCL.050 compliance checks disabled.");
    }



}

void NewAcft::on_RegistrationLineEdit_editingFinished()
{
    registration = ui->RegistrationLineEdit->text();
}

void NewAcft::on_EasaEnabledCheckBox_stateChanged()
{
    QMessageBox nope;
    nope.setText("Data Input without Format checking may corrupt the database.");
    nope.exec();
}

void NewAcft::on_showAllPushButton_clicked()
{
    ShowAircraftList sa(this);
    sa.exec();
}
