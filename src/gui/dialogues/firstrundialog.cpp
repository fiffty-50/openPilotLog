#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
#include "src/astandardpaths.h"
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

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);

    // [G]: no sure what this connects and why button is overloaded but changed it to what QtCreator suggests
    QObject::connect(themeGroup, &QButtonGroup::idClicked,
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
            QMessageBox(QMessageBox::Warning,QStringLiteral("Error"),
                             QStringLiteral("Please enter first and last name")
                             ).exec();
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
    ASettings::write(ASettings::Main::Theme, id);
}

void FirstRunDialog::finish()
{
    ASettings::write(ASettings::UserData::LastName, ui->lastnameLineEdit->text());
    ASettings::write(ASettings::UserData::FirstName, ui->firstnameLineEdit->text());
    ASettings::write(ASettings::UserData::EmployeeID, ui->employeeidLineEdit->text());
    ASettings::write(ASettings::UserData::Phone, ui->phoneLineEdit->text());
    ASettings::write(ASettings::UserData::Email, ui->emailLineEdit->text());

    ASettings::write(ASettings::FlightLogging::Function, ui->functionComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::Approach, ui->approachComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::NightLogging, ui->nightComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, ui->prefixLineEdit->text());
    ASettings::write(ASettings::FlightLogging::NumberTakeoffs, 1);
    ASettings::write(ASettings::FlightLogging::NumberLandings, 1);
    ASettings::write(ASettings::FlightLogging::PopupCalendar, true);
    ASettings::write(ASettings::FlightLogging::PilotFlying, true);

    QMap<QString, QVariant> data;
    ASettings::write(ASettings::UserData::DisplaySelfAs, ui->aliasComboBox->currentIndex());
    data.insert(ASettings::stringOfKey(ASettings::UserData::LastName), ui->lastnameLineEdit->text());
    data.insert(ASettings::stringOfKey(ASettings::UserData::FirstName), ui->firstnameLineEdit->text());
    data.insert(ASettings::stringOfKey(ASettings::UserData::Alias), "self");
    data.insert(ASettings::stringOfKey(ASettings::UserData::EmployeeID), ui->employeeidLineEdit->text());
    data.insert(ASettings::stringOfKey(ASettings::UserData::Phone), ui->phoneLineEdit->text());
    data.insert(ASettings::stringOfKey(ASettings::UserData::Email), ui->emailLineEdit->text());

    auto db_fail_msg_box = QMessageBox(QMessageBox::Critical, QStringLiteral("Database setup failed"),
                                       QStringLiteral("Errors have ocurred creating the database."
                                                      "Without a working database The application will not be usable."));
    // [G]: Im abit confused on the logic here
    // why do you write setup complete twice?
    if (!setupDatabase()) {
        db_fail_msg_box.exec();
    }
    ASettings::write(ASettings::Setup::SetupComplete, true);

    aDB()->disconnect(); // reset db connection to refresh layout after initial setup.
    aDB()->connect();

    auto pilot = APilotEntry(1);
    pilot.setData(data);
    if (aDB()->commit(pilot)) {
        ASettings::write(ASettings::Setup::SetupComplete, true);
        QDialog::accept();
    } else {
        db_fail_msg_box.exec();
        QDialog::reject();
    }
}

bool FirstRunDialog::setupDatabase()
{
    auto confirm = QMessageBox(QMessageBox::Question, QStringLiteral("Create Database"),
                               QStringLiteral("We are now going to create the database.<br>"  // [G]: Why both <br> and \n ?
                                              "Would you like to download the latest database information?"
                                              "<br>(Recommended, Internet connection required)\n"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes)
        ADataBaseSetup::downloadTemplates();

    aDB()->disconnect();
    ADataBaseSetup::backupOldData();
    aDB()->connect();

    // [F]: todo: handle unsuccessful steps
    // [G]: only two / for comments
    // three are shorthand for documentation
    if(!ADataBaseSetup::createDatabase())
        return false;
    if(!ADataBaseSetup::importDefaultData())
        return false;
    ASettings::write(ASettings::Setup::SetupComplete, true);
    return true;
}

void FirstRunDialog::reject()
{
    auto confirm = QMessageBox(QMessageBox::Critical,
                                     QStringLiteral("Setup incomplete"),
                                     QStringLiteral("Without completing the initial setup"
                                                    " you cannot use the application.<br><br>"
                                                    "Quit anyway?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     this
                                     );
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes) {
        DEB << "rejected.";
        QDialog::reject();
        }
}
