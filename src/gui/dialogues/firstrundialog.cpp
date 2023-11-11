/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "src/database/previousexperienceentry.h"
#include "ui_firstrundialog.h"
#include "src/opl.h"
#include "src/database/database.h"
#include "src/database/dbsummary.h"
#include "src/gui/widgets/backupwidget.h"
#include "src/database/row.h"
#include "src/classes/downloadhelper.h"
#include "src/classes/settings.h"
#include "src/functions/datetime.h"
#include "src/classes/style.h"
#include "src/classes/md5sum.h"
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
    ui->logoLabel->setPixmap(QPixmap(OPL::Assets::LOGO));

    // Approach Combo Box and Function Combo Box
    OPL::GLOBALS->loadApproachTypes(ui->approachComboBox);
    OPL::GLOBALS->loadPilotFunctios(ui->functionComboBox);
    OPL::GLOBALS->fillViewNamesComboBox(ui->logbookViewComboBox);


    // Style combo box
    const QSignalBlocker style_blocker(ui->styleComboBox);
    OPL::Style::loadStylesComboBox(ui->styleComboBox);
    ui->styleComboBox->setCurrentText(OPL::Style::defaultStyle);

    // Prepare Date Edits
    const auto date_edits = this->findChildren<QDateEdit *>();
    for (const auto &date_edit : date_edits) {
        date_edit->setDisplayFormat(OPL::DateTime::getFormatString(OPL::DateFormat::ISODate));
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
    QFileInfo database_file(OPL::Paths::databaseFileInfo());

    if (database_file.exists() && database_file.size() != 0) {

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

    if (!DB->connect()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed (connect)"),
                                tr("Errors have ocurred creating the database."
                                   "Without a working database The application will not be usable.<br>"
                                   "The following error has ocurred:<br>"
                                   "Database: Unable to connect"));
        message_box.exec();
        return false;
    }

    if (!setupDatabase()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed (create schema)"),
                                tr("Errors have ocurred creating the database."
                                   "Without a working database The application will not be usable.<br>"
                                   "The following error has ocurred:<br>%1"
                                   ).arg(DB->lastError.text()));
        message_box.exec();
        return false;
    }

    if (!createUserEntry()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed (user entry)"),
                                tr("Unable to execute database query<br>"
                                   "The following error has occured:<br>%1"
                                   ).arg(DB->lastError.text()));
        message_box.exec();
        return false;
    }

    if (!setupPreviousExperienceEntry()) {
        QMessageBox message_box(QMessageBox::Critical, tr("Database setup failed (previous Experience)"),
                                tr("Unable to execute database query<br>"
                                   "The following error has occured:<br>%1"
                                   ).arg(DB->lastError.text()));
        message_box.exec();
        return false;
    }

    // non-critical error
    if(!writeCurrencies())
        LOG << "Error writing currencies during initial setup.";

    DB->disconnect(); // Connection will be re-established by MainWindow
    return true;
}

bool FirstRunDialog::downloadTemplates(QString branch_name)
{
    // Create url string
    auto template_url_string = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/");
    template_url_string.append(branch_name);
    template_url_string.append(QLatin1String("/assets/database/templates/"));

    QDir template_dir(OPL::Paths::directory(OPL::Paths::Templates));

    QStringList template_table_names;
    for (const auto table : DB->getTemplateTables())
        template_table_names.append(OPL::GLOBALS->getDbTableName(table));

    // Download json files
    for (const auto& table_name : template_table_names) {
        QEventLoop loop;
        DownloadHelper* dl = new DownloadHelper;
        QObject::connect(dl, &DownloadHelper::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table_name + QLatin1String(".json")));
        dl->setFileName(template_dir.absoluteFilePath(table_name + QLatin1String(".json")));
        DEB << "Downloading: " << template_url_string + table_name + QLatin1String(".json");
        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table_name + QLatin1String(".json")));
        if (downloaded_file.size() == 0) {
            LOG << "Unable to download template files (SSL / Network Error)";
            return false; // ssl/network error
        }
    }
    // Download checksum files
    for (const auto& table_name : template_table_names) {
        QEventLoop loop;
        DownloadHelper* dl = new DownloadHelper;
        QObject::connect(dl, &DownloadHelper::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(template_url_string + table_name + QLatin1String(".md5")));
        dl->setFileName(template_dir.absoluteFilePath(table_name + QLatin1String(".md5")));

        LOG << "Downloading: " << template_url_string + table_name + QLatin1String(".md5");

        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table_name + QLatin1String(".md5")));
        if (downloaded_file.size() == 0) {
            LOG << "Unable to download checksum files (SSL / Network Error)";
            return false; // ssl/network error
        }
    }
    // check downloadad files
    return verifyTemplates();
}

bool FirstRunDialog::verifyTemplates()
{QStringList template_table_names;
    for (const auto table : DB->getTemplateTables())
        template_table_names.append(OPL::GLOBALS->getDbTableName(table));
    for (const auto &table_name : template_table_names) {
        const QString path = OPL::Paths::filePath(OPL::Paths::Templates, table_name);

        QFileInfo check_file(path + QLatin1String(".json"));
        Md5Sum hash(check_file);

        QFileInfo md5_file(path + QLatin1String(".md5"));
        if (!hash.compare(md5_file))
            return false;
    }
    return true;
}

void FirstRunDialog::writeSettings()
{
    Settings::resetToDefaults();
    Settings::setPilotFunction(OPL::PilotFunction(ui->functionComboBox->currentIndex()));
    Settings::setApproachType(ui->approachComboBox->currentText());
    Settings::setNightLoggingEnabled(ui->nightComboBox->currentIndex());
    Settings::setLogIfr(ui->rulesComboBox->currentIndex());
    Settings::setFlightNumberPrefix(ui->prefixLineEdit->text());
    Settings::setLogbookView(OPL::LogbookView(ui->logbookViewComboBox->currentIndex()));
    Settings::setApplicationStyle(ui->styleComboBox->currentText());

    switch (ui->nightRulesComboBox->currentIndex()) {
    case 0:
        Settings::setNightAngle(-6);
        break;
    case 1:
        Settings::setNightAngle(0);
        break;
    }

    Settings::setShowSelfAs(ui->aliasComboBox->currentIndex());
    Settings::sync();
}

bool FirstRunDialog::setupDatabase()
{
    QMessageBox confirm(QMessageBox::Question, tr("Create Database"),
                               tr("We are now going to create the database.<br>"
                                              "Would you like to download the latest database information?"
                                              "<br>(Recommended, Internet connection required)"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::Yes);

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

    if(!DB->createSchema()) {
        WARN(tr("Database creation has been unsuccessful. The following error has ocurred:<br><br>%1<br><br>%2")
             .arg(FUNC_IDENT, DB->lastError.text()));
        return false;
    }

    if(!DB->importTemplateData(useRessourceData)) {
        WARN(tr("Database creation has been unsuccessful. Unable to fill template data.<br><br>%1<br><br>%2")
             .arg(FUNC_IDENT, DB->lastError.text()));
        return false;
    }
    return true;
}

bool FirstRunDialog::createUserEntry()
{
    OPL::RowData_T data;
    data.insert(OPL::PilotEntry::LASTNAME,   ui->lastnameLineEdit->text());
    data.insert(OPL::PilotEntry::FIRSTNAME,  ui->firstnameLineEdit->text());
    data.insert(OPL::PilotEntry::ALIAS,      QStringLiteral("self"));
    data.insert(OPL::PilotEntry::EMPLOYEEID, ui->employeeidLineEdit->text());
    data.insert(OPL::PilotEntry::PHONE,      ui->phoneLineEdit->text());
    data.insert(OPL::PilotEntry::EMAIL,      ui->emailLineEdit->text());

    return DB->setLogbookOwner(data);
}

bool FirstRunDialog::setupPreviousExperienceEntry()
{
    OPL::RowData_T prevData;
    prevData.insert(OPL::PreviousExperienceEntry::TBLK, 0);
    auto pXpEntry = OPL::PreviousExperienceEntry(1, prevData);
    return DB->commit(pXpEntry);
}

bool FirstRunDialog::writeCurrencies()
{
    const QList<QPair<QString, QDateEdit*>> currencies = {
        { ui->currLicLabel->text(), 		ui->currLicDateEdit },
        { ui->currTrLabel->text(), 			ui->currTrDateEdit },
        { ui->currLckLabel->text(), 		ui->currLckDateEdit },
        { ui->currMedLabel->text(), 		ui->currMedDateEdit },
        { ui->currCustom1LineEdit->text(), 	ui->currCustom1DateEdit },
        { ui->currCustom2LineEdit->text(), 	ui->currCustom2DateEdit },
    };

    const QDate today = QDate::currentDate();

    for(const auto &pair : currencies) {
        // list 0-indexed, db row indexes start at 1
        OPL::CurrencyEntry currencyEntry = OPL::CurrencyEntry(currencies.indexOf(pair) + 1, OPL::RowData_T());

        currencyEntry.setName(pair.first);

        // only set expiry date if user has modified it
        const QDate date = pair.second->date();
        if(date != today) {
            int julianDay = date.toJulianDay();
            currencyEntry.setExpiryDate(OPL::Date(julianDay, m_format));
        }

        if(!DB->commit(currencyEntry))
            return false;
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
        OPL::Style::setStyle(OPL::Style::darkPalette());
        return;
    }
    for (const auto &style_name : OPL::Style::styles) {
        if (new_style_setting == style_name) {
            OPL::Style::setStyle(style_name);
            return;
        }
    }
    for (const auto &style_sheet : OPL::Style::styleSheets) {
        if (new_style_setting == style_sheet.styleSheetName) {
            OPL::Style::setStyle(style_sheet);
            return;
        }
    }
}

void FirstRunDialog::on_importPushButton_clicked()
{
    QString filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(
                this,
                tr("Choose backup file"),
                QDir::homePath(),
                "*.db"));

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
                       ).arg(OPL::DbSummary::summaryString(filename)));
    if (confirm.exec() == QMessageBox::Yes) {
        if(!DB->restoreBackup(QDir::toNativeSeparators(filename))) {
            WARN(tr("Unable to import database file:").arg(filename));
            return;
        }
        INFO(tr("Database successfully imported."));
        QDialog::accept(); // quit the dialog as if a database was successfully created
    } else {
        return;
    }
}
