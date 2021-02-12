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
#include "src/classes/astyle.h"

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->previousPushButton->setEnabled(false);
    ui->logoLabel->setPixmap(QPixmap(Opl::Assets::LOGO));

    // approach Combo Box
    for (const auto &approach : Opl::ApproachTypes){
        ui->approachComboBox->addItem(approach);
    }
    // Style combo box
    const QSignalBlocker blocker_style(ui->styleComboBox);
    ui->styleComboBox->addItems(AStyle::styles);
    for (const auto &style_sheet : AStyle::styleSheets) {
        ui->styleComboBox->addItem(style_sheet.styleSheetName);
    }
    ui->styleComboBox->addItem(QStringLiteral("Dark-Palette"));
    ui->styleComboBox->model()->sort(0);
    ui->styleComboBox->setCurrentText(AStyle::defaultStyle);
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    auto current_index = ui->stackedWidget->currentIndex();
    switch (current_index) {
    case 0:
        return;
    case 1:
        ui->previousPushButton->setEnabled(false);
        break;
    case 2:
        ui->nextPushButton->setText(tr("Next"));
        break;
    }
    ui->stackedWidget->setCurrentIndex(current_index - 1);

}

void FirstRunDialog::on_nextPushButton_clicked()
{
    auto current_index = ui->stackedWidget->currentIndex();
    switch (current_index) {
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
    case 3:
        ui->nextPushButton->setText(tr("Done"));
        break;
    case 4:
        if(!finishSetup())
            QDialog::reject();
        else
            QDialog::accept();
        return;
    }
    ui->stackedWidget->setCurrentIndex(current_index + 1);
}

bool FirstRunDialog::finishSetup()
{
    writeSettings();

    QFileInfo database_file(AStandardPaths::directory(AStandardPaths::Database).
                                 absoluteFilePath(QStringLiteral("logbook.db")));
    if (database_file.exists()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database found"),
                                tr("Warning."
                                   "An existing database file has been detected on your system.<br>"
                                   "A backup copy of the existing database will be created at this location:<br>"
                                   "%1").arg(
                                    QDir::cleanPath(AStandardPaths::directory(AStandardPaths::Backup).canonicalPath())));
        message_box.exec();
        ADataBaseSetup::backupOldData();
    }
    if (!aDB->connect()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed"),
                                tr("Errors have ocurred creating the database."
                                   "Without a working database The application will not be usable.<br>"
                                   "The following error has ocurred:<br>"
                                   "Database: Unable to connect"));
        message_box.exec();
        return false;
    }

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
    aDB->disconnect(); // connection will be re-established by main()
    return true;
}

void FirstRunDialog::writeSettings()
{
    ASettings::resetToDefaults();
    ASettings::write(ASettings::FlightLogging::Function, ui->functionComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::Approach, ui->approachComboBox->currentIndex());
    switch (ui->nightComboBox->currentIndex()) {
    case 0:
        ASettings::write(ASettings::FlightLogging::NightLoggingEnabled, true);
        break;
    case 1:
        ASettings::write(ASettings::FlightLogging::NightLoggingEnabled, false);
        break;
    default:
        ASettings::write(ASettings::FlightLogging::NightLoggingEnabled, true);
        break;
    }
    switch (ui->nightRulesComboBox->currentIndex()) {
    case 0:
        ASettings::write(ASettings::FlightLogging::NightAngle, 6);
        break;
    case 1:
        ASettings::write(ASettings::FlightLogging::NightAngle, 0);
        break;

    }
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, ui->prefixLineEdit->text());
    ASettings::write(ASettings::FlightLogging::FlightTimeFormat, Opl::Time::Default);
    ASettings::write(ASettings::UserData::DisplaySelfAs, ui->aliasComboBox->currentIndex());
    ASettings::write(ASettings::Main::LogbookView, ui->logbookViewComboBox->currentIndex());

    switch (ui->currWarningCheckBox->checkState()) {
    case Qt::CheckState::Checked:
        ASettings::write(ASettings::UserData::CurrWarningEnabled, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::CurrWarningEnabled, false);
        break;
    default:
        break;
    }
    ASettings::write(ASettings::UserData::CurrWarningThreshold, ui->currWarningThresholdSpinBox->value());
    ASettings::write(ASettings::Main::Style, ui->styleComboBox->currentText());
    QSettings settings;
    settings.sync();
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
        useRessourceData = false;
        if (!ADataBaseSetup::downloadTemplates()) {
            QMessageBox message_box(this);
            message_box.setText(tr("Downloading latest data has failed.<br><br>Using local data instead."));
            message_box.exec();
            useRessourceData = true; // fall back
        }
    } else {
        useRessourceData = true;
    }

    // [F]: todo: handle unsuccessful steps
    if(!ADataBaseSetup::createDatabase())
        return false;

    aDB->updateLayout();

    if(!ADataBaseSetup::importDefaultData(useRessourceData))
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

void FirstRunDialog::on_styleComboBox_currentTextChanged(const QString &new_style_setting)
{
    DEB << "style selected:"<<new_style_setting;
    if (new_style_setting == QLatin1String("Dark-Palette")) {
        //DEB << "Palette";
        AStyle::setStyle(AStyle::darkPalette());
        return;
    }
    for (const auto &style_name : AStyle::styles) {
        if (new_style_setting == style_name) {
            //DEB << "style";
            AStyle::setStyle(style_name);
            return;
        }
    }
    for (const auto &style_sheet : AStyle::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            //DEB << "stylesheet";
            AStyle::setStyle(style_sheet);
            return;
        }
    }
}

void FirstRunDialog::on_currWarningCheckBox_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        ASettings::write(ASettings::UserData::CurrWarningEnabled, true);
        break;
    case Qt::CheckState::Unchecked:
        ASettings::write(ASettings::UserData::CurrWarningEnabled, false);
        break;
    default:
        break;
    }
    ASettings::write(ASettings::UserData::CurrWarningThreshold, arg1);
}

void FirstRunDialog::on_currWarningThresholdSpinBox_valueChanged(int arg1)
{
    ASettings::write(ASettings::UserData::CurrWarningThreshold, arg1);
}
// [F:] To Do - WIP
/*
void FirstRunDialog::on_currLicDateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::LicCurrencyDate, date);
}

void FirstRunDialog::on_currTrDateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::TrCurrencyDate, date);
}

void FirstRunDialog::on_currLckDateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::LckCurrencyDate, date);
}

void FirstRunDialog::on_currMedDateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::MedCurrencyDate, date);
}

void FirstRunDialog::on_currCustom1DateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::Custom1CurrencyDate, date);
}

void FirstRunDialog::on_currCustom2DateEdit_userDateChanged(const QDate &date)
{
    ASettings::write(ASettings::UserData::Custom2CurrencyDate, date);
}
*/

void FirstRunDialog::on_currCustom1LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom1CurrencyName, ui->currCustom1LineEdit->text());
}

void FirstRunDialog::on_currCustom2LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom2CurrencyName, ui->currCustom2LineEdit->text());
}
