#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
#include "src/astandardpaths.h"

static inline
void prompt_error_box(QString title, QString text, QWidget* parent = nullptr)
{
    QMessageBox(QMessageBox::Warning, title, text, QMessageBox::Ok, parent).exec();
}

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->previousPushButton->setEnabled(false);
    ui->nightComboBox->setCurrentIndex(1);

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);

    QObject::connect(themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                     this, &FirstRunDialog::on_themeGroup_toggled);
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
        ui->nextPushButton->setText(QStringLiteral("Next"));
        break;
    }
    ui->stackedWidget->setCurrentIndex(current_idx - 1);

}

void FirstRunDialog::on_nextPushButton_clicked()
{
    auto current_idx = ui->stackedWidget->currentIndex();
    // [G]: per index do appropriate error handling
    switch (current_idx) {
    case 0:
        if(ui->firstnameLineEdit->text().isEmpty()
           || ui->lastnameLineEdit->text().isEmpty())
        {
            prompt_error_box(QStringLiteral("Error"),
                             QStringLiteral("Please enter first and last name"));
            return;
        }
        ui->previousPushButton->setEnabled(true);
        break;
    case 1:
        ui->nextPushButton->setText(QStringLiteral("Done"));
        break;
    case 2:
        finish();
        return;
    }
    ui->stackedWidget->setCurrentIndex(current_idx + 1);
}

void FirstRunDialog::on_themeGroup_toggled(int id)
{
    ASettings::write(QStringLiteral("main/theme"), id);
}

void FirstRunDialog::finish()
{
    if(ui->lastnameLineEdit->text().isEmpty() || ui->firstnameLineEdit->text().isEmpty()){
        auto mb = QMessageBox(this);
        mb.setText(QStringLiteral("You have to enter a valid first and last name for the logbook."));
        mb.show();
    } else {
        ASettings::write("userdata/lastname", ui->lastnameLineEdit->text());
        ASettings::write(QStringLiteral("userdata/firstname"), ui->firstnameLineEdit->text());
        ASettings::write(QStringLiteral("userdata/employeeid"), ui->employeeidLineEdit->text());
        ASettings::write(QStringLiteral("userdata/phone"), ui->phoneLineEdit->text());
        ASettings::write(QStringLiteral("userdata/email"), ui->emailLineEdit->text());

        ASettings::write(QStringLiteral("flightlogging/function"), ui->functionComboBox->currentText());
        ASettings::write(QStringLiteral("flightlogging/approach"), ui->approachComboBox->currentText());
        ASettings::write(QStringLiteral("flightlogging/nightlogging"), ui->nightComboBox->currentIndex());
        ASettings::write(QStringLiteral("flightlogging/logIfr"), ui->rulesComboBox->currentIndex());
        ASettings::write(QStringLiteral("flightlogging/flightnumberPrefix"), ui->prefixLineEdit->text());

        ASettings::write(QStringLiteral("flightlogging/numberTakeoffs"), 1);
        ASettings::write(QStringLiteral("flightlogging/numberLandings"), 1);
        ASettings::write(QStringLiteral("flightlogging/popupCalendar"), true);
        ASettings::write(QStringLiteral("flightlogging/pilotFlying"), true);


        QMap<QString, QVariant> data;
        switch (ui->aliasComboBox->currentIndex()) {
        case 0:
            ASettings::write(QStringLiteral("userdata/displayselfas"), ui->aliasComboBox->currentIndex());
            break;
        case 1:
            ASettings::write(QStringLiteral("userdata/displayselfas"), ui->aliasComboBox->currentIndex());
            break;
        case 2:
            ASettings::write(QStringLiteral("userdata/displayselfas"), ui->aliasComboBox->currentIndex());
            break;
        default:
            break;
        }
        data.insert(QStringLiteral("lastname"), ui->lastnameLineEdit->text());
        data.insert(QStringLiteral("firstname"), ui->firstnameLineEdit->text());
        data.insert(QStringLiteral("alias"), QStringLiteral("self"));
        data.insert(QStringLiteral("employeeid"), ui->employeeidLineEdit->text());
        data.insert(QStringLiteral("phone"), ui->phoneLineEdit->text());
        data.insert(QStringLiteral("email"), ui->emailLineEdit->text());

        if (!setupDatabase()) {
            QMessageBox message_box(this);
            message_box.setText(QStringLiteral("Errors have ocurred creating the database. "
                                               "Without a working database The application will "
                                               "not be usable."));
        }
        ASettings::write(QStringLiteral("setup/setup_complete"), true);
        aDB()->disconnect(); // reset db connection to refresh layout after initial setup.
        aDB()->connect();
        auto pilot = APilotEntry(1);
        pilot.setData(data);
        if (aDB()->commit(pilot)) {
            QDialog::accept();
        } else {
            QMessageBox message_box(this);
            message_box.setText(QStringLiteral("Errors have ocurred creating the database. "
                                               "Without a working database The application will "
                                               "not be usable."));
        }
    }
}

bool FirstRunDialog::setupDatabase()
{
    //check if template dir exists and create if needed.

    QMessageBox confirm;
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle(QStringLiteral("Create Database"));
    confirm.setText(QStringLiteral("We are now going to create the database. "
                                   "Would you like to download the latest database information?\n"
                                   "(Recommended, Internet connection required)\n"));
    int reply = confirm.exec();
    if (reply == QMessageBox::Yes) {// to do: else use local copy that shipped with the release
        //close database connection
        aDB()->disconnect();
        // back up old database if exists
        auto oldDatabase = QFile(AStandardPaths::getPath(QStandardPaths::AppDataLocation)
                                 % QStringLiteral("/logbook.db"));

        if (oldDatabase.exists()) {
            auto dateString = QDateTime::currentDateTime().toString(Qt::ISODate);
            DEB << "Backing up old database as: " << "logbook-backup-" + dateString;
            if (!oldDatabase.rename(AStandardPaths::getPath(QStandardPaths::AppDataLocation)
                                    % QStringLiteral("/logbook-backup-") % dateString
                                    % QStringLiteral(".db"))) {
                DEB << "Warning: Creating backup of old database has failed.";
            }
        }
        // re-connect and create new database
        aDB()->connect();

        if(! ADataBaseSetup::createDatabase()) {
            DEB << "Error creating Database";
            return false;
        }
        if(! ADataBaseSetup::setupTemplateDir()) {
                    DEB << "Error setting up template dir";
                    return false;
                }
        if(! ADataBaseSetup::downloadTemplates()) {
                    DEB << "Download Error";
                    return false;
                }
        if(! ADataBaseSetup::importDefaultData()) {
                    DEB << "Error importing default data";
                    return false;
                }
        ASettings::write(QStringLiteral("setup/setup_complete"), true);
    }
    return true;
}

void FirstRunDialog::reject()
{


    auto confirm = QMessageBox(this);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle("Delete Flight");
    confirm.setText(QStringLiteral("Without completing the initial setup"
                                   " you cannot use the application.<br><br>"
                                   "Quit anyway?"));
    if (confirm.exec() == QMessageBox::Yes) {
        DEB << "rejected.";
        QDialog::reject();
        }
}
