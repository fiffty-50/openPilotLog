#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
#include "src/classes/astandardpaths.h"
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

//    auto *themeGroup = new QButtonGroup;
//    themeGroup->addButton(ui->systemThemeCheckBox, 0);
//    themeGroup->addButton(ui->lightThemeCheckBox, 1);
//    themeGroup->addButton(ui->darkThemeCheckBox, 2);

//    QObject::connect(themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
//                     this, &FirstRunDialog::on_themeGroup_toggled);
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
            QMessageBox(QMessageBox::Warning, QStringLiteral("Error"),
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
        if(!finish())
            QDialog::reject();
        else
            QDialog::accept();
        return;
    }
    ui->stackedWidget->setCurrentIndex(current_idx + 1);
}

//void FirstRunDialog::on_themeGroup_toggled(int id)
//{
//    ASettings::write(ASettings::Main::Theme, id);
//}

bool FirstRunDialog::finish()
{

    ASettings::write(ASettings::FlightLogging::Function, ui->functionComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::Approach, ui->approachComboBox->currentText());
    ASettings::write(ASettings::FlightLogging::NightLogging, ui->nightComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::LogIFR, ui->rulesComboBox->currentIndex());
    ASettings::write(ASettings::FlightLogging::FlightNumberPrefix, ui->prefixLineEdit->text());
    ASettings::write(ASettings::FlightLogging::NumberTakeoffs, 1);
    ASettings::write(ASettings::FlightLogging::NumberLandings, 1);
    ASettings::write(ASettings::FlightLogging::PopupCalendar, true);
    ASettings::write(ASettings::FlightLogging::PilotFlying, true);
    ASettings::write(ASettings::FlightLogging::FlightTimeFormat, opl::time::Default);

    QMap<QString, QVariant> data;
    ASettings::write(ASettings::UserData::DisplaySelfAs, ui->aliasComboBox->currentIndex());
    data.insert(opl::db::PILOTS_LASTNAME, ui->lastnameLineEdit->text());
    data.insert(opl::db::PILOTS_FIRSTNAME, ui->firstnameLineEdit->text());
    data.insert(opl::db::PILOTS_ALIAS, QStringLiteral("self"));
    data.insert(opl::db::PILOTS_EMPLOYEEID, ui->employeeidLineEdit->text());
    data.insert(opl::db::PILOTS_PHONE, ui->phoneLineEdit->text());
    data.insert(opl::db::PILOTS_EMAIL, ui->emailLineEdit->text());

    QMessageBox db_fail_msg_box(QMessageBox::Critical, QStringLiteral("Database setup failed"),
                                       QStringLiteral("Errors have ocurred creating the database."
                                                      "Without a working database The application will not be usable."));
    if (!setupDatabase()) {
        db_fail_msg_box.exec();
        return false;
    }

    aDB->updateLayout();

    auto pilot = APilotEntry(1);
    pilot.setData(data);
    if(!aDB->commit(pilot)){
        db_fail_msg_box.exec();
        return false;
    }
    return true;
}

bool FirstRunDialog::setupDatabase()
{
    QMessageBox confirm(QMessageBox::Question, QStringLiteral("Create Database"),
                               QStringLiteral("We are now going to create the database.<br>"  // [G]: Why both <br> and \n ?
                                              "Would you like to download the latest database information?"
                                              "<br>(Recommended, Internet connection required)"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes)
        ADataBaseSetup::downloadTemplates();

    aDB->disconnect();
    ADataBaseSetup::backupOldData();
    aDB->connect();

    // [F]: todo: handle unsuccessful steps
    if(!ADataBaseSetup::createDatabase())
        return false;

    aDB->updateLayout();

    if(!ADataBaseSetup::importDefaultData())
        return false;
    return true;
}

void FirstRunDialog::reject()
{
    QMessageBox confirm(QMessageBox::Critical,
                               QStringLiteral("Setup incomplete"),
                               QStringLiteral("Without completing the initial setup"
                                              " you cannot use the application.<br><br>"
                                              "Quit anyway?"),
                               QMessageBox::Yes | QMessageBox::No, this);
    confirm.setDefaultButton(QMessageBox::No);

    if (confirm.exec() == QMessageBox::Yes) {
        DEB << "rejected.";
        QDialog::reject();
    }
}
