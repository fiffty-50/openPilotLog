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
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/database/adatabase.h"
#include "src/gui/widgets/backupwidget.h"
#include "src/database/adbsetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
#include "src/functions/adate.h"
#include "src/classes/astyle.h"
#include "src/functions/adatetime.h"
#include "src/classes/ahash.h"
#include <QErrorMessage>
#include <QFileDialog>
#include <QKeyEvent>

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->previousPushButton->setEnabled(false);
    ui->logoLabel->setPixmap(QPixmap(Opl::Assets::LOGO));

    // Approach Combo Box and Function Combo Box
    Opl::loadApproachTypes(ui->approachComboBox);
    Opl::loadPilotFunctios(ui->functionComboBox);

    // Style combo box
    const QSignalBlocker style_blocker(ui->styleComboBox);
    AStyle::loadStylesComboBox(ui->styleComboBox);
    ui->styleComboBox->setCurrentText(AStyle::defaultStyle);

    // Prepare Date Edits
    const auto date_edits = this->findChildren<QDateEdit *>();
    for (const auto &date_edit : date_edits) {
        date_edit->setDisplayFormat(ADate::getFormatString(Opl::Date::ADateFormat::ISODate));
        date_edit->setDate(QDate::currentDate());
    }
    // Debug - use ctrl + t to enable branchLineEdit to select from which git branch the templates are pulled
    ui->branchLineEdit->setVisible(false);
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    const int current_index = ui->stackedWidget->currentIndex();
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
    const int current_index = ui->stackedWidget->currentIndex();
    switch (current_index) {
    case 0:
        if(ui->firstnameLineEdit->text().isEmpty()
           || ui->lastnameLineEdit->text().isEmpty())
        {
            QMessageBox(QMessageBox::Information, tr("No name entered"),
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
        ui->nextPushButton->setDisabled(true);
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

        QMessageBox message_box(QMessageBox::Question, tr("Existing Database found"),
                                   tr("An existing database file has been detected on your system.<br>"
                                   "Would you like to create a backup of the existing database?<br><br>"
                                   "Note: if you select no, the existing database will be overwritten. This "
                                   "action is irreversible."),
                                   QMessageBox::Yes | QMessageBox::No, this);
        message_box.setDefaultButton(QMessageBox::Yes);

        if(message_box.exec() == QMessageBox::Yes) {
            // Create Backup
            const QString backup_name = BackupWidget::absoluteBackupPath();
            QFile old_db_file(database_file.absoluteFilePath());
            if (!old_db_file.copy(backup_name)) {
                WARN(tr("Unable to backup old database:<br>%1").arg(old_db_file.errorString()));
                return false;
            } else {
                INFO(tr("Backup successfully created."));
            }
        }

        //delete existing DB file
        QFile db_file(database_file.absoluteFilePath());
        if (!db_file.remove()) {
            WARN(tr("Unable to delete existing database file."));
            return false;
        }
    } // if database file exists

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

    if (!writeCurrencies()) {
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

bool FirstRunDialog::downloadTemplates(QString branch_name)
{
    // Create url string
    auto template_url_string = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/");
    template_url_string.append(branch_name);
    template_url_string.append(QLatin1String("/assets/database/templates/"));

    QDir template_dir(AStandardPaths::directory(AStandardPaths::Templates));

    const auto template_tables = aDB->getTemplateTableNames();
    // Download json files
    for (const auto& table : template_tables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        QObject::connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table + QLatin1String(".json")));
        dl->setFileName(template_dir.absoluteFilePath(table + QLatin1String(".json")));
        DEB << "Downloading: " << template_url_string + table + QLatin1String(".json");
        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table + QLatin1String(".json")));
        if (downloaded_file.size() == 0)
            return false; // ssl/network error
    }
    // Download checksum files
    for (const auto& table : template_tables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        QObject::connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table + QLatin1String(".md5")));
        dl->setFileName(template_dir.absoluteFilePath(table + QLatin1String(".md5")));

        LOG << "Downloading: " << template_url_string + table + QLatin1String(".md5");

        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table + QLatin1String(".md5")));
        if (downloaded_file.size() == 0)
            return false; // ssl/network error
    }
    // check downloadad files
    return verifyTemplates();
}

bool FirstRunDialog::verifyTemplates()
{
    const auto table_names = aDB->getTemplateTableNames();
    for (const auto &table_name : table_names) {
        const QString path = AStandardPaths::asChildOfDir(AStandardPaths::Templates, table_name);

        QFileInfo check_file(path + QLatin1String(".json"));
        AHash hash(check_file);

        QFileInfo md5_file(path + QLatin1String(".md5"));
        if (!hash.compare(md5_file))
            return false;
    }
    return true;
}

void FirstRunDialog::writeSettings()
{
    ASettings::resetToDefaults();

    ASettings::write(ASettings::FlightLogging::Function, ui->functionComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::Approach, ui->approachComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::NightLoggingEnabled, ui->nightComboBox->currentIndex());
    switch (ui->nightRulesComboBox->currentIndex()) {
    case 0:
        ASettings::write(ASettings::FlightLogging::NightAngle, -6);
        break;
    case 1:
        ASettings::write(ASettings::FlightLogging::NightAngle, 0);
        break;
    }
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, ui->prefixLineEdit->text());
    ASettings::write(ASettings::UserData::DisplaySelfAs, ui->aliasComboBox->currentIndex());
    ASettings::write(ASettings::Main::LogbookView, ui->logbookViewComboBox->currentIndex());
    ASettings::write(ASettings::Main::Style, ui->styleComboBox->currentText());
    ASettings::sync();
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
        if (!downloadTemplates(ui->branchLineEdit->text())) {
            QMessageBox message_box(this);
            message_box.setText(tr("Downloading or verifying latest data has failed.<br><br>Using local data instead."));
            message_box.exec();
            useRessourceData = true; // fall back
        }
    } else {
        useRessourceData = true;
    }

    if(!aDbSetup::createDatabase()) {
        WARN(tr("Database creation has been unsuccessful. The following error has ocurred:<br><br>%1")
             .arg(aDB->lastError.text()));
        return false;
    }


    if(!aDbSetup::importTemplateData(useRessourceData)) {
        WARN(tr("Database creation has been unsuccessful. Unable to fill template data.<br><br>%1")
             .arg(aDB->lastError.text()));
        return false;
    }

    return true;
}

bool FirstRunDialog::createUserEntry()
{
    QMap<QString, QVariant> data;
    data.insert(Opl::Db::PILOTS_LASTNAME,   ui->lastnameLineEdit->text());
    data.insert(Opl::Db::PILOTS_FIRSTNAME,  ui->firstnameLineEdit->text());
    data.insert(Opl::Db::PILOTS_ALIAS,      QStringLiteral("self"));
    data.insert(Opl::Db::PILOTS_EMPLOYEEID, ui->employeeidLineEdit->text());
    data.insert(Opl::Db::PILOTS_PHONE,      ui->phoneLineEdit->text());
    data.insert(Opl::Db::PILOTS_EMAIL,      ui->emailLineEdit->text());

    auto pilot = APilotEntry(1);
    pilot.setData(data);

    return aDB->commit(pilot);
}

bool FirstRunDialog::writeCurrencies()
{
    const QList<QPair<ACurrencyEntry::CurrencyName, QDateEdit*>> currencies_list = {
        {ACurrencyEntry::CurrencyName::Licence,     ui->currLicDateEdit},
        {ACurrencyEntry::CurrencyName::TypeRating,  ui->currTrDateEdit},
        {ACurrencyEntry::CurrencyName::LineCheck,   ui->currLckDateEdit},
        {ACurrencyEntry::CurrencyName::Medical,     ui->currMedDateEdit},
        {ACurrencyEntry::CurrencyName::Custom1,     ui->currCustom1DateEdit},
        {ACurrencyEntry::CurrencyName::Custom2,     ui->currCustom1DateEdit},
    };

    QDate today = QDate::currentDate();
    for (const auto &pair : currencies_list) {
        // only write dates that have been edited
        if (pair.second->date() != today) {
            ACurrencyEntry entry(pair.first, pair.second->date());
            if (!aDB->commit(entry))
                return false;
        }
    }
    return true;
}

void FirstRunDialog::reject()
{
    QMessageBox confirm(QMessageBox::Critical,
                               tr("Setup incomplete"),
                               tr("Without completing the initial setup "
                                  "you cannot use the application.<br><br>"
                                  "Quit anyway?"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void FirstRunDialog::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->type() == QKeyEvent::KeyPress) {
        if(keyEvent->matches(QKeySequence::AddTab)) {
            ui->branchLineEdit->setVisible(true);
            ui->branchLineEdit->setEnabled(true);
        }
    }
}

void FirstRunDialog::on_styleComboBox_currentTextChanged(const QString &new_style_setting)
{
    if (new_style_setting == QLatin1String("Dark-Palette")) {
        AStyle::setStyle(AStyle::darkPalette());
        return;
    }
    for (const auto &style_name : AStyle::styles) {
        if (new_style_setting == style_name) {
            AStyle::setStyle(style_name);
            return;
        }
    }
    for (const auto &style_sheet : AStyle::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            AStyle::setStyle(style_sheet);
            return;
        }
    }
}

void FirstRunDialog::on_currCustom1LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom1CurrencyName, ui->currCustom1LineEdit->text());
}

void FirstRunDialog::on_currCustom2LineEdit_editingFinished()
{
    ASettings::write(ASettings::UserData::Custom2CurrencyName, ui->currCustom2LineEdit->text());
}

void FirstRunDialog::on_importPushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Choose backup file"),
                QDir::homePath(),
                "*.db"
    );

    if(filename.isEmpty()) { // QFileDialog has been cancelled
        WARN(tr("No Database has been selected."));
        return;
    }

    QMessageBox confirm(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(tr("Import Database"));
    confirm.setText(tr("The following database will be imported:<br><br><b><tt>"
                       "%1<br></b></tt>"
                       "<br>Is this correct?"
                       ).arg(aDB->databaseSummaryString(filename)));
    if (confirm.exec() == QMessageBox::Yes) {
        if(!aDB->restoreBackup(filename)) {
            WARN(tr("Unable to import database file:").arg(filename));
            return;
        }
        INFO(tr("Database successfully imported."));
        QDialog::accept(); // quit the dialog as if a database was successfully created
    } else {
        return;
    }
}
