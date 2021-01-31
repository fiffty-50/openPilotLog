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
#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
#include "src/oplconstants.h"
#include <QErrorMessage>

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->previousPushButton->setEnabled(false);
    ui->nightComboBox->setCurrentIndex(1);

    for (const auto &approach : Opl::ApproachTypes){
        ui->approachComboBox->addItem(approach);
    }
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    auto current_idx = ui->stackedWidget->currentIndex();
    switch (current_idx) {
    case 0:
        return;
    case 1:
        ui->previousPushButton->setEnabled(false);
        break;
    case 2:
        ui->nextPushButton->setText(tr("Next"));
        break;
    }
    ui->stackedWidget->setCurrentIndex(current_idx - 1);

}

void FirstRunDialog::on_nextPushButton_clicked()
{
    auto current_idx = ui->stackedWidget->currentIndex();
    switch (current_idx) {
    case 0:
        if(ui->firstnameLineEdit->text().isEmpty()
           || ui->lastnameLineEdit->text().isEmpty())
        {
            QMessageBox(QMessageBox::Warning, tr("Error"),
                        tr("Please enter first and last name")
                        ).exec();
            return;
        }
        ui->previousPushButton->setEnabled(true);
        break;
    case 1:
        ui->nextPushButton->setText(tr("Done"));
        break;
    case 2:
        if(!finishSetup())
            QDialog::reject();
        else
            QDialog::accept();
        return;
    }
    ui->stackedWidget->setCurrentIndex(current_idx + 1);
}

bool FirstRunDialog::finishSetup()
{
    writeSettings();

    if (!setupDatabase()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed"),
                                tr("Errors have ocurred creating the database."
                                   "Without a working database The application will not be usable.<br>"
                                   "The following error has ocurred:<br>%1"
                                   ).arg(aDB->lastError.text()));
        message_box.exec();
        return false;
    }

    if (!createUserEntry()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed"),
                                tr("Unable to execute database query<br>"
                                   "The following error has occured:<br>%1"
                                   ).arg(aDB->lastError.text()));
        message_box.exec();
        return false;
    }
    return true;
}

void FirstRunDialog::writeSettings()
{
    ASettings::write(ASettings::FlightLogging::Function, ui->functionComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::Approach, ui->approachComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::NightLogging, ui->nightComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, ui->prefixLineEdit->text());
    ASettings::write(ASettings::FlightLogging::NumberTakeoffs, 1);
    ASettings::write(ASettings::FlightLogging::NumberLandings, 1);
    ASettings::write(ASettings::FlightLogging::PopupCalendar, true);
    ASettings::write(ASettings::FlightLogging::PilotFlying, true);
    ASettings::write(ASettings::FlightLogging::FlightTimeFormat, Opl::Time::Default);
    ASettings::write(ASettings::Main::UseSystemFont, true);
    ASettings::write(ASettings::UserData::DisplaySelfAs, ui->aliasComboBox->currentIndex());
}

bool FirstRunDialog::setupDatabase()
{
    QMessageBox confirm(QMessageBox::Question, tr("Create Database"),
                               tr("We are now going to create the database.<br>"
                                              "Would you like to download the latest database information?"
                                              "<br>(Recommended, Internet connection required)"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes) {
        useLocalTemplates = false;
        if (!ADataBaseSetup::downloadTemplates()) { // To do: return true only if size of dl != 0
            QMessageBox message_box(this);
            message_box.setText(tr("Downloading latest data has failed.<br><br>Using local data instead."));
            message_box.exec();
            useLocalTemplates = true; // fall back
        } else {
            useLocalTemplates = true;
        }
    }

    aDB->disconnect();
    ADataBaseSetup::backupOldData();
    aDB->connect();

    // [F]: todo: handle unsuccessful steps
    if(!ADataBaseSetup::createDatabase())
        return false;

    aDB->updateLayout();

    if(!ADataBaseSetup::importDefaultData(useLocalTemplates))
        return false;
    aDB->updateLayout();
    return true;
}

bool FirstRunDialog::createUserEntry()
{
    QMap<QString, QVariant> data;
    data.insert(Opl::Db::PILOTS_LASTNAME, ui->lastnameLineEdit->text());
    data.insert(Opl::Db::PILOTS_FIRSTNAME, ui->firstnameLineEdit->text());
    data.insert(Opl::Db::PILOTS_ALIAS, QStringLiteral("self"));
    data.insert(Opl::Db::PILOTS_EMPLOYEEID, ui->employeeidLineEdit->text());
    data.insert(Opl::Db::PILOTS_PHONE, ui->phoneLineEdit->text());
    data.insert(Opl::Db::PILOTS_EMAIL, ui->emailLineEdit->text());

    auto pilot = APilotEntry(1);
    pilot.setData(data);

    return aDB->commit(pilot);
}

void FirstRunDialog::reject()
{
    QMessageBox confirm(QMessageBox::Critical,
                               tr("Setup incomplete"),
                               tr("Without completing the initial setup"
                                              " you cannot use the application.<br><br>"
                                              "Quit anyway?"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes) {
        DEB << "rejected.";
        QDialog::reject();
    }
}
